#include "stdio.h"
#include "string.h"
#include "../NatDataDef.h"
#include "NatDetectorClient.h"

int main(int argc, const char * argv[])
{
	printf("begin.\n");

	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == sock)
		return 0;

	printf("create socket: %d.\n", sock);

	sockaddr_in srvAddr;
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(SERVER_PORT);
	srvAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	
	
	ssize_t ret = 0;
	
	for(int i = 0; i < 10; i++)
	{
		char szSend[1024] = {0};
		sprintf(szSend, "send data cnt=%d", i);
		ret = sendto(sock, szSend, strlen(szSend), 0, (sockaddr*)&srvAddr, sizeof(sockaddr_in));
		printf("sendto, ret=%lu.\n", ret);

		char szRecv[1024] = {0};
		sockaddr_in peerAddr;
		socklen_t nPeerAddrLen = sizeof(sockaddr_in);
		memset(&peerAddr, 0, sizeof(peerAddr));
		ret = recvfrom(sock, szRecv, sizeof(szRecv), 0, (sockaddr*)&peerAddr, &nPeerAddrLen);
		
		printf("recvfrom, ret=%lu, %s\n", ret, szRecv);	
	}			
		
	printf("end.\n");
	return 0;
}
