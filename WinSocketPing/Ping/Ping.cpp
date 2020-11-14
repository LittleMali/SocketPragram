// Ping.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include  "SimplePing.h"

const std::string g_strIps[] =
{
    "169.56.149.102",
    "119.28.114.189",
    "49.51.36.177",
    "49.51.137.205",
    "161.202.93.141",
    "49.51.134.205",
    "119.28.178.108",
    "119.28.193.98",
    "119.28.17.59",
    "119.28.50.80",
    "119.28.204.131",
    "119.28.178.108",
    "119.28.193.98",
    "119.28.193.98",
    "119.28.143.37",
    "119.28.140.230",
    "119.28.178.108",
    "119.28.193.98",
    "119.28.143.234",
    "119.28.41.191",
    "119.28.55.74",
    "119.28.178.108",
    "119.28.193.98",
    "119.28.182.250",
    "119.28.84.182",
    "119.28.163.18",
    "119.28.150.117",
    "119.28.163.18",
    "119.28.158.98",
    "119.28.150.94",
    "119.28.163.18",
    "119.28.154.110",
    "119.28.150.146",
    "119.28.149.101",
    "119.28.149.51",
    "119.28.163.18",
    "119.28.160.166",
    "119.28.114.189",
    "119.28.111.166",
    "119.28.106.80",
    "49.51.36.148",
    "49.51.36.177",
    "169.56.86.56",
};

int _tmain(int argc, _TCHAR* argv[])
{
    VecPingTime vecPing;
    for (int i = 0; i < sizeof(g_strIps) / sizeof(g_strIps[0]); ++i)
    {
        PingTime tmp;
        IN_ADDR ipAddr;
        if (InetPtonA(AF_INET, g_strIps[i].c_str(), (PVOID)&ipAddr))
        {
            tmp.ip_dst_n = ipAddr.S_un.S_addr;
            memcpy(tmp.ip_dst_p, g_strIps[i].c_str(), g_strIps[i].size());

            vecPing.push_back(tmp);
        }
    }

    printf("total ip cnt=%d.\n", vecPing.size());
    
    CSimplePing oSimplePing;
    oSimplePing.Init();

    oSimplePing.SetIp(vecPing);
    oSimplePing.PingEx();

    oSimplePing.UnInit();

    getchar();

	return 0;
}

