#include "udpSocket.h"
#include <stdio.h>
#pragma once


//参考教程 Socket网络编程
//https://www.bilibili.com/video/BV1tb4y1C7qm

bool init_Socket()
{
	WSADATA wsadata;
	if(0 !=WSAStartup(MAKEWORD(2,2),&wsadata ))//启动windows异步套接字
	{
		printf("WSAStartup failed code %d",WSAGetLastError());
		return false;
	}
	return true;
}

bool close_Socket()
{
	if (0 != WSACleanup())
	{
		printf("WSACleanup failed code %d", WSAGetLastError());
		return false;
	}
	return true;
}

void create_socket()
{
	//创建空的Socket
	//参数1 af 地址协议族 ipv4/ipv6   参数2 type 传输协议类型 流式套接字 数据报  参数3  protocol 使用具体的某个传输协议
	localsk = socket(AF_INET,SOCK_DGRAM,0);
	if (localsk == INVALID_SOCKET)
	{
		printf("local socket create faild.\n");
		//return INVALID_SOCKET;
	}

	seversk = socket(AF_INET, SOCK_DGRAM, 0);
	if (seversk == INVALID_SOCKET)
	{
		printf("sever socket create faild.\n");
		//return INVALID_SOCKET;
	}

	//给socket绑定ip地址和端口号
	/*struct sockaddr_in severaddr;
	struct sockaddr_in localaddr;
	struct sockaddr_in clientaddr;
	*/

	//int unBlock = 1;
	//ioctlsocket(seversk, FIONBIO, (u_long FAR*) & unBlock);//将外部套街口设置为非阻塞
	//ioctlsocket(localsk, FIONBIO, (u_long FAR*) & unBlock);//将本地套街口设置为非阻塞

	severaddr.sin_family = AF_INET;//和创建socket时必须一样
	severaddr.sin_port = htons(PORT);//宏定义端口为53
	severaddr.sin_addr.s_addr = inet_addr(SEVER_DNS_ADDR);
	
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(PORT);
	localaddr.sin_addr.s_addr = inet_addr(LOCAL_DNS_ADDR);
	//localaddr.sin_addr.s_addr = ADDR_ANY;
	
	if (SOCKET_ERROR == bind(localsk, (SOCKADDR*) & localaddr, sizeof(localaddr)))
	{
		printf("Failed to bind port 53.\n");
	}
	else
	{
		printf("Success to bind port 53.\n");
	}

}
