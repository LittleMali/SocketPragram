#ifndef _HTTPS_CLIENT_H_
#define _HTTPS_CLIENT_H_

#include <string>
#include <vector>
#include "FTbj/QMWifiSafeLib/specstrings_undef.h"
#include "FTbj/QMWifiSafeLib/specstrings_strict.h"
#include "FTbj/QMWifiSafeLib/winhttp.h"

#define HTTPS_INVALID_STATUS    -1

class CHttpsClient
{
public:
    enum HttpsErrCode
    {
        HttpsErrCode_Invalid            = -1,
        HttpsErrCode_Success            = 0,        
        HttpsErrCode_EmptyUrl,
        HttpsErrCode_WrongUrl,
        HttpsErrCode_HttpOpen,
        HttpsErrCode_HttpConnect,
        HttpsErrCode_HttpOpenRequest,
        HttpsErrCode_HttpSendRequest,
        HttpsErrCode_HttpReceiveResponse,
        HttpsErrCode_HttpQueryDataAvailable,
        HttpsErrCode_HttpQueryHeaders,
        HttpsErrCode_HttpReadBody,
        HttpsErrCode_HttpNewBufferFailed,
    };
    
public:
    CHttpsClient();
    ~CHttpsClient();

    // lpUrl: 带http头的完整链接
    // lpRefer: 需要带refer头时使用
    // pdwTimeOut: 不设置时使用系统默认值。设置超时后对建联、发送、接收都有影响，单位ms
    BOOL Navigate(LPCWSTR lpUrl, LPCWSTR lpRefer = NULL, int *piTimeOut = NULL);

    int GetHttpStatus()   { return m_iStatusCode; }
    DWORD GetBodyLength()   { return (DWORD)m_vecBody.size(); }
    const byte *GetBody();

    int GetHttpsErrCode() { return m_iHttpsErrCode; }
    int GetWinErrCode()   { return m_iWinErrCode; }

private:
    BOOL Navigate_Safe(LPCWSTR lpUrl, LPCWSTR lpRefer = NULL, int *piTimeOut = NULL);
    BOOL RecvResponse_Header(HINTERNET hRequest);
    BOOL RecvResponse_Body(HINTERNET hRequest);

    void SetStatus(int iStatus)  { m_iHttpsErrCode = iStatus; }
    void SetErrCode(int iErrCode)  { m_iWinErrCode = iErrCode; }
    void Clear();

private:
    int     m_iWinErrCode;
    int     m_iHttpsErrCode;

    int     m_iStatusCode;         // http回包header的状态码
    std::vector<char> m_vecBody;    // http回包body
    byte    *m_pbyteBodyReturned;

};

#endif