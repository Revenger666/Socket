#include "udpSocket.h"
#include <stdio.h>
#pragma once


//�ο��̳� Socket������
//https://www.bilibili.com/video/BV1tb4y1C7qm

bool init_Socket()
{
	WSADATA wsadata;
	if(0 !=WSAStartup(MAKEWORD(2,2),&wsadata ))//����windows�첽�׽���
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
	//�����յ�Socket
	//����1 af ��ַЭ���� ipv4/ipv6   ����2 type ����Э������ ��ʽ�׽��� ���ݱ�  ����3  protocol ʹ�þ����ĳ������Э��
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

	//��socket��ip��ַ�Ͷ˿ں�
	/*struct sockaddr_in severaddr;
	struct sockaddr_in localaddr;
	struct sockaddr_in clientaddr;
	*/

	//int unBlock = 1;
	//ioctlsocket(seversk, FIONBIO, (u_long FAR*) & unBlock);//���ⲿ�׽ֿ�����Ϊ������
	//ioctlsocket(localsk, FIONBIO, (u_long FAR*) & unBlock);//�������׽ֿ�����Ϊ������

	severaddr.sin_family = AF_INET;//�ʹ���socketʱ����һ��
	severaddr.sin_port = htons(PORT);//�궨��˿�Ϊ53
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
