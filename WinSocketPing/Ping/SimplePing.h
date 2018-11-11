#ifndef _SIMPLE_PING_H_
#define _SIMPLE_PING_H_

#include "PingDefine.h"

class CSimplePing
{
public:
    CSimplePing();
    ~CSimplePing();

    BOOL Init();
    void UnInit();

    void SetIp(const MapPingTime& mapIp) { m_mapPingTime = mapIp; }
    void SetIp(const VecPingTime& vecIp) { m_vecPingTime = vecIp; }
    BOOL PingEx();

    static unsigned int WINAPI RecvThread(LPVOID pParam);

private:
    SOCKET CreateSocket(const DWORD dwTimeOut = 1000);
    void CloseSocket(SOCKET socPing);

    void SendAll();
    void WaitRecvDone();
    void RecvThreadImpl();
    void PrintPingResult();

    USHORT CalcChecksum(LPBYTE lpBuf, DWORD dwSize);
    void FillIcmpData(char* pIcmpData, int nDataSize, int nSeq = 0);
    DWORD DecodeIcmpResponse(char* buf, int bytes);

private:
    volatile DWORD m_dwRecvCnt;
    SOCKET      m_socPing;
    MapPingTime m_mapPingTime;
    VecPingTime m_vecPingTime;

};

#endif