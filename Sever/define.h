#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Windows.h>
#include <time.h>
#include <stdbool.h>

#define BUFSIZ 1024 //�����С
//#define max_

#pragma pack(push,1)

typedef struct dns_head
{
	//��ʶ 16
	unsigned short ID;
	//��־ 16
	unsigned short FlaG;

	//������Ŀ 16*4
	unsigned short QDCOUNT;
	unsigned short ANCOUNT;
	unsigned short NSCOUNT;
	unsigned short ARCOUNT;

}DNS_HEAD;
//ID �ͺ������Ϊ�������ֽ��������������ʱ��Ҫ��ת��
#pragma pack(pop)
//#pragma pack(push, 1) �� #pragma pack(pop)��ʹ�ṹ�尴1�ֽڷ�ʽ���룬����push��ʾ��ԭ���Ķ��뷽ʽѹջ��pop��ʾ�ָ�ԭ���Ķ��뷽ʽ��

typedef struct dns_table//����DNSת����ṹ��
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
