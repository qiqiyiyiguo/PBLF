#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define COBJMACROS
#include <stdio.h>
#include <MsXml6.h>
#include <assert.h>
#include <time.h>
#include <Windows.h>
#include <stdlib.h>
#include "microhttpd.h"
// �û����ݿ⣬��ʾ��
#define DB_FILE "users.txt"


#define PORT 5500

#include <sys/stat.h>

#define MAX_LEN 100

int check_user(const char *username, const char *password) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        return 0;  // �ļ���ʧ��
    }

    char stored_username[MAX_LEN], stored_password[MAX_LEN];
    while (fscanf(file, "%s %s", stored_username, stored_password) != EOF) {
        if (strcmp(stored_username, username) == 0 && strcmp(stored_password, password) == 0) {
            fclose(file);
            return 1;  // �û���������ƥ��
        }
    }

    fclose(file);
    return 0;  // �û������������
}



//������һ����Ϊvisitor�Ľṹ�壬���ڴ洢�ο͵���Ϣ
typedef struct visitor {
    char id[20]; //����֤��
    char name[20]; //����
    char sex[2]; //�Ա�
    int age; //����
    char remark[100]; //��ע
    struct visitor *next; //ָ����һ���ڵ��ָ��
} visitor;

//���������ṹ��
typedef struct list {//������һ����Ϊlist�Ľṹ�壬���ڴ洢������ͷ�ڵ�ͳ���
    visitor *head; //ָ������ͷ�ڵ��ָ��
    int size; //�����ĳ���
} list;

//����ȫ�ֱ���
list *vis_list; //�ο�����
char filename[] = "visitor.txt"; //�����ο���Ϣ���ļ���
char password[] = "123456"; //�޸���Ϣ������

typedef struct {
    char username[50];
    char password[50];
} User;

// �����û����ݵ��ļ�
int save_user(const User *user) {
    FILE *file = fopen(DB_FILE, "a");
    if (file == NULL) {
        return -1;
    }
    fprintf(file, "%s,%s\n", user->username, user->password);
    fclose(file);
    return 0;
}

// ����û��Ƿ����
int check_user(const User *user) {
    FILE *file = fopen(DB_FILE, "r");
    if (file == NULL) {
        return -1;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        char stored_username[50], stored_password[50];
        sscanf(line, "%49[^,],%49s", stored_username, stored_password);
        if (strcmp(stored_username, user->username) == 0 &&
            strcmp(stored_password, user->password) == 0) {
            fclose(file);
            return 0; // �û����ڣ�������ȷ
        }
    }
    fclose(file);
    return -1; // �û������ڻ��������
}

// ����HTTP����
static int answer_to_request(void *cls, struct MHD_Connection *connection, const char *url, const char *method,
                             const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {
    if (strcmp(method, "POST") == 0) {
        char *response = NULL;
        int ret = -1;

        // ����POST����
        if (upload_data_size != NULL && *upload_data_size > 0) {
            User user;
            sscanf(upload_data, "username=%49[^&]&password=%49s", user.username, user.password);

            if (strstr(url, "/login") != NULL) {
                // ��¼����
                if (check_user(&user) == 0) {
                    response = "Login successful!";
                    ret = 200;
                } else {
                    response = "Invalid username or password!";
                    ret = 403; // ��¼ʧ��
                }
            } else if (strstr(url, "/register") != NULL) {
                // ע�����
                if (save_user(&user) == 0) {
                    response = "Registration successful!";
                    ret = 200;
                } else {
                    response = "Error saving user!";
                    ret = 500; // ע��ʧ��
                }
            }
        }

        if (response != NULL) {
            struct MHD_Response *mhd_response = MHD_create_response_from_buffer(strlen(response), (void*)response, MHD_RESPMEM_PERSISTENT);
            MHD_add_response_header(mhd_response, "Content-Type", "text/plain");
            int ret_code = MHD_queue_response(connection, ret, mhd_response);
            MHD_destroy_response(mhd_response);
            return ret_code;
        }
    }
    return MHD_NO;
}

int main() {
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, PORT, NULL, NULL, &answer_to_request, NULL, MHD_OPTION_END);
    if (NULL == daemon) {
        return 1;
    }
    printf("Server started on port %d\n", PORT);
    getchar(); // �ȴ��û������Թرշ�����
    MHD_stop_daemon(daemon);
    return 0;
}

//��������
void init_list(); //��ʼ������
void input_info(); //¼���ο���Ϣ
void display_info(); //��ʾ�ο���Ϣ
void save_info(); //�����ο���Ϣ
void delete_info(); //ɾ���ο���Ϣ
void modify_info(); //�޸��ο���Ϣ
void search_info(); //��ѯ�ο���Ϣ
void free_list(); //�ͷ�����
visitor *create_node(); //�����ڵ�
void insert_node(visitor *node); //����ڵ�
void delete_node(char *id); //ɾ���ڵ�
visitor *find_node(char *id); //���ҽڵ�
void print_node(visitor *node); //��ӡ�ڵ�
void print_menu(); //��ӡ�˵�
int check_password(); //�������
void modify_password(char *password);
int check_password_strength(char*password);//�������ǿ��

//������
int main() {

     char *data;
    char username[MAX_LEN], password[MAX_LEN];
    
    // ��ȡPOST����ı�������
    data = getenv("QUERY_STRING");
    if (data != NULL) {
        sscanf(data, "username=%s&password=%s", username, password);
        
        // URL ����
        for (int i = 0; username[i]; i++) if (username[i] == '+') username[i] = ' ';
        for (int i = 0; password[i]; i++) if (password[i] == '+') password[i] = ' ';
        
        // ��֤�û�
        if (check_user(username, password)) {
            printf("Content-Type: text/html\n\n");
            printf("<h1>��¼�ɹ���</h1>");
        } else {
            printf("Content-Type: text/html\n\n");
            printf("<h1>�û������������</h1>");
        }
    } else {
        printf("Content-Type: text/html\n\n");
        printf("<h1>�����ύʧ��</h1>");
    }
    
    return 0;

     // ����ļ�Ȩ��
    struct stat file_stat;
    if (stat("/usr/lib/cgi-bin/login.cgi", &file_stat) == 0) {
        printf("File permission: %o\n", file_stat.st_mode);
    } else {
        perror("Error getting file status");
    }

    // �����ļ�Ȩ��
    if (chmod("/usr/lib/cgi-bin/login.cgi", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0) {
        printf("File permission changed successfully.\n");
    } else {
        perror("Error changing file permission");
    }

    return 0;


    if(check_password())
    {   
        check_password_strength(password);
        char choice; //�û�ѡ��Ĺ�����
        init_list(); //��ʼ������
        while (1) {
            print_menu(); //��ӡ�˵�
            printf("��ѡ��ϵͳ�����\n");
            scanf("%c", &choice); //����ѡ��
            getchar(); //����������Ļ��з�
            switch (choice) {
                case 'a': //¼����Ϣ
                    input_info();
                    break;
                case 'b': //��ʾ��Ϣ
                    display_info();
                    break;
                case 'c': //������Ϣ
                    save_info();
                    break;
                case 'd': //ɾ����Ϣ
                    delete_info();
                    break;
                case 'e': //�޸���Ϣ
                    modify_info();
                    break;
                case 'f': //��ѯ��Ϣ
                    search_info();
                    break;
                case 'g': //�˳�ϵͳ
                    free_list(); //�ͷ�����
                    printf("��л��ʹ�ñ�ϵͳ���ټ���\n");
                    return 0; //��������
                case 'h': //�޸�����
                    modify_password(password);
                    break;
                default: //��Чѡ��
                    printf("\033[91m��Ч��ѡ�����������롣\033[0m\n");
                    break;
            }
            break;
        }
    }
}


//�����ڵ�
visitor *create_node() {
    visitor *node = (visitor *)malloc(sizeof(visitor)); //�����ڴ�ռ�
    if (node == NULL) { //����ʧ��
        printf("�ڴ����ʧ�ܣ��޷������ڵ㡣\n");
        exit(1); //�˳�����
    }
    node->next = NULL; //ָ��Ϊ��
    return node; //���ؽڵ�
}

//��ʼ������
void init_list() {
    vis_list = (list *)malloc(sizeof(list)); //�����ڴ�ռ�
    if (vis_list == NULL) { //����ʧ��
        printf("�ڴ����ʧ�ܣ��޷���ʼ��������\n");
        exit(1); //�˳�����
    }
    vis_list->head = NULL; //ͷ�ڵ�Ϊ��
    vis_list->size = 0; //��������Ϊ0
    FILE *fp = fopen(filename, "r"); //���ļ�
    if (fp == NULL) { //��ʧ��
        printf("�޷����ļ�%s�������ǵ�һ��ʹ�ñ�ϵͳ��\n", filename);
        return; //����
    }
    visitor *node; //��ʱ�ڵ�
    while (1) {
        node = create_node(); //�����ڵ�
        if (fscanf(fp, "%s %s %s %d %s", node->id, node->name, node->sex, &node->age, node->remark) == EOF) { //��ȡ�ļ����ڵ�
            free(node); //�ͷŽڵ�
            break; //����ѭ��
        }
        insert_node(node); //����ڵ�
    }
    fclose(fp); //�ر��ļ�
    printf("�Ѵ��ļ�%s�ж�ȡ�ο���Ϣ��\n", filename);
}

//����ڵ�
void insert_node(visitor *node) {
    if (vis_list->head == NULL) { //����Ϊ��
        vis_list->head = node; //ͷ�ڵ�Ϊ�½ڵ�
    } else { //������Ϊ��
        visitor *p = vis_list->head; //��ͷ�ڵ㿪ʼ
        while (p->next != NULL) { //��������
            p = p->next; //ָ����һ���ڵ�
        }
        p->next = node; //β�ڵ��ָ��ָ���½ڵ�
    }
    vis_list->size++; //�������ȼ�һ
}


//¼��ѧ����Ϣ
void input_info() {
    printf("�������ο͵Ļ�����Ϣ���Կո�ָ����Իس�������\n");
    printf("��ʽ������֤�� ���� �Ա� ���� ��ע\n");
    visitor *node = create_node(); //�����ڵ�
    scanf("%s %s %s %d %s", node->id, node->name, node->sex, &node->age, node->remark); //������Ϣ���ڵ�
    getchar(); //����������Ļ��з�
    if (find_node(node->id) != NULL) { //�����Ƿ��Ѵ�����ͬѧ�ŵĽڵ�
        printf("������֤���Ѵ��ڣ��޷�¼�롣\n");
        free(node); //�ͷŽڵ�
        return; //����
    }
    insert_node(node); //����ڵ�
    printf("�ѳɹ�¼���ο���Ϣ��\n");
}

//��ʾѧ����Ϣ
void display_info() {
    if (vis_list->size == 0) { //����Ϊ��
        printf("û���ο���Ϣ������ʾ��\n");
        return; //����
    }
    printf("�����������ο͵Ļ�����Ϣ��\n");
    visitor *node = vis_list->head; //��ͷ�ڵ㿪ʼ
    while (node != NULL) { //��������
        print_node(node); //��ӡ�ڵ�
        node = node->next; //ָ����һ���ڵ�
    }
}

//����ѧ����Ϣ
void save_info() {
    if (vis_list->size == 0) { //����Ϊ��
        printf("û���ο���Ϣ���Ա��档\n");
        return; //����
    }
    FILE *fp = fopen(filename, "w+"); //���ļ�
    if (fp == NULL) { //��ʧ��
        printf("�޷����ļ�%s���޷������ο���Ϣ��\n", filename);
        return; //����
    }
    visitor *node = vis_list->head; //��ͷ�ڵ㿪ʼ
    while (node != NULL) { //��������
        fprintf(fp, "%s %s %s %d %s\n", node->id, node->name, node->sex, node->age, node->remark); //д���ļ�
        node = node->next; //ָ����һ���ڵ�
    }
    fclose(fp); //�ر��ļ�
    printf("�ѳɹ������ο���Ϣ���ļ�%s��\n", filename);
}

//ɾ���ο���Ϣ
void delete_info() {
    if (vis_list->size == 0) { //����Ϊ��
        printf("û���ο���Ϣ����ɾ����\n");
        return; //����
    }
    char id[20]; //Ҫɾ��������֤��
    printf("������Ҫɾ�����ο͵�����֤�ţ�\n");
    scanf("%s", id); //��������֤��
    getchar(); //����������Ļ��з�
    visitor *node = find_node(id); //���ҽڵ�
    if (node == NULL) { //δ�ҵ�
        printf("û���ҵ�������֤�ŵ��οͣ��޷�ɾ����\n");
        return; //����
    }
    delete_node(id); //ɾ���ڵ�
    printf("�ѳɹ�ɾ���ο���Ϣ��\n");
}

//�޸��ο���Ϣ
void modify_info() {
    if (vis_list->size == 0) { //����Ϊ��
        printf("û���ο���Ϣ�����޸ġ�\n");
        return; //����
    }
    check_password(); //�������
    char id[20]; //Ҫ�޸ĵ�����֤��
    printf("������Ҫ�޸ĵ��ο͵�����֤�ţ�\n");
    scanf("%s", id); //����ѧ��
    getchar(); //����������Ļ��з�
    visitor *node = find_node(id); //���ҽڵ�
    if (node == NULL) { //δ�ҵ�
        printf("û���ҵ�������֤�ŵ��οͣ��޷��޸ġ�\n");
        return; //����
    }
    printf("�������ο͵�����Ϣ���Կո�ָ����Իس�������\n");
    printf("��ʽ������֤�� ���� �Ա� ���� ��ע\n");
    scanf("%s %s %s %d %s", node->id, node->name, node->sex, &node->age, node->remark); //��������Ϣ���ڵ�
    getchar(); //����������Ļ��з�
    printf("�ѳɹ��޸��ο���Ϣ��\n");
}

//��ѯ�ο���Ϣ
void search_info() {
    if (vis_list->size == 0) { //����Ϊ��
        printf("û���ο���Ϣ���Բ�ѯ��\n");
        return; //����
    }
    char choice; //�û�ѡ��Ĳ�ѯ��ʽ
    printf("��ѡ���ѯ��ʽ��\n");
    printf("(1)������֤�Ų�ѯ\n");
    printf("(2)��������ѯ\n");
    printf("(3)���Ա��ѯ\n");
    printf("(4)�������ѯ\n");
    scanf("%c", &choice); //����ѡ��
    getchar(); //����������Ļ��з�
    char key[20]; //��ѯ�ؼ���
    int count = 0; //��ѯ���������
    switch (choice) {
        case '1': //������֤�Ų�ѯ
            printf("������Ҫ��ѯ��ѧ�ţ�\n");
            scanf("%s", key); //����ѧ��
            getchar(); //����������Ļ��з�
            visitor *node = find_node(key); //���ҽڵ�
            if (node == NULL) { //δ�ҵ�
                printf("û���ҵ�������֤���ο͡�\n");
                return; //����
            }
            printf("�����ǲ�ѯ�����\n");
            print_node(node); //��ӡ�ڵ�
            break;
        case '2': //��������ѯ
            printf("������Ҫ��ѯ��������\n");
            scanf("%s", key); //��������
            getchar(); //����������Ļ��з�
            visitor *p = vis_list->head; //��ͷ�ڵ㿪ʼ
            printf("�����ǲ�ѯ�����\n");
            while (p != NULL) { //��������
                if (strcmp(p->name, key) == 0) { //ƥ������
                    print_node(p); //��ӡ�ڵ�
                    count++; //������һ
                }
                p = p->next; //ָ����һ���ڵ�
            }
            if (count == 0) { //û��ƥ����
                printf("û���ҵ����������ο͡�\n");
            }
            break;
        case '3': //���Ա��ѯ
            printf("������Ҫ��ѯ���Ա�\n");
            scanf("%s", key); //�����Ա�
            getchar(); //����������Ļ��з�
            visitor *q = vis_list->head; //��ͷ�ڵ㿪ʼ
            printf("�����ǲ�ѯ�����\n");
            while (q != NULL) { //��������
                if (strcmp(q->sex, key) == 0) { //ƥ���Ա�
                    print_node(q); //��ӡ�ڵ�
                    count++; //������һ
                }
                q = q->next; //ָ����һ���ڵ�
            }
            if (count == 0) { //û��ƥ����
                printf("û���ҵ����Ա���ο͡�\n");
            }
            break;
        case '4': //�������ѯ
            printf("������Ҫ��ѯ�����䣺\n");
            scanf("%s", key); //��������
            getchar(); //����������Ļ��з�
            int age = atoi(key); //ת��Ϊ����
            visitor *r = vis_list->head; //��ͷ�ڵ㿪ʼ
            printf("�����ǲ�ѯ�����\n");
            while (r != NULL) { //��������
                if (r->age == age) { //ƥ������
                    print_node(r); //��ӡ�ڵ�
                    count++; //������һ
                }
                r = r->next; //ָ����һ���ڵ�
            }
            if (count == 0) { //û��ƥ����
                printf("û���ҵ���������ο͡�\n");
            }
            break;
        default: //��Чѡ��
            printf("��Ч��ѡ�����������롣\n");
    }
}


//�ͷ�����
void free_list() {
    visitor *node = vis_list->head; //��ͷ�ڵ㿪ʼ
    visitor *temp; //��ʱ�ڵ�
    while (node != NULL) { //��������
        temp = node; //���浱ǰ�ڵ�
        node = node->next; //ָ����һ���ڵ�
        free(temp); //�ͷŵ�ǰ�ڵ�
    }
    free(vis_list); //�ͷ�����
}


//ɾ���ڵ�
void delete_node(char *id) {
    if (vis_list->head == NULL) { //����Ϊ��
        printf("����Ϊ�գ��޷�ɾ���ڵ㡣\n");
        return; //����
    }
    visitor *p = vis_list->head; //��ͷ�ڵ㿪ʼ
    visitor *prev = NULL; //ǰһ���ڵ�
    while (p != NULL) { //��������
        if (strcmp(p->id, id) == 0) { //ƥ��ѧ��
            if (prev == NULL) { //ͷ�ڵ�
                vis_list->head = p->next; //ͷ�ڵ�ָ����һ���ڵ�
            } else { //��ͷ�ڵ�
                prev->next = p->next; //ǰһ���ڵ��ָ��ָ����һ���ڵ�
            }
            free(p); //�ͷŽڵ�
            vis_list->size--; //�������ȼ�һ
            return; //����
        }
        prev = p; //���浱ǰ�ڵ�
        p = p->next; //ָ����һ���ڵ�
    }
}

//���ҽڵ�
visitor *find_node(char *id) {
    visitor *node = vis_list->head; //��ͷ�ڵ㿪ʼ
    while (node != NULL) { //��������
        if (strcmp(node->id, id) == 0) { //ƥ��ѧ��
            return node; //���ؽڵ�
        }
        node = node->next; //ָ����һ���ڵ�
    }
    return NULL; //δ�ҵ�
}

//��ӡ�ڵ�
void print_node(visitor *node) {
    printf("����֤�ţ�%s\n", node->id);
    printf("������%s\n", node->name);
    printf("�Ա�%s\n", node->sex);
    printf("���䣺%d\n", node->age);
    printf("��ע��%s\n", node->remark);
    printf("**********\n");
}

//��ӡ�˵�
void print_menu() {
    printf("��ѡ��ϵͳ�����\n");
    printf("a   �οͻ�����Ϣ¼��\n");
    printf("b   �οͻ�����Ϣ��ʾ\n");
    printf("c   �οͻ�����Ϣ����\n");
    printf("d   �οͻ�����Ϣɾ��\n");
    printf("e   �οͻ�����Ϣ�޸ģ�Ҫ�����������룩\n");
    printf("f   �οͻ�����Ϣ��ѯ\n");
    printf("g   �˳�ϵͳ\n");
    printf("h   �޸�����(�����µ�½ϵͳ)\n");
}

//�������
int check_password() {
    char input[20]; //�û����������
    int count = 0; //���Դ���
    while (1) {
        printf("���������룺\n");
        scanf("%s", input); //��������
        getchar(); //����������Ļ��з�
        if (strcmp(input, password) == 0) { //������ȷ
            printf("������ȷ����ӭ����ϵͳ��\n");
            return 1; //����ѭ��
        } else { //�������
            printf("����������������롣\n");
            count++; //������һ
            if (count == 3) { //���Դ����ﵽ3��
                printf("���Ѿ������������3�Σ��޷�����ϵͳ��\n");
                exit(EXIT_FAILURE); //�˳�����
            }
        }
    }
}

int check_password_strength(char*password) {//������ǿ�ȵļ��
    int length = strlen(password);
    int has_lower = 0;
    int has_upper = 0;
    int has_digit = 0;
    int has_special = 0;

    for (int i = 0; i < length; i++) {
        if(islower(password[i])) {
            has_lower = 1;
        }
        if(isupper(password[1])) {
            has_upper = 1;
        }
        if (isdigit(password[i])) {
            has_digit = 1;
        }
        if (!isalnum(password[i])) {
            has_special = 1;
        }
    }
    if (length < 8 || !(has_lower && has_upper && has_digit && has_special)) {
        return 0;//��
    } else if (length < 12 || !(has_lower && has_upper && has_digit && has_special)) {
        return 1;//�е�
    } else {
        return 2;//ǿ
    }
}


void modify_password(char *password)
{
    int count = 0;
    char verify_password[20];
    char temp[20];
    printf("������ԭ�ȵ�����: \n");
    scanf("%s",verify_password);
    getchar(); // ����������Ļ��з�
    while(1)
    {
        if (strcmp(verify_password, password) == 0)
        { // ������ȷ
            printf("�������µ����룺\n");
            scanf("%s",temp);
            strcpy(password,temp);
            break; // ����ѭ��
        } 
        else{ // �������
            printf("����������������롣\n");
            count++; 
            // ������һ
            if (count == 3)
            { // ���Դ����ﵽ3��
                printf("���������������3�Σ��޷��޸����롣\n");
                return; // �˳�����
            }
            printf("������ԭ�ȵ�����: \n");
            scanf("%s",verify_password);
            getchar(); // ����������Ļ��з�
        }
    }
    check_password();
}

