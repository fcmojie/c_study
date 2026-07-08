#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define MAX_NAME_SIZE 256

int main(int argc, char const *argv[]) {
	int sock = 0;	//客户端socket文件描述符
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE + MAX_NAME_SIZE + 1] = {0};
	char name[MAX_NAME_SIZE] = {0};
	fd_set read_fds;

	// 创建socket
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket creation error");
		return -1;
	}

	// 设置服务端地址
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		perror("inet_pton");
		return -1;
	}

	printf("请先输入您的用户名：");
	fgets(name, sizeof(name), stdin);
	name[strcspn(name, "\n")] = '\0';

	// 连接服务器
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr))  == -1) {
		perror("connect faild");
		return -1;
	}
	send(sock, name, strlen(name), 0);
	printf("[客户端] 连接服务器成功！可以开始聊天了(输入exit断开)\n");

	//聊天循环
	while (1)
	{
		FD_ZERO(&read_fds);
		FD_SET(STDIN_FILENO, &read_fds);
		FD_SET(sock, &read_fds);

		if (select(sock + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			break;
		}

		// 键盘输入
		if (FD_ISSET(STDIN_FILENO, &read_fds)) {
			fgets(buffer, BUFFER_SIZE, stdin);
			buffer[strcspn(buffer, "\n")] = '\0';
			if (strcmp(buffer, "exit") == 0) {
				printf("[客户端] 断开连接...\n");
				send(sock, buffer, strlen(buffer), 0);
				break;
			}
			send(sock, buffer, strlen(buffer), 0);
		}

		// 收到服务端消息
		if (FD_ISSET(sock, &read_fds)) {
			int valread = recv(sock, buffer, BUFFER_SIZE, 0);
			if (valread <= 0) {
				printf("[客户端] 服务器已断开连接\n");
				break;
			}
			buffer[valread] = '\0';
			printf("%s\n", buffer);
		}
	}
	close(sock);
	return 0;
}
