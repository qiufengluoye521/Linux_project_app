#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define	UDP_TEST_PORT		50001

int main(int argC, char* arg[])
{
	struct sockaddr_in addr;
	int sockfd, len = 0;	
	int addr_len = sizeof(struct sockaddr_in);
	char buffer[256];	

	/* ����socket��ע�������SOCK_DGRAM */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror ("socket");
		exit(1);
	}

	/* ��дsockaddr_in �ṹ */
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(UDP_TEST_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY) ;// ��������IP����������

	/* ��socket */
	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr))<0) {
		perror("connect");
		exit(1);
	}

	while(1) {
		bzero(buffer, sizeof(buffer));
		len = recvfrom(sockfd, buffer, sizeof(buffer), 0, 
					   (struct sockaddr *)&addr ,&addr_len);
		/* ��ʾclient�˵������ַ���յ����ַ�����Ϣ */
		printf("Received a string from client %s, string is: %s\n", 
				inet_ntoa(addr.sin_addr), buffer);
		/* ���յ����ַ�����Ϣ���ظ�client�� */
		sendto(sockfd,buffer, len, 0, (struct sockaddr *)&addr, addr_len);
	}

	return 0;
}

// ----------------------------------------------------------------------------
// End of udp_server.c