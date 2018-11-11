#ifndef _PING_DEFINE_H_
#define _PING_DEFINE_H_

#include <vector>
#include <map>

typedef std::vector<ULONG> VecPingIp;

typedef struct __tagIcmpHeader
{
    BYTE    icmp_type;
    BYTE    icmp_code;
    USHORT  icmp_chksum;
    USHORT  icmp_id;
    USHORT  icmp_seq;
} IcmpHeader, *PIcmpHeader;

typedef struct __tagIpHeader
{
    unsigned int h_len : 4;     // ��ͷ����
    unsigned int version : 4;   // ipЭ��汾
    unsigned char tos;          // ��������
    unsigned short total_len;   // ���ܳ���
    unsigned short ident;       // ��Ψһ��ʾ
    unsigned short frag_and_flags;  // ��ʾ
    unsigned char ttl;          // ����ʱ��
    unsigned char proto;        // ����Э��
    unsigned short checksum;    // ipУ���
    in_addr  source_ip;
    in_addr  dst_ip;
} IpHeader, *PIpHeader;

#define ICMP_HEADER_SIZE        (sizeof(IcmpHeader))
#define ICMP_DATA_SIZE          32
#define ICMP_PACKET_SIZE        (ICMP_HEADER_SIZE + ICMP_DATA_SIZE)
#define ICMP_MAX_PACKET_SIZE    1024
#define ICMP_MIN_HEADER_SIZE    8

#define ICMP4_ECHO_REQUEST      8
#define ICMP4_ECHO_REPLY        0

#define IP_MAX_HEADER_SIZE      60

#define IP_SIZE         20
typedef struct __tagPingTime
{
    char        ip_dst_p[IP_SIZE];
    ULONG       ip_dst_n;
    DWORD       seq;            // ���
    DWORD       send_time;
    DWORD       recv_time;
    DWORD       time_span;
    SOCKET      socPing;

    __tagPingTime()
    {
        memset(this, 0, sizeof(__tagPingTime));
    }
}PingTime, *PPingTime;

typedef std::map<ULONG, PingTime> MapPingTime;
typedef std::vector<PingTime> VecPingTime;

#pragma comment(lib, "Ws2_32.lib")

#endif