#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>

#define BUFFER_SIZE 2048

typedef struct {
    char *name;
    int client_fd;
}talk_client;


int main(int argc, char const *argv[]) {
    // printf("argc:%d", argc);
    // printf("argv: %s", argv[1]);

    int server_fd;
    int opt = 1;
    char buffer[BUFFER_SIZE] = {0};
    fd_set read_fds; //select 使用的文件描述符
    talk_client clients[128];

    char *portstr = "";
    if (argc >= 2) {
        portstr = argv[1];
    }
    int port = atoi(portstr);
    if (port == 0) {
        exit(EXIT_FAILURE);
    }

    if (server_fd = socket(AF_INET, SOCK_STREAM, 0) == -1) {
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



    return 0;
}