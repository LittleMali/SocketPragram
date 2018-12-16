#ifndef _NAT_DATA_DEF_H_
#define _NAT_DATA_DEF_H_ 

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define SERVER_IP	"193.112.22.43"
#define SERVER_PORT	9999

#define NAT_DATA_BUF_LEN	1024

enum NatDataType
{
	NatData_Request_Self = 0x0,
	NatData_Response_Self,
	NatData_Request_User, 
	NatData_Response_User,
};

typedef struct __tagNatData
{
	int iType;
	char szBuf[NAT_DATA_BUF_LEN];

	//__tagNatData()
	//{
	//	memset(this, 0, sizeof(__tagNatData));
	//}
} NatData, *PNatData;

#endif
