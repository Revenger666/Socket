#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Windows.h>
#include <time.h>
#include <stdbool.h>

#define BUFSIZ 1024 //缓存大小
//#define max_

#pragma pack(push,1)

typedef struct dns_head
{
	//标识 16
	unsigned short ID;
	//标志 16
	unsigned short FlaG;

	//各种数目 16*4
	unsigned short QDCOUNT;
	unsigned short ANCOUNT;
	unsigned short NSCOUNT;
	unsigned short ARCOUNT;

}DNS_HEAD;
//ID 和后面的因为是网络字节序，所以填充内容时候要做转换
#pragma pack(pop)
//#pragma pack(push, 1) 和 #pragma pack(pop)。使结构体按1字节方式对齐，其中push表示把原来的对齐方式压栈，pop表示恢复原来的对齐方式。

typedef struct dns_table//本地DNS转换表结构体
{
	char *IP;
	char* domain;
}DNS_TABLE;

typedef struct transID
{
	unsigned short old;
	SOCKADDR_IN client_addr;
	bool done;
}ID_TRANS;
