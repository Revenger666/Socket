#ifndef _UDPSOCKET_H_
#define _UDPSOCKET_H_
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS//�����٣���Ȼinet_addr�ᱨ��

#include <WinSock2.h>             //windowsƽ̨�������ͷ�ļ�
#pragma comment(lib,"Ws2_32.lib") //���ļ�
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


//�������
bool init_Socket();

//�ر������
bool close_Socket();

//����������socket
void create_socket();

#endif 
