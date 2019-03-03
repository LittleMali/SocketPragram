#include "stdio.h"
#include "string.h"
#include "../NatDataDef.h"
#include "NatDetectorClient.h"

int main(int argc, const char * argv[])
{
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == sock)
		return 0;

	sockaddr_in srvAddr;
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(SERVER_PORT);
	srvAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	
	char szSend[1024] = "hello world";
	sendto(sock, szSend, strlen(szSend), 0, (sockaddr*)&srvAddr, sizeof(sockaddr_in));

	char szRecv[1024] = {0};
	sockaddr_in peerAddr;
	socklen_t nPeerAddrLen = sizeof(sockaddr_in);
	memset(&peerAddr, 0, sizeof(peerAddr));
	recvfrom(sock, szRecv, sizeof(szRecv), 0, (sockaddr*)&peerAddr, &nPeerAddrLen);
	
	printf("%s\n", szRecv);	
	
	return 0;
}
