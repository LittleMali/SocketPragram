#include "stdio.h"
#include "NatUtils.h"

void LogNatData(const NatData &data)
{
	printf("type=%d, buf=%s.\n", data.iType, data.szBuf);
}

void LogIpPort(const sockaddr_in &addr)
{
	printf("(%s %d)\n", inet_ntoa(addr.sin_addr), ntohl(addr.sin_port));
}

int ParseNatData(const NatData &data)
{
	int ret = 0;

	return 1;
}
