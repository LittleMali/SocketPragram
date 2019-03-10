#include "stdio.h"
#include "./NatDetectorServer.h"
#include "../NatUtils.h"
#include "string.h"

int main(int argc, const char *argv[])
{
	printf("begin.\n");
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == sock)
		return 0;

	printf("socket=%d.\n", sock);

	sockaddr_in srvAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(SERVER_PORT);
	srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(sock, (sockaddr*)&srvAddr, sizeof(srvAddr));
	if (-1 == ret)
		return 0;
	printf("bind suc.\n");
	
	char recvBuf[1024] = {0};
	sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(clientAddr));
	socklen_t clientAddrLen = sizeof(clientAddr);

	ret = recvfrom(sock, recvBuf, sizeof(recvBuf), 0, (sockaddr *)&clientAddr, &clientAddrLen);
	printf("recv : %s\n", recvBuf);
	char *pClientIp = inet_ntoa(clientAddr.sin_addr);
	printf("client ip: %s, port: %d\n", pClientIp, ntohs(clientAddr.sin_port));

	char sendBuf[1024] = {0};
	ret = sendto(sock, sendBuf, strlen(sendBuf), 0, (sockaddr *)&clientAddr, sizeof(clientAddr));
	printf("send ret: %d\n", ret);

	printf("end.\n");
	return 0;
}

int HandleNatMsg(int sock)
{
	if (sock == -1)
		return 0;

	sockaddr_in clientAddr;
	socklen_t addrLen = sizeof(clientAddr);

	NatData recvData;
	size_t recvLen = sizeof(recvData);
	size_t ret = recvfrom(sock, (char *)&recvData, recvLen, 0, (sockaddr *)&clientAddr, &addrLen);
	if (-1 == ret)
		return 0;

	LogIpPort(clientAddr);
	LogNatData(recvData);

	return 1;
}
