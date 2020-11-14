#include "stdafx.h"
#include "SimplePing.h"
#include <process.h>

CSimplePing::CSimplePing()
    : m_dwRecvCnt(0)
{

}

CSimplePing::~CSimplePing()
{

}

BOOL CSimplePing::Init()
{
    int nRet = 0;
    WSADATA data = { 0 };
    SYSTEM_INFO si = { 0 };

    nRet = ::WSAStartup(MAKEWORD(2, 1), &data);
    if (0 != nRet)
    {
        return FALSE;
    }

    m_dwRecvCnt = 0;

    m_socPing = CreateSocket();
    if (INVALID_SOCKET == m_socPing)
    {
        return FALSE;
    }

    return TRUE;
}

void CSimplePing::UnInit()
{        
    CloseSocket(m_socPing);
    ::WSACleanup();
}

SOCKET CSimplePing::CreateSocket(const DWORD dwTimeOut)
{
    SOCKET socPing = INVALID_SOCKET;
    BOOL bRet = FALSE;

    do
    {
        socPing = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        if (INVALID_SOCKET == socPing)
            break;

        int nRet = setsockopt(socPing, SOL_SOCKET, SO_SNDTIMEO, (char*)&dwTimeOut, sizeof(dwTimeOut));
        if (SOCKET_ERROR == nRet)
            break;

        nRet = setsockopt(socPing, SOL_SOCKET, SO_RCVTIMEO, (char*)&dwTimeOut, sizeof(dwTimeOut));
        if (SOCKET_ERROR == nRet)
            break;

        bRet = TRUE;

    } while (FALSE);

    if (!bRet && INVALID_SOCKET != socPing)
    {
        closesocket(socPing);
        socPing = INVALID_SOCKET;
    }

    return socPing;
}

void CSimplePing::CloseSocket(SOCKET socPing)
{
    if (socPing != INVALID_SOCKET)
    {
        closesocket(socPing);
        socPing = INVALID_SOCKET;
    }
}

void CSimplePing::FillIcmpData(char* pIcmpData, int nDataSize, int nSeq)
{
    if (!pIcmpData || nDataSize < ICMP_MIN_HEADER_SIZE)
        return;

    PIcmpHeader pHdr = (PIcmpHeader)pIcmpData;
    pHdr->icmp_type = ICMP4_ECHO_REQUEST;
    pHdr->icmp_code = 0;
    pHdr->icmp_id = (USHORT)::GetCurrentProcessId();
    pHdr->icmp_seq = nSeq;

    char* pData = (char*)pIcmpData + ICMP_HEADER_SIZE;
    memset(pData, 'A', nDataSize - ICMP_HEADER_SIZE);

    pHdr->icmp_chksum = CalcChecksum((LPBYTE)pIcmpData, nDataSize);
}

DWORD CSimplePing::DecodeIcmpResponse(char* buf, int bytes)
{
    DWORD nSeq = -1;
    if (!buf)
        return nSeq;

    PIpHeader pIpHdr = (PIpHeader)buf;
    int nIpHdrSize = pIpHdr->h_len * 4;
    if (bytes < nIpHdrSize + ICMP_MIN_HEADER_SIZE)
        return nSeq;

    PIcmpHeader pIcmpHdr = (PIcmpHeader)(buf + nIpHdrSize);
    if (pIcmpHdr->icmp_type != ICMP4_ECHO_REPLY)
        return nSeq;

    if (pIcmpHdr->icmp_id != (USHORT)::GetCurrentProcessId())
        return nSeq;

    char szSrcIp[IP_SIZE] = { 0 }, szDstIp[IP_SIZE] = { 0 };
    InetNtopA(AF_INET, (LPVOID)(&pIpHdr->source_ip), szSrcIp, IP_SIZE);
    InetNtopA(AF_INET, (LPVOID)(&pIpHdr->dst_ip), szDstIp, IP_SIZE);

    //printf("recv ip header, srcIp=%s, dstIp=%s.\n", szSrcIp, szDstIp)

    return pIcmpHdr->icmp_seq;
}

USHORT CSimplePing::CalcChecksum(LPBYTE lpBuf, DWORD dwSize)
{
    DWORD       dwCheckSum = 0;
    USHORT      *lpWord = (USHORT *)lpBuf;

    while (dwSize > 1)
    {
        dwCheckSum += *lpWord++;
        dwSize -= 2;
    }
    if (dwSize)
    {
        dwCheckSum += *(UCHAR *)lpWord;
    }
    dwCheckSum = (dwCheckSum >> 16) + (dwCheckSum & 0xffff);
    dwCheckSum += (dwCheckSum >> 16);
    return (USHORT)(~dwCheckSum);
}

BOOL CSimplePing::PingEx()
{
    DWORD dwCost = ::GetTickCount();

    // send
    SendAll();

    // recv
    WaitRecvDone();

    // print
    PrintPingResult();

    printf("total cost time=%d.\n", ::GetTickCount() - dwCost);

    return TRUE;
}

void CSimplePing::SendAll()
{
    for (int i = 0; i < (int)m_vecPingTime.size(); ++i)
    {
        SOCKADDR_IN dstAddr;
        dstAddr.sin_family = AF_INET;
        dstAddr.sin_port = htons(0);
        dstAddr.sin_addr.s_addr = m_vecPingTime[i].ip_dst_n;

        char packet[ICMP_PACKET_SIZE] = { 0 };
        FillIcmpData(packet, ICMP_PACKET_SIZE, i);

        DWORD dwSendBegin = ::GetTickCount();
        int nRet = sendto(m_socPing, packet, ICMP_PACKET_SIZE, 0, (const sockaddr*)&dstAddr, sizeof(dstAddr));
        if (nRet == SOCKET_ERROR)
        {
            printf("send failed, lasterr=%d, seq=%d, sendtime=%d.\n", WSAGetLastError(), i, ::GetTickCount() - dwSendBegin);
        }

        m_vecPingTime[i].seq = i;
        m_vecPingTime[i].send_time = ::GetTickCount();
    }

}

void CSimplePing::WaitRecvDone()
{
    int nThreadCnt = 3;

    HANDLE* pThread = new HANDLE[nThreadCnt];
    if (!pThread)
        return;

    do
    {
        int nIndex = 0;
        for (int i = 0; i < nThreadCnt; ++i)
        {
            HANDLE hTmp = (HANDLE)_beginthreadex(NULL, 0, RecvThread, (void*)this, 0, NULL);
            if (hTmp)
                pThread[nIndex++] = hTmp;
        }

        if (nIndex == 0)
            break;

        WaitForMultipleObjects(nIndex, pThread, TRUE, INFINITE);

    } while (FALSE);
}

unsigned int CSimplePing::RecvThread(LPVOID pParam)
{
    CSimplePing* pThis = (CSimplePing*)pParam;
    if (pThis)
    {
        pThis->RecvThreadImpl();
    }

    return 0;
}

void CSimplePing::RecvThreadImpl()
{
    while (TRUE)
    {
        char szRecvBuf[ICMP_MAX_PACKET_SIZE] = { 0 };

        DWORD dwRcvBegin = ::GetTickCount();
        int nRet = recv(m_socPing, szRecvBuf, ICMP_MAX_PACKET_SIZE, 0);


        InterlockedIncrement(&m_dwRecvCnt);

        if (nRet == SOCKET_ERROR)
        {
            printf("recv error, last err=%d, tid=%d, cnt=%d.\n", WSAGetLastError(), (DWORD)::GetCurrentThreadId(), m_dwRecvCnt);
        }
        else
        {
            DWORD nSeq = DecodeIcmpResponse(szRecvBuf, nRet);
            if (nSeq != -1 && nSeq < m_vecPingTime.size())
            {
                m_vecPingTime[nSeq].recv_time = ::GetTickCount();
                
                printf("recv seq=%d, time=%d, tid=%d.\n", nSeq, m_vecPingTime[nSeq].recv_time - m_vecPingTime[nSeq].send_time, (DWORD)::GetCurrentThreadId());
            }
        }

        if (m_dwRecvCnt >= m_vecPingTime.size())
        {
            printf("thread quit.\n");
            break;
        }
    }
}

void CSimplePing::PrintPingResult()
{
    for (int i = 0; i < (int)m_vecPingTime.size(); ++i)
    {
        if (m_vecPingTime[i].recv_time != 0)
        {
            DWORD dwCostTime = m_vecPingTime[i].recv_time - m_vecPingTime[i].send_time;
            m_vecPingTime[i].time_span = dwCostTime;
            printf("seq=%d, ip=%s, time=%d\n", m_vecPingTime[i].seq, m_vecPingTime[i].ip_dst_p, dwCostTime);
        }
    }
}