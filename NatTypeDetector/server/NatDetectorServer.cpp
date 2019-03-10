#include "stdio.h"
#include "./NatDetectorServer.h"
#include "../NatUtils.h"
#include "string.h"

void TestUdp();
void TestTcp();

int main(int argc, const char *argv[])
{
	TestTcp();	
	return 0;
}

void TestUdp()
{
	printf("begin.\n");
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == sock)
		return;

	printf("socket=%d.\n", sock);

	sockaddr_in srvAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(SERVER_PORT);
	srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(sock, (sockaddr*)&srvAddr, sizeof(srvAddr));
	if (-1 == ret)
		return;
	printf("bind suc.\n");

	char recvBuf[1024] = {0};
	sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(clientAddr));
	socklen_t clientAddrLen = sizeof(clientAddr);

	while (true)
	{
		ret = recvfrom(sock, recvBuf, sizeof(recvBuf), 0, (sockaddr *)&clientAddr, &clientAddrLen);
		printf("recv : %s\n", recvBuf);
		char *pClientIp = inet_ntoa(clientAddr.sin_addr);
		printf("client ip: %s, port: %d\n", pClientIp, ntohs(clientAddr.sin_port));

		char sendBuf[1024] = {0};
		sprintf(sendBuf, "ip: %s, port: %d", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		ret = sendto(sock, sendBuf, strlen(sendBuf), 0, (sockaddr *)&clientAddr, sizeof(clientAddr));
		printf("send ret: %d\n", ret);
	}
	
	printf("end.\n");
}

void TestTcp()
{
	printf("tcp begin.\n");

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == sock)
		return;

	sockaddr_in srvIp;
	srvIp.sin_family = AF_INET;
	srvIp.sin_addr.s_addr = inet_addr(INADDR_ANY);
	srvIp.sin_port = htons(SERVER_PORT);

	int ret = -1;
	int lastErr = 0;

	ret = bind(sock, (sockaddr *)&srvIp, sizeof(srvIp));
	lastErr = errno;
	printf("bind ret=%d, lastErr=%d.\n", ret, lastErr);
	if (-1 == ret)
		return;

	ret = listen(sock, MAX_TCP_CONNECT_CNT);
	lastErr = errno;
	printf("listen ret=%d, lastErr=%d.\n", ret, lastErr);
	if (-1 == ret)
		return;
	
	int peerSock[MAX_TCP_CONNECT_CNT] = {0};
	int i = 0;
	while(true)
	{
		sockaddr_in peerAddr;
		socklen_t peerLen = sizeof(sockaddr_in);
		memset(&peerAddr, 0, sizeof(peerAddr));

		peerSock[i++] = accept(sock, (sockaddr *)&peerAddr, &peerLen);
		lastErr = errno;
		printf("accept peerSock=%d, lastErr=%d, peerIp=%s, peerPort=%d\n"
			, peerSock[i], lastErr, inet_ntoa(peerAddr.sin_addr), ntohs(peerAddr.sin_port));

		for (int j = 0; j < i && peerSock[j] != 0; ++j)
		{
			char szRcvBuf[1024] = {0};
			ret = recv(peerSock[j], szRcvBuf, sizeof(szRcvBuf), 0);
			printf("sock(%d) recv, ret=%d, szRcv=%s.\n", peerSock[j], ret, szRcvBuf);
			if (ret == 0)
			{
				// peer is closed;
			}
			else if (ret == -1)
			{
				// err occored.
				close(peerSock[j]);
				peerSock[j] = 0;
			}

			char szSendBuf[1024] = {0};
			sprintf(szSendBuf, "%s--srv append.", szRcvBuf);
			ret = send(peerSock[j], szSendBuf, strlen(szSendBuf), 0);
			printf("sock(%d) send, ret=%d, szSend=%s.\n", peerSock[j], ret, szSendBuf);
		}
	}

	printf("tcp end.\n");
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
