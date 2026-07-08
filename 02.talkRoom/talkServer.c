#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>

#define BUFFER_SIZE 2048

#define MAX_NAME_SIZE 256

#define MAX_CLIENT_NUM 128

typedef struct {
    char name[256];
    int client_fd;
    int is_online; // 1在线，0断开
}talk_client;

void initClients(talk_client clients[]) {
    for(int i = 0; i < MAX_CLIENT_NUM; i++) {
        clients[i].client_fd = -1;
        clients[i].is_online = 0;
        memset(clients[i].name, 0, sizeof(clients[i].name)); 
    }
}

int add_client(int fd, const char *name, talk_client clients[]) {
    for (int i = 0; i < MAX_CLIENT_NUM; i++) {
        if (clients[i].is_online == 0) {
            clients[i].client_fd = fd;
            clients[i].is_online = 1;
            strncpy(clients[i].name, name, MAX_NAME_SIZE - 1);  // 安全拷贝，防止溢出
            clients[i].name[MAX_NAME_SIZE - 1] = '\0';          // 确保结尾有 '\0'
            printf("[系统] %s (fd=%d) 加入了聊天室\n", clients[i].name, fd);
            return 0;
        }
    }
    printf("[系统] 聊天室已满！\n");
    return -1;
}

void remove_client_by_index(int index, talk_client clients[]) {
        if ( clients[index].is_online == 1) {
            printf("[系统] %s 已退出聊天室\n", clients[index].name);
            clients[index].is_online = 0;
            close(clients[index].client_fd); // 关闭 socket
            clients[index].client_fd = -1; 
            return;
        }
}

// 根据名字查找fd，找不到返回-1
int find_fd_by_name(const char *name, talk_client clients[]) {
    for (int i = 0; i < MAX_CLIENT_NUM; i++) {
        if (clients[i].is_online == 1 && strcmp(clients[i].name, name) == 0) {
            return clients[i].client_fd;
        }
    }
    return -1;
}

// 获取当前在线人数 
int get_online_count(talk_client clients[]) {
    int count = 0;
    for (int i = 0; i < MAX_CLIENT_NUM; i++) {
        if (clients[i].is_online == 1) count++;
    }
    return count;
}

// 主函数
int main(int argc, char const *argv[]) {
    // printf("argc:%d", argc);
    // printf("argv: %s", argv[1]);

    int server_fd;
    int opt = 1;
    char buffer[BUFFER_SIZE] = {0};
    fd_set read_fds; //select 使用的文件描述符
    talk_client clients[MAX_CLIENT_NUM];

    initClients(clients);

    char portstr[4] = {0};
    int port = 8888;
    if (argc >= 2) {
        strcpy(portstr, argv[1]);
        port = atoi(portstr);
        if (port == 0) {
            exit(EXIT_FAILURE);
        }
    }
    

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("创建socket错误");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind失败");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 128) == -1) {
        perror("listen ");
        exit(EXIT_FAILURE);
    }
    printf("[服务器] 已启动，监听端口 %d，等待客户端连接...\n", port);

    int addrlen = sizeof(address);

    int max_fd = -1;

    // 取出连接
    while(1) {
        // 每次循环都要清空集合重新设置
		FD_ZERO(&read_fds);

		FD_SET(STDIN_FILENO, &read_fds);    // 监听键盘输入（文件描述符0）
        FD_SET(server_fd, &read_fds);       // 监听服务socket是否有新客户端连接
        max_fd = server_fd;

        // 将所有在线的客户端的文件描述符放进去
        for (int i = 0; i < MAX_CLIENT_NUM; i++) {
            if (clients[i].is_online == 1) {
                FD_SET(clients[i].client_fd, &read_fds);
                if (clients[i].client_fd > max_fd) {
                    max_fd = clients[i].client_fd;
                }
            }
        }

        // select 会阻塞，直到有事件发生
		if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			break;
		}

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            if (strcmp(buffer, "exit") == 0) {
                printf("服务器关闭\n");
                for (int i = 0; i < MAX_CLIENT_NUM; i++) { 
                    if (clients[i].is_online == 1) {
                        send(clients[i].client_fd, buffer, strlen(buffer), 0);
                    }
                }   
                break;
            }
        }
        
        if (FD_ISSET(server_fd, &read_fds)) {
            int new_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
            // 接收客户端发来的名字
            char name[MAX_NAME_SIZE] = {0};
            recv(new_fd, name, MAX_NAME_SIZE, 0);
            name[strcspn(name, "\n")] = '\0';
            FD_SET(new_fd, &read_fds);
            add_client(new_fd,name, clients);
            continue;
        }

        for (int i = 0; i < MAX_CLIENT_NUM; i++) {
            if (clients[i].is_online == 1) {
                if (!FD_ISSET(clients[i].client_fd, &read_fds)) {
                    continue;
                }
                int valread = recv(clients[i].client_fd, buffer, BUFFER_SIZE, 0);
                // 返回值小于等于0说明客户端已经断开连接
                if (valread <= 0) {
                    remove_client_by_index(i, clients);
                    continue;
                }
                // 将消息发送到其他连接上的客户端
                buffer[valread] = '\0';
                char tempBuffer[MAX_NAME_SIZE + BUFFER_SIZE + 1] = {0};
                strcat(tempBuffer, clients[i].name);
                strcat(tempBuffer, ":");
                strcat(tempBuffer, buffer);
                for (int j = 0; j < MAX_CLIENT_NUM; j++) {
                    if (j == i) {
                        continue;
                    }
                    if (clients[j].is_online == 1) {
                        send(clients[j].client_fd, tempBuffer, strlen(tempBuffer), 0);
                    }
                }
            }
        }
    }

    // 服务器关闭，关闭剩下socket
    for (int i = 0; i < MAX_CLIENT_NUM; i++) { 
        if (clients[i].is_online == 1) {
            remove_client_by_index(i, clients);
        }
    }
    close(server_fd);

    return 0;
}