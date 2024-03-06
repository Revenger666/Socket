#ifndef _UDPSOCKET_H_
#define _UDPSOCKET_H_
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS//不能少，不然inet_addr会报错

#include <WinSock2.h>             //windows平台的网络库头文件
#pragma comment(lib,"Ws2_32.lib") //库文件
#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Windows.h>
#include <time.h>
#include <stdbool.h>


#define PORT 53
#define SEVER_DNS_ADDR "10.3.9.44"
#define LOCAL_DNS_ADDR "127.0.0.1"


SOCKADDR_IN severaddr, localaddr, clientaddr;

SOCKET localsk;
SOCKET seversk;


//打开网络库
bool init_Socket();

//关闭网络库
bool close_Socket();

//创建服务器socket
void create_socket();

#endif 
