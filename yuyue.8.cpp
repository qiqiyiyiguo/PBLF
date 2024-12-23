#include <libwebsockets.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <pthread.h>
#include <time.h>
#include <ctype.h>
#define MAX_GUESTS 10
#define APPOINTMENT_EXPIRATION_DAYS 7  // ԤԼ��������

#define MAX_PAYLOAD_SIZE 512
#define USER_FILE "users.txt"  // �û���Ϣ�洢���ļ�
#define MAX_USERS 100  // ����û�����
#define LWS_SEND_BUFFER_POST_PADDING 0 // ����libwebsockets���ĵ������ֵ������0
#define APPOINTMENT_FILE "appointments.txt"  // ԤԼ��Ϣ�洢���ļ�
#define SESSION_TIMEOUT 600  // �Ự��ʱ��10���ӣ�
#define CHECK_INTERVAL 60  // �Ự�������ÿ60����һ�Σ�

// �û��ṹ��
typedef struct {
    char name[100];
    char number[100];
    char password[100];
    char is_guest[10];  // ��ʶ�Ƿ�Ϊ�ο�
} User;

// ԤԼ��Ϣ�ṹ��
typedef struct  {
    char name[100];
    char date[100];
    char time[100];
    char campus[100];
    char number[100];
    struct Appointment *next; 
} Appointment;
//��������ڳ���ʵ��һ������
//ȷ��Appointment�ṹ�����һ��ָ����һ��Appointment��ָ�롣
typedef struct AppointmentNode {
    Appointment data;
    struct AppointmentNode *next;
} AppointmentNode;

// �洢���л�û��Ự
typedef struct {
    struct lws *wsi;
    User user_data;  // ÿ���Ự��Ӧһ���û�
    time_t last_active;  // �Ự�����ʱ��
} UserSession;

//��������ṹ��
typedef struct list {//������һ����Ϊlist�Ľṹ�壬���ڴ洢�����ͷ�ڵ�ͳ���
    Appointment *head; //ָ������ͷ�ڵ��ָ��
    int size; //����ĳ���
} list;

User users[MAX_USERS];  // �û��б�
UserSession active_sessions[MAX_USERS];  // ��Ự�б�
int user_count = 0;  // ��ǰ�û�����
int active_session_count = 0;  // ��ǰ��Ự����
list *vis_list; //����

// ������������ͬ�����ʹ�������
pthread_mutex_t users_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t active_sessions_mutex = PTHREAD_MUTEX_INITIALIZER;

// ��������
void write_users(void);  // ����
void clean_expired_appointments();  // �����������ԤԼ�ĺ���
void clean_expired_sessions(void);

//��ʼ������
void init_list() {
    vis_list = (list *)malloc(sizeof(list)); //�����ڴ�ռ�
    if (vis_list == NULL) { //����ʧ��
        exit(1); //�˳�����
    }
    vis_list->head = NULL; //ͷ�ڵ�Ϊ��
    vis_list->size = 0; //������Ϊ0
    }

Appointment *find_node(char *id) {
    Appointment *node = vis_list->head; //��ͷ�ڵ㿪ʼ
    while (node != NULL) { //��������
        if (strcmp(node->number,id) == 0) { //ƥ��ID
            return node; //���ؽڵ�
        }
    }
    return NULL; //δ�ҵ�
}

// ����ɾ�����ڵ�ԤԼ
void clean_expired_appointments() {
    time_t current_time = time(NULL);
    struct tm now = *localtime(&current_time);
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
        struct tm appointment_time = {0};
        sscanf(file_date, "%d-%d-%d", &appointment_time.tm_year, &appointment_time.tm_mon, &appointment_time.tm_mday);
        time_t appointment_timestamp = mktime(&appointment_time);
        if (difftime(current_time, appointment_timestamp) > APPOINTMENT_EXPIRATION_DAYS * 86400) {
            found = 1;
        } else {
            fputs(line, temp_file);
        }
    }
    fclose(file);
    fclose(temp_file);
    if (found) {
        remove(APPOINTMENT_FILE);
        rename("appointments_temp.txt", APPOINTMENT_FILE);
        printf("Expired appointments cleaned.\n");
    } else {
        remove("appointments_temp.txt");
    }
}

// �Ự�����߳��е���
void *session_cleanup_thread(void *arg) {
    while (1) {
        sleep(CHECK_INTERVAL);  // ÿ60����һ��
        clean_expired_sessions();
        clean_expired_appointments();  // �������ԤԼ
    }
    return NULL;
}

// ������û�ʱ����ο�����
void add_user(const char *name, const char *number, const char *password) {
    pthread_mutex_lock(&users_mutex);  // �����û�����
    if (user_count < MAX_USERS) {
        int guest_count = 0;
        for (int i = 0; i < user_count; i++) {
            if (strcmp(users[i].is_guest, "yes") == 0) {
                guest_count++;
            }
        }
        if (strcmp(users[user_count].is_guest, "yes") == 0 && guest_count >= MAX_GUESTS) {
            printf("Max guest limit reached.\n");
        } else {
            strcpy(users[user_count].name, name);
            strcpy(users[user_count].number, number);
            strcpy(users[user_count].password, password);
            user_count++;
            write_users();  // �����ļ�
        }
    } else {
        printf("Max user limit reached.\n");
    }
    pthread_mutex_unlock(&users_mutex);  // ����
}

//�ͷ����� 
void free_list() {
   Appointment *node = vis_list->head; //��ͷ�ڵ㿪ʼ
   Appointment *temp; //��ʱ�ڵ�
    while (node != NULL) { //��������
        temp = node; //���浱ǰ�ڵ�
        node = node->next; //ָ����һ���ڵ�
        free(temp); //�ͷŵ�ǰ�ڵ�
    }
    free(vis_list); //�ͷ�����
}


// ��ȡ�ļ��е������û�
int read_users() {
    FILE *file = fopen(USER_FILE, "r");
    if (!file) {
        return 0;  // ����ļ������ڣ����� 0����ʾû���û�
    }

    int count = 0;
    while (fscanf(file, "%99s %99s %99s\n", users[count].name, users[count].number, users[count].password) == 3) {
        count++;
        if (count >= MAX_USERS) {
            break;  // ��ֹ��������û�����
        }
    }
    fclose(file);
    return count;
}

// ���û���Ϣд���ļ�
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

// �洢ԤԼ��Ϣ
void save_appointment(const Appointment *appointment) {
    FILE *file = fopen(APPOINTMENT_FILE, "a");
    if (!file) {
        printf("Unable to open appointment file for writing.\n");
        return;
    }
    fprintf(file, "%s %s %s %s\n", appointment->name, appointment->date, appointment->time, appointment->campus);
    fclose(file);
}

// ɾ��ԤԼ��Ϣ
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

// ��ѯԤԼ��Ϣ
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

//ԤԼ������ѯ
int query_appointmentcount(const char* date, const char* campus) {
    FILE* file = fopen(APPOINTMENT_FILE, "r");
    int count = 0;
    if (!file) {
        printf("Unable to open appointment file for reading.\n");
        return -1;
    }
    char line[1024];
    char file_date[100];
    char file_campus[100];
    while (fgets(line, sizeof(line), file)) {   //��ȡÿһ�е���Ϣ
        sscanf(line, "%50s %100s", file_date, file_campus);       //����Ϣ����
        if (strcmp(date, file_date) == 0 && strcmp(campus, file_campus) == 0) {  //�ȶ���Ϣ��ͬ
            count++;   //ԤԼ������1
        }
    }
    fclose(file);//�ر��ļ�
    return count;  //����ԤԼ����
}

// ������ڵĻỰ
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
            i--;  // ���¼�鵱ǰ����
        }
    }
    pthread_mutex_unlock(&active_sessions_mutex);
}

// �ж��û��Ƿ����
int user_exists(const char *name, const char *number, const char *password) {
    // �����û�����
    pthread_mutex_lock(&users_mutex);
    
    // ���ļ��ж�ȡ�û�����
    read_users();

    // �����û��б�����ƥ����û�
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].name, name) == 0 &&
            strcmp(users[i].number, number) == 0 &&
            strcmp(users[i].password, password) == 0) {
            // �û����ڣ�����������1
            pthread_mutex_unlock(&users_mutex);
            return 1;
        }
    }

    // �û������ڣ�����������0
    pthread_mutex_unlock(&users_mutex);
    return 0;
}


// ɾ���û�
void delete_user(const char *name, const char *number, const char *password) {
    pthread_mutex_lock(&users_mutex);  // �����û�����
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].name, name) == 0 && strcmp(users[i].number, number) == 0 && strcmp(users[i].password, password) == 0) {
            // ɾ���û���������û�ǰ��
            for (int j = i; j < user_count - 1; j++) {
                users[j] = users[j + 1];
            }
            user_count--;
            write_users();  // �����ļ�
            pthread_mutex_unlock(&users_mutex);  // ����
            return;
        }
    }
    pthread_mutex_unlock(&users_mutex);  // ����
}


//����ǿ�ȷ���
void send_password_strength_response(struct lws *wsi, const char *password_str) {
    int length = strlen(password_str);
    int has_lower = 0, has_upper = 0, has_digit = 0, has_special = 0;

    for (int i = 0; i < length; i++) {
        if (islower((unsigned char)password_str[i])) has_lower = 1;
        if (isupper((unsigned char)password_str[i])) has_upper = 1;
        if (isdigit((unsigned char)password_str[i])) has_digit = 1;
        if (!isalnum((unsigned char)password_str[i])) has_special = 1;
    }

    // ��������Ƿ���������������͵��ַ�
    int has_all_types = has_lower && has_upper && has_digit && has_special;

    const char *response;
    int len;

    if (length < 8 || !has_all_types) {
        response = "Weak";
    } else if (length >= 8 && length < 12) {
        response = "Medium";
    } else {
        response = "Strong";
    }

    len = strlen(response);
    char buf[LWS_PRE + 256 + LWS_SEND_BUFFER_POST_PADDING];// ���ӻ�������С������JSON��Ӧ
    sprintf(buf + LWS_PRE, "{\"passwordStrength\":\"%s\"}", response); // ����JSON��Ӧ
    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), strlen(buf + LWS_PRE), LWS_WRITE_TEXT); // ����JSON��Ӧ
}

// ���� WebSocket ����
static int callback_websocket(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
struct json_object *parsed_json;
struct json_object *name_json = NULL, *number_json = NULL, *password_json = NULL, *task_json = NULL, *date_json = NULL, *time_json = NULL, *campus_json = NULL;
switch (reason) {
    case LWS_CALLBACK_RECEIVE: {
        char msg[MAX_PAYLOAD_SIZE];
        memcpy(msg, in, len);
        msg[len] = '\0';

        printf("Received message: %s\n", msg);

        // �����յ��� JSON ����
        parsed_json = json_tokener_parse(msg);
        if (!parsed_json) {
            // JSON����ʧ�ܣ����ʹ�����Ϣ���ͻ���
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
                    // �û��Ѵ���
                    const char *response = "User already exists!";
                    int len = strlen(response);
                    int i=0;
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                } else {
                    // ע�����û�
                    add_user(name_str, number_str, password_str);
                    const char *response = "success";
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                    
                    // ���ú�����������ǿ����Ӧ
                    send_password_strength_response(wsi, password_str);
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
                    // ��¼�ɹ����洢�Ự
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
                    // �û�������
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
                    // ɾ���û�
                    delete_user(name_str, number_str, password_str);
                    const char *response = "User deleted successfully!";
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                } else {
                    // �û�������
                    const char *response = "User not found!";
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                }
            }
        }else if (strcmp(task_str, "checkLogin") == 0) {
                // ����û��Ƿ��ѵ�¼
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
                    // �û��ѵ�¼�������û���Ϣ
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
                    // �û�δ��¼
                    const char *response = "Not logged in";
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                }
            }else if (strcmp(task_str, "logout") == 0) {
                // �����û��ǳ�
                pthread_mutex_lock(&active_sessions_mutex);
                for (int i = 0; i < active_session_count; i++) {
                    if (active_sessions[i].wsi == wsi) {
                        // �ӻ�Ự�б����Ƴ�
                        for (int j = i; j < active_session_count - 1; j++) {
                            active_sessions[j] = active_sessions[j + 1];
                        }
                        active_session_count--;
                        i--;  // ���¼�鵱ǰ����
                        break;
                    }
                }
                pthread_mutex_unlock(&active_sessions_mutex);

                // ���͵ǳ��ɹ���Ϣ
                const char *response = "logged out";
                int len = strlen(response);
                char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                memcpy(buf + LWS_PRE, response, len);
                lws_write(wsi, (unsigned char *)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
            }else if (strcmp(task_str, "getAppointments") == 0) {
        // �����ȡԤԼ��Ϣ
        pthread_mutex_lock(&active_sessions_mutex);
        for (int i = 0; i < active_session_count; i++) {
            if (active_sessions[i].wsi == wsi) {
                // ����ԤԼ��Ϣ��ǰ��
                query_appointments(active_sessions[i].user_data.name, wsi);
                break;
            }
        }
        pthread_mutex_unlock(&active_sessions_mutex);
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

                int client = 0;
                pthread_mutex_lock(&active_sessions_mutex);
                client = query_appointmentcount(appointment.date, appointment.campus);
                pthread_mutex_unlock(&active_sessions_mutex);
                if (client <= 800) {
                    const char* response = "Appointment created successfully!";
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char*)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                }
                else {
                    const char* response = "Exceed the quantity!";
                    int len = strlen(response);
                    char buf[LWS_PRE + len + LWS_SEND_BUFFER_POST_PADDING];
                    memcpy(buf + LWS_PRE, response, len);
                    lws_write(wsi, (unsigned char*)(buf + LWS_PRE), len, LWS_WRITE_TEXT);
                }
            }
        } else if (strcmp(task_str, "query") == 0) {
            pthread_mutex_lock(&active_sessions_mutex);
            for (int i = 0; i < active_session_count; i++) {
                if (active_sessions[i].wsi == wsi) {
                    query_appointments(active_sessions[i].user_data.name, wsi);
                    break;
                }
            }
            pthread_mutex_unlock(&active_sessions_mutex);
        }else if (strcmp(task_str, "getUser") == 0) {
                // ����û��Ƿ��ѵ�¼
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
                    // �û��ѵ�¼�������û���Ϣ
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
                    // �û�δ��¼
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
        json_object_put(parsed_json);  // �ͷ� JSON ����
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

// WebSocket Э������
static struct lws_protocols protocols[] = {
{
"http",               // protocol name
callback_websocket,    // callback function
sizeof(void *)        // per-session data size
},
{ NULL, NULL, 0 } // terminator
};

// ������
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

// ����������ڻỰ���߳�
pthread_t cleanup_thread;
pthread_create(&cleanup_thread, NULL, session_cleanup_thread, NULL);

// �����¼�ѭ��
while (1) {
    lws_service(context, 100);  // �¼�ѭ��
}

lws_context_destroy(context);
return 0;
}

