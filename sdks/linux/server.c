/*************************************
�ļ����� server.c
linux ��socket�����̼���  - ����˳���
�������˿���Ϊ 0x8888   ���˿ں͵�ַ�ɸ���ʵ��������ģ�����ʹ�ò������룩
��������ַ��Ϊ 192.168.1.2
����:mfcai
*/

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main()
{
int sfp,nfp; /* �������������� */
struct sockaddr_in s_add,c_add;
int sin_size;
unsigned short portnum=0x8888; /* �����ʹ�ö˿� */

printf("Hello,welcome to my server !\r\n");
sfp = socket(AF_INET, SOCK_STREAM, 0);
if(-1 == sfp)
{
    printf("socket fail ! \r\n");
    return -1;
}
printf("socket ok !\r\n");

/* ��ʼ���������˿ڵ�ַ��Ϣ */
bzero(&s_add,sizeof(struct sockaddr_in));
s_add.sin_family=AF_INET;
s_add.sin_addr.s_addr=htonl(INADDR_ANY);
s_add.sin_port=htons(portnum);
/* ʹ��bind���а󶨶˿� */
if(-1 == bind(sfp,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
{
    printf("bind fail !\r\n");
    return -1;
}
printf("bind ok !\r\n");
/* ��ʼ������Ӧ�Ķ˿� */
if(-1 == listen(sfp,5))
{
    printf("listen fail !\r\n");
    return -1;
}
printf("listen ok\r\n");

while(1)
{
sin_size = sizeof(struct sockaddr_in);
/* accept�����ʹ�ú���������ʱ����������״̬���ȴ��û��������ӣ���û�пͻ��˽�������ʱ������ֹͣ�ڴ˴� */
nfp = accept(sfp, (struct sockaddr *)(&c_add), &sin_size);
if(-1 == nfp)
{
    printf("accept fail !\r\n");
    return -1;
}
printf("accept ok!\r\nServer start get connect from %#x : %#x\r\n",ntohl(c_add.sin_addr.s_addr),ntohs(c_add.sin_port));

/* ����ʹ��write��ͻ��˷�����Ϣ*/
if(-1 == write(nfp,"hello,welcome to my server \r\n",32))
{
    printf("write fail!\r\n");
    return -1;
}
printf("write ok!\r\n");
close(nfp);

}
close(sfp);
return 0;
}
