#include "stdio.h"
#include "string.h"
#include "../NatDataDef.h"
#include "NatDetectorClient.h"

void TestUdp();
void TestTcp();

int main(int argc, const char * argv[])
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

	printf("create socket: %d.\n", sock);

	sockaddr_in srvAddr;
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(SERVER_PORT);
	srvAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	
	
	ssize_t ret = 0;
	
	for(int i = 0; i < 100; i++)
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

		sleep(1);
	}			
		
	printf("end.\n");
}

void TestTcp()
{
	printf("beign.\n");

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == sock)
		return;

	sockaddr_in srvIp;
	srvIp.sin_family = AF_INET;
	srvIp.sin_addr.s_addr = inet_addr(SERVER_IP);
	srvIp.sin_port = htons(SERVER_PORT);

	int ret = -1;
	ret = connect(sock, (sockaddr *)&srvIp, sizeof(srvIp));
	int lastErr = errno;
	printf("connect ret=%d, lastErr=%d.\n", ret, lastErr);

	if (-1 == ret)
		return;

	for (int i  = 0; i < 10; ++i)
	{
		char szSendBuf[1024] = {0};
		sprintf(szSendBuf, "send cnt: %d", i);

		ret = send(sock, szSendBuf, strlen(szSendBuf), 0);
		printf("send ret=%d, %s\n", ret, szSendBuf);

		char szRcvBuf[1024] = {0};
		ret = recv(sock, szRcvBuf, sizeof(szRcvBuf) - 1, 0);
		printf("recv ret=%d, %s\n", ret, szRcvBuf);

		if (ret == -1)
		{
			// error
		}
		else if (ret == 0)
		{
			// peer is closed
		}
	}

	printf("end.\n");
}