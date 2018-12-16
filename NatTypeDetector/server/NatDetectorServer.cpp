#include "stdio.h"
#include "./NatDetectorServer.h"
#include "../NatUtils.h"
#include "string.h"

int main(int argc, const char *argv[])
{
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == sock)
		return 0;

	sockaddr_in srvAddr;
	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(SERVER_PORT);
	srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(sock, (sockaddr*)&srvAddr, sizeof(srvAddr));
	if (-1 == ret)
		return 0;

	ret = HandleNatMsg(sock);

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
