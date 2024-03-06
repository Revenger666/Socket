#include "../tcpScoket/udpSocket.h"
#include "../Sever/define.h"

#include <stdio.h>
#pragma once

DNS_TABLE dns_trans[2000];//DNS转换表
char domain_find[64];//被查询的域名
ID_TRANS id_trans_table[20000];//ID转换表

int read_dns_table();//读入本地解析表的函数
void get_domain(int recv_data,char * recvbuf);//从客户端收到的查询报文中找到要查询的domain
int find_domain(int table_num);//在本地的解析表中查询
unsigned short trans_ID(unsigned short old,SOCKADDR_IN addr,bool done);//将请求ID换为新ID
void printinfo(int result);//打印函数


int main()
{
	init_Socket();
	create_socket();
	//localsk,seversk
	
	char sendbuf[BUFSIZ];//发送缓存
	char recvbuf[BUFSIZ];//接收缓存
	int recv_data;//接受到的数据按字节计算的长度
	int send_data;
	

	//读取本地保存的dns解析列表
	int table_num;//解析表中的条数
	table_num = read_dns_table();//读取
	//printf("before while\n");


	while (1)
	{
		
		int timeout_flag = 0;//1表示超时了
		memset(recvbuf, 0, BUFSIZ);//接收缓存清空
		int clientaddr_long = sizeof(clientaddr);//clientaddr的长度
		recv_data = recvfrom(localsk, recvbuf, BUFSIZ, 0, (SOCKADDR*)&clientaddr, &clientaddr_long);

		//错误
		if (recv_data == SOCKET_ERROR)
		{
			printf("recv failed, begain the next recv.\n");
			continue;
		}
		if(recv_data == 0) 
		{
			break;
		}

		//读取到报文
		else
		{
			printf("get a dns request\n");

			//在报文中获取域名
			
			get_domain(recv_data,recvbuf);
			//GetUrl(recvbuf, recv_data);
			//printf("%s",domain_find);
			//在保存的DNS解析表中寻找该域名
			int result = find_domain(table_num);
			//在解析表中找到了域名
			if (result >= 0)
			{
				//printf("find!!!!!!!!! in line:%d\n", result);
				//构造响应报文的报文头
				//ID:与收到的相同
				unsigned short* ans_ID = (unsigned short*)malloc(sizeof(unsigned short*));
				memcpy(ans_ID, recvbuf, sizeof(unsigned short));
				//转换ID，使得符合网络发送的形式
				unsigned short ans_ID_trans = trans_ID(ntohs(*ans_ID), clientaddr, FALSE);
				
				printinfo(result);

				memcpy(sendbuf, recvbuf, recv_data);//将请求报文拷贝到发送缓存中

				//标志字段：响应报文，递归求法，支持递归查询的服务器
				//1000 0001 1000 0000分别对应报文头部FLAG标志字段
				unsigned short new_flag = htons(0x8180);
				memcpy(&sendbuf[2], &new_flag, sizeof(unsigned short));

				//回答数域，若查询得到IP为0.0.0.0则回答为0，否则为1
				if (strcmp(dns_trans[result].IP, "0.0.0.0") != 0)
				{
					new_flag = htons(0x0001);
				}
				else
				{
					new_flag = htons(0x0000);
				}
				memcpy(&sendbuf[6], &new_flag, sizeof(unsigned short));

				//数据部分
				//参考：http://c.biancheng.net/view/6457.html
				int len = 0;//临时使用的长度

				char ans[100];
				unsigned short Name = htons(0xc00c); //域名指针（偏移量）
				memcpy(ans, &Name, sizeof(unsigned short));
				len += sizeof(unsigned short);

				unsigned short A = htons(0x0001); //类型
				memcpy(ans + len, &A, sizeof(unsigned short));
				len += sizeof(unsigned short);

				memcpy(ans + len, &A, sizeof(unsigned short));//类
				len += sizeof(unsigned short);

				unsigned long time_to_live = htonl(0x80); //生存时间,设置为128
				memcpy(ans + len, &time_to_live, sizeof(unsigned long));
				len += sizeof(unsigned long);

				unsigned short RDLength = htons(0x0004); //资源数据长度
				memcpy(ans + len, &RDLength, sizeof(unsigned short));
				len += sizeof(unsigned short);

				unsigned long IP = (unsigned long)inet_addr(dns_trans[result].IP); //inet_addr为IP地址转化函数
				memcpy(ans + len, &IP, sizeof(unsigned long));
				len += sizeof(unsigned long);
				
				//len 最后为发回去的总长度
				memcpy(sendbuf + recv_data, ans, len);
				len += recv_data;
				
				send_data = sendto(localsk, sendbuf, len, 0, (SOCKADDR*)&clientaddr, sizeof(clientaddr));


				free(ans_ID);
			}

			//在解析表中没有找到域名，进行转发
			else
			{
				clock_t start, end;
				clock_t timeout = 5000;

				//ID:与收到的相同
				unsigned short* ans_ID = (unsigned short*)malloc(sizeof(unsigned short*));
				memcpy(ans_ID, recvbuf, sizeof(unsigned short));
				//转换ID，使得符合网络发送的形式
				unsigned short ans_ID_trans = trans_ID(ntohs(*ans_ID), clientaddr, FALSE);
				printf("%u\n", ans_ID_trans);

				memcpy(recvbuf, &ans_ID_trans, sizeof(unsigned short));//将新ID拷贝到接收缓存中
				
				printinfo(result);
				//因为是转发功能，所以只改报头的ID就行了
				send_data = sendto(seversk, recvbuf, recv_data, 0, (SOCKADDR*)&severaddr, sizeof(severaddr));
				start = clock();
				if (send_data == SOCKET_ERROR)
				{
					printf("failed to send data to server.\n");
					continue;
				}
				else if (send_data == 0)
				{
					break;
				}

				free(ans_ID);
				
				//接收外部服务器的回答报文
			
				recv_data = recvfrom(seversk, recvbuf, sizeof(recvbuf), 0, (SOCKADDR*)&clientaddr, &clientaddr_long);
				while ((recv_data == 0) || (recv_data == SOCKET_ERROR))
				{
					//memset(recvbuf, 0, BUFSIZ);
					//当收到的外部报文错误或者没有收到时，持续接收
					recv_data = recvfrom(seversk, recvbuf, sizeof(recvbuf), 0, (SOCKADDR*)&clientaddr, &clientaddr_long);
					end = clock();
					if (end - start > timeout)
					{
						printf("Timeout to recv from server.\n");
						timeout_flag = 1;
						break;
					}
				}
				
				if (timeout_flag == 0)
				{

					//ID:与收到的相同
					unsigned short* ans_ID = (unsigned short*)malloc(sizeof(unsigned short*));
					
					memcpy(ans_ID, recvbuf, sizeof(unsigned short));
					//printf("%u\n", *ans_ID);
					int getid = *ans_ID;					
					//printf("getid=%d\n", getid);
					//把原来从客户端发过来的ID再安回去
					unsigned short old_ID = htons(id_trans_table[getid].old);
					memcpy(recvbuf, &old_ID, sizeof(unsigned short));
		
					id_trans_table[getid].done = TRUE;

					printinfo(result);
					//找到发过来这个请求的客户端（处理并发）
					clientaddr = id_trans_table[getid].client_addr;
					
					send_data = sendto(localsk, recvbuf, recv_data, 0, (SOCKADDR*)&clientaddr, sizeof(clientaddr));
					if (send_data == SOCKET_ERROR)
					{
						printf("failed to send back to client.\n");
						continue;
					}
					else if (send_data == 0) break;
					
					free(ans_ID);
				}
			}
		}
	}
	closesocket(seversk);
	closesocket(localsk);
	close_Socket();
	return 0;
}



int read_dns_table()//读取本地的dns解析列表文件
{
	int table_num = 0;//表示列表长度

	FILE* fp = fopen("dnsrelay.txt", "ab+");//ab+表示允许读取一个文本文件并在他的末尾追加数据

	//打开文件失败
	if (!fp)
	{
		printf("open dnsrelay.txt failed.\n");
		return false;
	}

	char* temp[2000];//指针数组，每个指针指向文件中每一行的开头
	//先按行分割
	int len = 0;
	for (int i = 0; i < 1999; i++)
	{
		temp[i] = (char*)malloc(sizeof(char) * 200);
		
		//fgets功能是从 fp流中读取 size(1000) 个字符存储到字符指针变量 temp[i] 所指向的内存空间。它的返回值是一个指针，指向字符串中第一个字符的地址
		if (fgets(temp[i], 1000, fp) == NULL) break;// 分行读入，一直到读完
		table_num++;
	}

	//将分好的每一行分为IP和网址
	for (int i = 0; i < table_num; i++)
	{
		//strtok分割函数
		dns_trans[i].IP = strtok(temp[i], " ");
		dns_trans[i].domain = strtok(NULL, " ");
	}

	//关闭文件
	fclose(fp);
	printf("read dnsrelay.txt success.\n");

	return table_num;
}

void get_domain(int recv_data, char* recvbuf)
{
	char temp_domain_find[64];

	memset(domain_find, 0, 64);
	//memcpy:以参数2指向的地址为起点，将连续的参数3个字节数据，复制到以参数1指向的地址为起点的内存中
	
	memcpy(temp_domain_find, &(recvbuf[sizeof(DNS_HEAD)]), recv_data - 12);//查询问题长度为整个收到的长度-12

	int question_len = strlen(temp_domain_find);

	//域名的构成 3www5baidu3com 数字+该长度的字符+数字+该长度的字符
	int i = 0,j=0,k=0;
	while (i < question_len)
	{
		if (temp_domain_find[i] < 64 && temp_domain_find[i]>0)
		{
			for (j = temp_domain_find[i], i++; j > 0; j--, i++, k++)
				domain_find[k] = temp_domain_find[i];

			if (temp_domain_find[i] != 0)
			{
				domain_find[k] = '.';
				k++;
			}
		}
	}
	domain_find[k] = '\0';
	//printf("domainname=%s\n", domain_find);
	return;
}


int find_domain(int table_num)//在本地列表中查找该域名，找到就返回行数(0到长度-1），没找到就返回-1
{
	char* temp;
	temp = (char*)malloc(sizeof(char) * 210);
	strcpy(temp, domain_find);
	strcat(temp, "\r\n");

	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	for (int i =0 ; i < table_num; i++)
	{
		if (strcmp(dns_trans[i].domain, temp) == 0)
		{
			return i;
		}
	}
	return -1;
}

int idnum = 0;//已经转换的ID数-1

unsigned short trans_ID(unsigned short old, SOCKADDR_IN addr, bool done)
{
	id_trans_table[idnum].old = old;
	id_trans_table[idnum].client_addr = addr;
	id_trans_table[idnum].done = done;
	idnum++;
	return (unsigned short)(idnum - 1);
}

void printinfo(int result)
{
	SYSTEMTIME currentTime;
	GetSystemTime(&currentTime);

	//输出时间
	printf("time: %u/%u/%u %u:%u:%u:%u ",
		currentTime.wYear, currentTime.wMonth, currentTime.wDay,
		currentTime.wHour, currentTime.wMinute, currentTime.wSecond,
		currentTime.wMilliseconds);
	printf("	");

	if (result == -1)
	{
		printf("中继	");
		printf("%s\n", domain_find);

	}
	else
	{
		if (strcmp(dns_trans[result].IP, "0.0.0.0") == 0)
		{
			printf("屏蔽	");
			printf("%s	", domain_find);
			printf("0.0.0.0 \n");
		}
		else
		{
			printf("服务器	");
			printf("%s	", domain_find);
			printf("%s	\n", dns_trans[result].IP);
		}
	}


	return;
}