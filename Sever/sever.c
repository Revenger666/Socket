#include "../tcpScoket/udpSocket.h"
#include "../Sever/define.h"

#include <stdio.h>
#pragma once

DNS_TABLE dns_trans[2000];//DNSת����
char domain_find[64];//����ѯ������
ID_TRANS id_trans_table[20000];//IDת����

int read_dns_table();//���뱾�ؽ�����ĺ���
void get_domain(int recv_data,char * recvbuf);//�ӿͻ����յ��Ĳ�ѯ�������ҵ�Ҫ��ѯ��domain
int find_domain(int table_num);//�ڱ��صĽ������в�ѯ
unsigned short trans_ID(unsigned short old,SOCKADDR_IN addr,bool done);//������ID��Ϊ��ID
void printinfo(int result);//��ӡ����


int main()
{
	init_Socket();
	create_socket();
	//localsk,seversk
	
	char sendbuf[BUFSIZ];//���ͻ���
	char recvbuf[BUFSIZ];//���ջ���
	int recv_data;//���ܵ������ݰ��ֽڼ���ĳ���
	int send_data;
	

	//��ȡ���ر����dns�����б�
	int table_num;//�������е�����
	table_num = read_dns_table();//��ȡ
	//printf("before while\n");


	while (1)
	{
		
		int timeout_flag = 0;//1��ʾ��ʱ��
		memset(recvbuf, 0, BUFSIZ);//���ջ������
		int clientaddr_long = sizeof(clientaddr);//clientaddr�ĳ���
		recv_data = recvfrom(localsk, recvbuf, BUFSIZ, 0, (SOCKADDR*)&clientaddr, &clientaddr_long);

		//����
		if (recv_data == SOCKET_ERROR)
		{
			printf("recv failed, begain the next recv.\n");
			continue;
		}
		if(recv_data == 0) 
		{
			break;
		}

		//��ȡ������
		else
		{
			printf("get a dns request\n");

			//�ڱ����л�ȡ����
			
			get_domain(recv_data,recvbuf);
			//GetUrl(recvbuf, recv_data);
			//printf("%s",domain_find);
			//�ڱ����DNS��������Ѱ�Ҹ�����
			int result = find_domain(table_num);
			//�ڽ��������ҵ�������
			if (result >= 0)
			{
				//printf("find!!!!!!!!! in line:%d\n", result);
				//������Ӧ���ĵı���ͷ
				//ID:���յ�����ͬ
				unsigned short* ans_ID = (unsigned short*)malloc(sizeof(unsigned short*));
				memcpy(ans_ID, recvbuf, sizeof(unsigned short));
				//ת��ID��ʹ�÷������緢�͵���ʽ
				unsigned short ans_ID_trans = trans_ID(ntohs(*ans_ID), clientaddr, FALSE);
				
				printinfo(result);

				memcpy(sendbuf, recvbuf, recv_data);//�������Ŀ��������ͻ�����

				//��־�ֶΣ���Ӧ���ģ��ݹ��󷨣�֧�ֵݹ��ѯ�ķ�����
				//1000 0001 1000 0000�ֱ��Ӧ����ͷ��FLAG��־�ֶ�
				unsigned short new_flag = htons(0x8180);
				memcpy(&sendbuf[2], &new_flag, sizeof(unsigned short));

				//�ش���������ѯ�õ�IPΪ0.0.0.0��ش�Ϊ0������Ϊ1
				if (strcmp(dns_trans[result].IP, "0.0.0.0") != 0)
				{
					new_flag = htons(0x0001);
				}
				else
				{
					new_flag = htons(0x0000);
				}
				memcpy(&sendbuf[6], &new_flag, sizeof(unsigned short));

				//���ݲ���
				//�ο���http://c.biancheng.net/view/6457.html
				int len = 0;//��ʱʹ�õĳ���

				char ans[100];
				unsigned short Name = htons(0xc00c); //����ָ�루ƫ������
				memcpy(ans, &Name, sizeof(unsigned short));
				len += sizeof(unsigned short);

				unsigned short A = htons(0x0001); //����
				memcpy(ans + len, &A, sizeof(unsigned short));
				len += sizeof(unsigned short);

				memcpy(ans + len, &A, sizeof(unsigned short));//��
				len += sizeof(unsigned short);

				unsigned long time_to_live = htonl(0x80); //����ʱ��,����Ϊ128
				memcpy(ans + len, &time_to_live, sizeof(unsigned long));
				len += sizeof(unsigned long);

				unsigned short RDLength = htons(0x0004); //��Դ���ݳ���
				memcpy(ans + len, &RDLength, sizeof(unsigned short));
				len += sizeof(unsigned short);

				unsigned long IP = (unsigned long)inet_addr(dns_trans[result].IP); //inet_addrΪIP��ַת������
				memcpy(ans + len, &IP, sizeof(unsigned long));
				len += sizeof(unsigned long);
				
				//len ���Ϊ����ȥ���ܳ���
				memcpy(sendbuf + recv_data, ans, len);
				len += recv_data;
				
				send_data = sendto(localsk, sendbuf, len, 0, (SOCKADDR*)&clientaddr, sizeof(clientaddr));


				free(ans_ID);
			}

			//�ڽ�������û���ҵ�����������ת��
			else
			{
				clock_t start, end;
				clock_t timeout = 5000;

				//ID:���յ�����ͬ
				unsigned short* ans_ID = (unsigned short*)malloc(sizeof(unsigned short*));
				memcpy(ans_ID, recvbuf, sizeof(unsigned short));
				//ת��ID��ʹ�÷������緢�͵���ʽ
				unsigned short ans_ID_trans = trans_ID(ntohs(*ans_ID), clientaddr, FALSE);
				printf("%u\n", ans_ID_trans);

				memcpy(recvbuf, &ans_ID_trans, sizeof(unsigned short));//����ID���������ջ�����
				
				printinfo(result);
				//��Ϊ��ת�����ܣ�����ֻ�ı�ͷ��ID������
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
				
				//�����ⲿ�������Ļش���
			
				recv_data = recvfrom(seversk, recvbuf, sizeof(recvbuf), 0, (SOCKADDR*)&clientaddr, &clientaddr_long);
				while ((recv_data == 0) || (recv_data == SOCKET_ERROR))
				{
					//memset(recvbuf, 0, BUFSIZ);
					//���յ����ⲿ���Ĵ������û���յ�ʱ����������
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

					//ID:���յ�����ͬ
					unsigned short* ans_ID = (unsigned short*)malloc(sizeof(unsigned short*));
					
					memcpy(ans_ID, recvbuf, sizeof(unsigned short));
					//printf("%u\n", *ans_ID);
					int getid = *ans_ID;					
					//printf("getid=%d\n", getid);
					//��ԭ���ӿͻ��˷�������ID�ٰ���ȥ
					unsigned short old_ID = htons(id_trans_table[getid].old);
					memcpy(recvbuf, &old_ID, sizeof(unsigned short));
		
					id_trans_table[getid].done = TRUE;

					printinfo(result);
					//�ҵ��������������Ŀͻ��ˣ���������
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



int read_dns_table()//��ȡ���ص�dns�����б��ļ�
{
	int table_num = 0;//��ʾ�б���

	FILE* fp = fopen("dnsrelay.txt", "ab+");//ab+��ʾ�����ȡһ���ı��ļ���������ĩβ׷������

	//���ļ�ʧ��
	if (!fp)
	{
		printf("open dnsrelay.txt failed.\n");
		return false;
	}

	char* temp[2000];//ָ�����飬ÿ��ָ��ָ���ļ���ÿһ�еĿ�ͷ
	//�Ȱ��зָ�
	int len = 0;
	for (int i = 0; i < 1999; i++)
	{
		temp[i] = (char*)malloc(sizeof(char) * 200);
		
		//fgets�����Ǵ� fp���ж�ȡ size(1000) ���ַ��洢���ַ�ָ����� temp[i] ��ָ����ڴ�ռ䡣���ķ���ֵ��һ��ָ�룬ָ���ַ����е�һ���ַ��ĵ�ַ
		if (fgets(temp[i], 1000, fp) == NULL) break;// ���ж��룬һֱ������
		table_num++;
	}

	//���ֺõ�ÿһ�з�ΪIP����ַ
	for (int i = 0; i < table_num; i++)
	{
		//strtok�ָ��
		dns_trans[i].IP = strtok(temp[i], " ");
		dns_trans[i].domain = strtok(NULL, " ");
	}

	//�ر��ļ�
	fclose(fp);
	printf("read dnsrelay.txt success.\n");

	return table_num;
}

void get_domain(int recv_data, char* recvbuf)
{
	char temp_domain_find[64];

	memset(domain_find, 0, 64);
	//memcpy:�Բ���2ָ��ĵ�ַΪ��㣬�������Ĳ���3���ֽ����ݣ����Ƶ��Բ���1ָ��ĵ�ַΪ�����ڴ���
	
	memcpy(temp_domain_find, &(recvbuf[sizeof(DNS_HEAD)]), recv_data - 12);//��ѯ���ⳤ��Ϊ�����յ��ĳ���-12

	int question_len = strlen(temp_domain_find);

	//�����Ĺ��� 3www5baidu3com ����+�ó��ȵ��ַ�+����+�ó��ȵ��ַ�
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


int find_domain(int table_num)//�ڱ����б��в��Ҹ��������ҵ��ͷ�������(0������-1����û�ҵ��ͷ���-1
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

int idnum = 0;//�Ѿ�ת����ID��-1

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

	//���ʱ��
	printf("time: %u/%u/%u %u:%u:%u:%u ",
		currentTime.wYear, currentTime.wMonth, currentTime.wDay,
		currentTime.wHour, currentTime.wMinute, currentTime.wSecond,
		currentTime.wMilliseconds);
	printf("	");

	if (result == -1)
	{
		printf("�м�	");
		printf("%s\n", domain_find);

	}
	else
	{
		if (strcmp(dns_trans[result].IP, "0.0.0.0") == 0)
		{
			printf("����	");
			printf("%s	", domain_find);
			printf("0.0.0.0 \n");
		}
		else
		{
			printf("������	");
			printf("%s	", domain_find);
			printf("%s	\n", dns_trans[result].IP);
		}
	}


	return;
}