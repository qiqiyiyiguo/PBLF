#include <libwebsockets.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <pthread.h>
#include <time.h>

#define MAX_PAYLOAD_SIZE 512
#define USER_FILE "users.txt"  // 用户信息存储的文件
#define MAX_USERS 100  // 最大用户数量
#define LWS_SEND_BUFFER_POST_PADDING 0 // 根据libwebsockets的文档，这个值现在是0
#define APPOINTMENT_FILE "appointments.txt"  // 预约信息存储的文件
#define SESSION_TIMEOUT 600  // 会话超时（10分钟）
#define CHECK_INTERVAL 60  // 会话检查间隔（每60秒检查一次）

// 用户结构体
typedef struct {
    char name[100];
    char number[100];
    char password[100];
} User;

// 预约信息结构体
typedef struct {
    char name[100];
    char date[100];
    char time[100];
    char campus[100];
} Appointment;

// 存储所有活动用户会话
typedef struct {
    struct lws *wsi;
    User user_data;  // 每个会话对应一个用户
    time_t last_active;  // 会话的最后活动时间
} UserSession;

User users[MAX_USERS];  // 用户列表
UserSession active_sessions[MAX_USERS];  // 活动会话列表
int user_count = 0;  // 当前用户数量
int active_session_count = 0;  // 当前活动会话数量

// 互斥锁，用于同步访问共享数据
pthread_mutex_t users_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t active_sessions_mutex = PTHREAD_MUTEX_INITIALIZER;

// 读取文件中的所有用户
int read_users() {
    FILE *file = fopen(USER_FILE, "r");
    if (!file) {
        return 0;  // 如果文件不存在，返回 0，表示没有用户
    }

    int count = 0;
    while (fscanf(file, "%99s %99s %99s\n", users[count].name, users[count].number, users[count].password) == 3) {
        count++;
        if (count >= MAX_USERS) {
            break;  // 防止超过最大用户数量
        }
    }
    fclose(file);
    return count;
}

// 将用户信息写入文件
void write_users() {
    FILE *file = fopen(USER_FILE, "w");
    if (!file) {
        printf("Unable to open file for writing.\n");
        return;
    }

    for (int i = 0; i < user_count; i++) {
        fprintf(file, "%s %s %s\n", users[i].name, users[i].number, users[i].password);
    }

    fclose(file);
}

// 存储预约信息
void save_appointment(const Appointment *appointment) {
    FILE *file = fopen(APPOINTMENT_FILE, "a");
    if (!file) {
        printf("Unable to open appointment file for writing.\n");
        return;
    }
    fprintf(file, "%s %s %s %s\n", appointment->name, appointment->date, appointment->time, appointment->campus);
    fclose(file);
}

// 删除预约信息
void delete_appointment(const char *name, const char *date, const char *time) {
    FILE *file = fopen(APPOINTMENT_FILE, "r");
    FILE *temp_file = fopen("appointments_temp.txt", "w");
    if (!file || !temp_file) {
        printf("Unable to open appointment file for reading/writing.\n");
        return;
    }
    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char file_name[100], file_date[100], file_time[100], file_campus[100];
        sscanf(line, "%s %s %s %s", file_name, file_date, file_time, file_campus);
        if (strcmp(file_name, name) != 0 || strcmp(file_date, date) != 0 || strcmp(file_time, time) != 0) {
            fputs(line, temp_file);
        } else {
            found = 1;
        }
    }
    fclose(file);
    fclose(temp_file);
    if (found) {
        remove(APPOINTMENT_FILE);
        rename("appointments_temp.txt", APPOINTMENT_FILE);
        printf("Appointment deleted successfully.\n");
    } else {
        printf("Appointment not found.\n");
        remove("appointments_temp.txt");
    }
}

// 查询预约信息
void query_appointments(const char *name, struct lws *wsi) {
    FILE *file = fopen(APPOINTMENT_FILE, "r");
    if (!file) {
        printf("Unable to open appointment file for reading.\n");
        return;
    }
    char response[2048] = "Appointments:\n";
    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char file_name[100], file_date[100], file_time[100], file_campus[100];
        sscanf(line, "%s %s %s %s", file_name, file_date, file_time, file_campus);
        if (strcmp(file_name, name) == 0) {
            found = 1;
            strcat(response, line);
        }
    }
    fclose(file);
    if (found) {
        int len = strlen(response);
        char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
        memcpy(buf + LWS_PRE, response, len);
        lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
    } else {
        const char *response = "No appointments found!";
        int len = strlen(response);
        char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
        memcpy(buf + LWS_PRE, response, len);
        lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
    }
}

// 清理过期的会话
void clean_expired_sessions() {
    time_t current_time = time(NULL);
    pthread_mutex_lock(&active_sessions_mutex);
    for (int i = 0; i < active_session_count; i++) {
        if (current_time - active_sessions[i].last_active > SESSION_TIMEOUT) {
            printf("Session expired: %s\n", active_sessions[i].user_data.name);
            for (int j = i; j < active_session_count - 1; j++) {
                active_sessions[j] = active_sessions[j + 1];
            }
            active_session_count--;
            i--;  // 重新检查当前索引
        }
    }
    pthread_mutex_unlock(&active_sessions_mutex);
}

// 会话清理线程
void *session_cleanup_thread(void *arg) {
    while (1) {
        sleep(CHECK_INTERVAL);  // 每60秒检查一次
        clean_expired_sessions();
    }
    return NULL;
}

// 判断用户是否存在
int user_exists(const char *name, const char *number, const char *password) {
    // 锁定用户数据
    pthread_mutex_lock(&users_mutex);
    
    // 从文件中读取用户数据
    read_users();

    // 遍历用户列表，查找匹配的用户
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].name, name) == 0 &&
            strcmp(users[i].number, number) == 0 &&
            strcmp(users[i].password, password) == 0) {
            // 用户存在，解锁并返回1
            pthread_mutex_unlock(&users_mutex);
            return 1;
        }
    }

    // 用户不存在，解锁并返回0
    pthread_mutex_unlock(&users_mutex);
    return 0;
}

// 添加新用户
void add_user(const char *name, const char *number, const char *password) {
    pthread_mutex_lock(&users_mutex);  // 锁定用户数据
    if (user_count < MAX_USERS) {  // 检查是否超出最大用户数量
        strcpy(users[user_count].name, name);
        strcpy(users[user_count].number, number);
        strcpy(users[user_count].password, password);
        user_count++;
        write_users();  // 更新文件
    } else {
        printf("Max user limit reached.\n");
    }
    pthread_mutex_unlock(&users_mutex);  // 解锁
}

// 删除用户
void delete_user(const char *name, const char *number, const char *password) {
    pthread_mutex_lock(&users_mutex);  // 锁定用户数据
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].name, name) == 0 && strcmp(users[i].number, number) == 0 && strcmp(users[i].password, password) == 0) {
            // 删除用户，后面的用户前移
            for (int j = i; j < user_count - 1; j++) {
                users[j] = users[j + 1];
                        }
        user_count--;
        write_users();  // 更新文件
        pthread_mutex_unlock(&users_mutex);  // 解锁
        return;
    }
}
pthread_mutex_unlock(&users_mutex);  // 解锁
}

// 处理 WebSocket 连接
static int callback_websocket(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
struct json_object *parsed_json;
struct json_object *name_json = NULL, *number_json = NULL, *password_json = NULL, *task_json = NULL, *date_json = NULL, *time_json = NULL, *campus_json = NULL;
switch (reason) {
    case LWS_CALLBACK_RECEIVE: {
        char msg[MAX_PAYLOAD_SIZE];
        memcpy(msg, in, len);
        msg[len] = '\0';

        printf("Received message: %s\n", msg);

        // 解析收到的 JSON 数据
        parsed_json = json_tokener_parse(msg);
        if (!parsed_json) {
            // JSON解析失败，发送错误消息给客户端
            const char *response = "Invalid JSON received!";
            int len = strlen(response);
            char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
            memcpy(buf + LWS_PRE, response, len);
            lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
            break;
        }

        if (!json_object_object_get_ex(parsed_json, "task", &task_json)) {
            json_object_put(parsed_json);
            const char *response = "Missing task!";
            int len = strlen(response);
            char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
            memcpy(buf + LWS_PRE, response, len);
            lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
            break;
        }

        const char *task_str = json_object_get_string(task_json);

        if (strcmp(task_str, "register") == 0) {
            if (json_object_object_get_ex(parsed_json, "name", &name_json) &&
                json_object_object_get_ex(parsed_json, "number", &number_json) &&
                json_object_object_get_ex(parsed_json, "password", &password_json)) {
                const char *name_str = json_object_get_string(name_json);
                const char *number_str = json_object_get_string(number_json);
                const char *password_str = json_object_get_string(password_json);

                if (user_exists(name_str, number_str, password_str)) {
                    // 用户已存在
                    const char *response = "User already exists!";
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                } else {
                    // 注册新用户
                    add_user(name_str, number_str, password_str);
                    const char *response = "success";
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                }
            }
        } else if (strcmp(task_str, "login") == 0) {
            if (json_object_object_get_ex(parsed_json, "name", &name_json) &&
                json_object_object_get_ex(parsed_json, "number", &number_json) &&
                json_object_object_get_ex(parsed_json, "password", &password_json)) {
                const char *name_str = json_object_get_string(name_json);
                const char *number_str = json_object_get_string(number_json);
                const char *password_str = json_object_get_string(password_json);
                if (user_exists(name_str, number_str, password_str)) {
                    // 登录成功，存储会话
                    pthread_mutex_lock(&active_sessions_mutex);
                    active_sessions[active_session_count].wsi = wsi;
                    strcpy(active_sessions[active_session_count].user_data.name, name_str);
                    strcpy(active_sessions[active_session_count].user_data.number, number_str);
                    strcpy(active_sessions[active_session_count].user_data.password, password_str);
                    active_sessions[active_session_count].last_active = time(NULL);
                    active_session_count++;
                    pthread_mutex_unlock(&active_sessions_mutex);
                    char response[256];
                sprintf(response, "{\"name\":\"%s\", \"number\":\"%s\"}", name_str, number_str);
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                } else {
                    // 用户不存在
                    const char *response = "User not found!";
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                }
            }
        } else if (strcmp(task_str, "delete") == 0) {
            if (json_object_object_get_ex(parsed_json, "name", &name_json) &&
                json_object_object_get_ex(parsed_json, "number", &number_json) &&
                json_object_object_get_ex(parsed_json, "password", &password_json)) {
                const char *name_str = json_object_get_string(name_json);
                const char *number_str = json_object_get_string(number_json);
                const char *password_str = json_object_get_string(password_json);
                if (user_exists(name_str, number_str, password_str)) {
                    // 删除用户
                    delete_user(name_str, number_str, password_str);
                    const char *response = "User deleted successfully!";
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                } else {
                    // 用户不存在
                    const char *response = "User not found!";
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                }
            }
        }else if (strcmp(task_str, "checkLogin") == 0) {
                // 检查用户是否已登录
                pthread_mutex_lock(&active_sessions_mutex);
                int is_logged_in = 0;
                for (int i = 0; i < active_session_count; i++) {
                    if (active_sessions[i].wsi == wsi) {
                        is_logged_in = 1;
                        break;
                    }
                }
                pthread_mutex_unlock(&active_sessions_mutex);

                if (is_logged_in) {
                    // 用户已登录，发送用户信息
                    for (int i = 0; i < active_session_count; i++) {
                        if (active_sessions[i].wsi == wsi) {
                            const char *response="success";
                            int len = strlen(response);
                            char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                            memcpy(buf + LWS_PRE, response, len);
                            lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                            break;
                        }
                    }
                } else {
                    // 用户未登录
                    const char *response = "Not logged in";
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                }
            }else if (strcmp(task_str, "logout") == 0) {
                // 处理用户登出
                pthread_mutex_lock(&active_sessions_mutex);
                for (int i = 0; i < active_session_count; i++) {
                    if (active_sessions[i].wsi == wsi) {
                        // 从活动会话列表中移除
                        for (int j = i; j < active_session_count - 1; j++) {
                            active_sessions[j] = active_sessions[j + 1];
                        }
                        active_session_count--;
                        i--;  // 重新检查当前索引
                        break;
                    }
                }
                pthread_mutex_unlock(&active_sessions_mutex);

                // 发送登出成功消息
                const char *response = "logged out";
                int len = strlen(response);
                char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                memcpy(buf + LWS_PRE, response, len);
                lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
            }else if (strcmp(task_str, "getAppointments") == 0) {
        // 处理获取预约信息
        
        for (int i = 0; i < active_session_count; i++) {
            if (active_sessions[i].wsi == wsi) {
                // 发送预约信息给前端
                query_appointments(active_sessions[i].user_data.name, wsi);
                break;
            }
        }
        
            }else if (strcmp(task_str, "appoint") == 0) {
            if (json_object_object_get_ex(parsed_json, "date", &date_json) &&
                json_object_object_get_ex(parsed_json, "time", &time_json) &&
                json_object_object_get_ex(parsed_json, "campus", &campus_json)) {
                const char *date_str = json_object_get_string(date_json);
                const char *time_str = json_object_get_string(time_json);
                const char *campus_str = json_object_get_string(campus_json);

                Appointment appointment;
                pthread_mutex_lock(&active_sessions_mutex);
                for (int i = 0; i < active_session_count; i++) {
                    if (active_sessions[i].wsi == wsi) {
                        strcpy(appointment.name, active_sessions[i].user_data.name);
                        break;
                    }
                }
                pthread_mutex_unlock(&active_sessions_mutex);

                strcpy(appointment.date, date_str);
                strcpy(appointment.time, time_str);
                strcpy(appointment.campus, campus_str);
                save_appointment(&appointment);

                const char *response = "Appointment created successfully!";
                int len = strlen(response);
                char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                memcpy(buf + LWS_PRE, response, len);
                lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
            }
        }else if (strcmp(task_str, "getUser") == 0) {
                // 检查用户是否已登录
                
                int is_logged_in = 0;
                for (int i = 0; i < active_session_count; i++) {
                    if (active_sessions[i].wsi == wsi) {
                        is_logged_in = 1;
                        break;
                    }
                }
                

                if (is_logged_in) {
                    // 用户已登录，发送用户信息
                    for (int i = 0; i < active_session_count; i++) {
                        if (active_sessions[i].wsi == wsi) {
                            char response[256];
                            sprintf(response, "%s %s", active_sessions[i].user_data.name, active_sessions[i].user_data.number);
                            char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                            memcpy(buf + LWS_PRE, response, len);
                            lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                            break;
                        }
                    }
                } else {
                    // 用户未登录
                    const char *response = "Not logged in";
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                }
                }else if (strcmp(task_str, "deleteAppointment") == 0) {
                    if (json_object_object_get_ex(parsed_json, "date", &date_json) &&
                json_object_object_get_ex(parsed_json, "time", &time_json)) {
                const char *date_str = json_object_get_string(date_json);
                const char *time_str = json_object_get_string(time_json);

                pthread_mutex_lock(&active_sessions_mutex);
                for (int i = 0; i < active_session_count; i++) {
                    if (active_sessions[i].wsi == wsi) {
                        delete_appointment(active_sessions[i].user_data.name, date_str, time_str);
                        break;
                    }
                }
                pthread_mutex_unlock(&active_sessions_mutex);
}
} else {
const char *response = "Unknown task!";
int len = strlen(response);
char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
memcpy(buf + LWS_PRE, response, len);
lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
}
        json_object_put(parsed_json);  // 释放 JSON 对象
        break;
    }
    case LWS_CALLBACK_ESTABLISHED:
        printf("New WebSocket connection established!\n");
        break;
    case LWS_CALLBACK_CLOSED:
        printf("WebSocket connection closed.\n");
        break;
    default:
        break;
}

return 0;
}

// WebSocket 协议配置
static struct lws_protocols protocols[] = {
{
"http",               // protocol name
callback_websocket,    // callback function
sizeof(void *)        // per-session data size
},
{ NULL, NULL, 0 } // terminator
};

// 主函数
int main() {
struct lws_context_creation_info info;
struct lws_context *context;
memset(&info, 0, sizeof(info));

info.port = 8080;
info.protocols = protocols;
info.gid = -1;
info.uid = -1;

context = lws_create_context(&info);
if (context == NULL) {
    fprintf(stderr, "WebSocket server failed to start.\n");
    return -1;
}

printf("WebSocket server started on port 8080\n");

// 创建清理过期会话的线程
pthread_t cleanup_thread;
pthread_create(&cleanup_thread, NULL, session_cleanup_thread, NULL);

// 运行事件循环
while (1) {
    lws_service(context, 100);  // 事件循环
}

lws_context_destroy(context);
return 0;
}