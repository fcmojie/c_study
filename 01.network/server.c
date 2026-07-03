#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main(){

	int server_fd;
	int opt = 1;
	fd_set read_fds;	// select使用的文件描述符集合

	// 创建套接字AF_INET->ipv4地址族，SOCK_STREAM->流式套接字，对应tcp，0->自动选择（TCP/UDP）
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("创建socket错误");
		exit(EXIT_FAILURE);
	}

	// 设置端口复用，SOL_SOCKET通用套接字层，SO_REUSEADDR允许重用本地地址（端口），&opt (1) 启用
	// 如果不配置这个选项，系统会保留端口一段时间，这样重新启动就会报错：Address already in use
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}


	// 准备地址结构体
	struct sockaddr_in address;
	address.sin_family = AF_INET;		//必须和socket第一个参数保持一致
	address.sin_addr.s_addr = INADDR_ANY;	//监听本机所有网卡
	address.sin_port = htons(PORT);		// 将主机字节序转换为网络字节序
						
	// 绑定socket到制定ip和端口
	if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1){
		perror("bind失败");
		exit(EXIT_FAILURE);
	}

	// 开启监听
	listen(server_fd, 128);
	printf("[服务器] 已启动，监听端口 %d，等待客户端连接...\n", PORT);
	
	int addrlen = sizeof(address);

	// 从已连接的队列中取出第一个连接，创建一个新的三哦此刻他与该客户端通信
	int client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

	printf("[服务器] 客户端 (%s:%d) 已连接！现在可以互发消息了（输入 exit 断开）\n", 
			inet_ntoa(address.sin_addr), ntohs(address.sin_port));

	while(1) {
		// 每次循环都要清空集合重新设置
		FD_ZERO(&read_fds);
	}
}
