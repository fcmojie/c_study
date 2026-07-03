#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
	int sock = 0;	//客户端socket文件描述符
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE] = [0];
	fd_set read_fds;


}
