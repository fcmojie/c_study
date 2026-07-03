#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8888

int main(){

	int socket_fd;

	if(socket_fd = socket(AF_INET, SOCK_STREAM, 0) == -1){
		perror("创建socket错误");
		return -1;
	}

	struct sockaddr_in address;
	address.sin_family = AF_INET;		//必须和socket第一个参数保持一致
	address.sin_addr.s_addr = INADDR_ANY;	//监听本机所有网卡
	address.sin_port = htons(PORT);

	if(bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) == -1){
		perror("bind失败");
		return -1;
	}

	listen(socket_fd, 128);

	while(1) {
		int client_fd = accept(socket_fd, (struct sockaddr*)&address, sizeof(address));

	}
}
