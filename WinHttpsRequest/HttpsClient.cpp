#include "stdafx.h"
#include "HttpsClient.h"

#pragma comment(lib, "winhttp")

CHttpsClient::CHttpsClient()
: m_iWinErrCode(0)
, m_iHttpsErrCode(HttpsErrCode_Invalid)
, m_iStatusCode(HTTPS_INVALID_STATUS)
, m_pbyteBodyReturned(NULL)
{

}

CHttpsClient::~CHttpsClient()
{
    Clear();
}

BOOL CHttpsClient::Navigate(LPCWSTR lpUrl, LPCWSTR lpRefer /* = NULL */, int *piTimeOut /* = NULL */)
{
    BOOL bRet = FALSE;

    Clear();

    try
    {
        bRet = Navigate_Safe(lpUrl, lpRefer, piTimeOut);
    }
    catch (...)
    {
    	
    }

    return bRet;
}


const byte * CHttpsClient::GetBody()
{
    if (m_vecBody.empty())
        return NULL;

    byte *pTmp = NULL;

    if (m_pbyteBodyReturned)
    {
        pTmp = m_pbyteBodyReturned;
    }
    else
    {
        m_pbyteBodyReturned = new (std::nothrow) byte [m_vecBody.size()];
        if (m_pbyteBodyReturned)
        {
            ZeroMemory(m_pbyteBodyReturned, m_vecBody.size());

            for (size_t i = 0; i < m_vecBody.size(); ++i)
            {
                m_pbyteBodyReturned[i] = m_vecBody[i];
            }

            pTmp = m_pbyteBodyReturned;
        }
    }

    return pTmp;
}

BOOL CHttpsClient::Navigate_Safe(LPCWSTR lpUrl, LPCWSTR lpRefer /* = NULL */, int *piTimeOut /* = NULL */)
{
    if (!lpUrl)
    {
        SetStatus(HttpsErrCode_EmptyUrl);
        return FALSE;
    }

    // crack url
    URL_COMPONENTS urlComp = {0};
    ZeroMemory(&urlComp, sizeof(URL_COMPONENTS));
    urlComp.dwStructSize = sizeof(URL_COMPONENTS);

    urlComp.dwSchemeLength    = -1;
    urlComp.dwHostNameLength  = -1;
    urlComp.dwUrlPathLength   = -1;
    urlComp.dwExtraInfoLength = -1;

    if (!::WinHttpCrackUrl(lpUrl, (DWORD)wcslen(lpUrl), 0, &urlComp))
    {
        SetStatus(HttpsErrCode_WrongUrl);
        SetErrCode(::GetLastError());
        return FALSE;
    }

    HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
    BOOL bRet = FALSE, bResults = FALSE;

    do 
    {
        // http open
        hSession = ::WinHttpOpen(L"A ProxyAcc Https/1.0"
            , WINHTTP_ACCESS_TYPE_DEFAULT_PROXY
            , WINHTTP_NO_PROXY_NAME
            , WINHTTP_NO_PROXY_BYPASS
            , 0);
        if (!hSession)
        {
            SetStatus(HttpsErrCode_HttpOpen);
            SetErrCode(::GetLastError());
            break;
        }

        BOOL bSetTime = FALSE;
        if (piTimeOut)
        {
            int iTimeOut = *piTimeOut;
            bSetTime = ::WinHttpSetTimeouts(hSession, iTimeOut, iTimeOut, iTimeOut, iTimeOut);
        }

        // http connect
        std::wstring strHostName = urlComp.lpszHostName;
        strHostName = strHostName.substr(0, urlComp.dwHostNameLength);

        hConnect = ::WinHttpConnect(hSession
            , strHostName.c_str()
            , INTERNET_DEFAULT_HTTPS_PORT       // https
            , 0);
        if (!hConnect)
        {
            SetStatus(HttpsErrCode_HttpConnect);
            SetErrCode(::GetLastError());
            break;
        }

        // open request
        hRequest = ::WinHttpOpenRequest(hConnect
            , L"GET"
            , urlComp.lpszUrlPath
            , NULL
            , lpRefer
            , WINHTTP_DEFAULT_ACCEPT_TYPES
            , WINHTTP_FLAG_SECURE);
        if (!hRequest)
        {
            SetStatus(HttpsErrCode_HttpOpenRequest);
            SetErrCode(::GetLastError());
            break;
        }

        // send request
        bResults = ::WinHttpSendRequest(hRequest
            , WINHTTP_NO_ADDITIONAL_HEADERS, 0
            , WINHTTP_NO_REQUEST_DATA, 0
            , 0
            , 0);
        if (!bResults)
        {
            SetStatus(HttpsErrCode_HttpSendRequest);
            SetErrCode(::GetLastError());
            break;
        }

        bResults = ::WinHttpReceiveResponse(hRequest, NULL);
        if (!bResults)
        {
            SetStatus(HttpsErrCode_HttpReceiveResponse);
            SetErrCode(::GetLastError());
            break;
        }
        
        // recv header status code
        if (!RecvResponse_Header(hRequest))
        {
            break;
        }

        // recv body
        if (!RecvResponse_Body(hRequest))
        {
            break;
        }

        bRet = TRUE;        // note
        m_iHttpsErrCode = HttpsErrCode_Success;

    } while (FALSE);
    
    // Close open handles.
    if (hRequest) 
    {
        ::WinHttpCloseHandle(hRequest);
        hRequest = NULL;
    }

    if (hConnect) 
    {     
        ::WinHttpCloseHandle(hConnect);
        hConnect = NULL;
    }

    if (hSession) 
    {     
        ::WinHttpCloseHandle(hSession);
        hSession = NULL;
    }

    return bRet;
}

BOOL CHttpsClient::RecvResponse_Header(HINTERNET hRequest)
{
    if (!hRequest)
        return FALSE;

    DWORD dwSize = 0;
    LPWSTR pszOutBufferW = NULL;
    BOOL bResults = FALSE;

    // WINHTTP_QUERY_RAW_HEADERS_CRLF - 可以获取到整个头部
    // 目前这里只获取status code
    ::WinHttpQueryHeaders(hRequest
        , WINHTTP_QUERY_STATUS_CODE
        , WINHTTP_HEADER_NAME_BY_INDEX
        , NULL, &dwSize
        , WINHTTP_NO_ADDITIONAL_HEADERS);

    if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
        SetStatus(HttpsErrCode_HttpQueryHeaders);
        SetErrCode(::GetLastError());
        return FALSE;
    }

    pszOutBufferW = new (std::nothrow) WCHAR [dwSize / sizeof(WCHAR) + 1];
    if (!pszOutBufferW)
    {
        SetStatus(HttpsErrCode_HttpNewBufferFailed);
        return FALSE;
    }

    ZeroMemory(pszOutBufferW, dwSize / sizeof(WCHAR) + 1);

    bResults = ::WinHttpQueryHeaders(hRequest
        , WINHTTP_QUERY_STATUS_CODE
        , WINHTTP_HEADER_NAME_BY_INDEX
        , (LPVOID)pszOutBufferW, &dwSize
        , WINHTTP_NO_ADDITIONAL_HEADERS);

    if (!bResults)
    {
        SetStatus(HttpsErrCode_HttpQueryHeaders);
        SetErrCode(::GetLastError());

        delete [] pszOutBufferW;
        pszOutBufferW = NULL;

        return FALSE;
    }
    
    m_iStatusCode = _wtoi(pszOutBufferW);

    delete [] pszOutBufferW;
    pszOutBufferW = NULL;

    return TRUE;
}

BOOL CHttpsClient::RecvResponse_Body(HINTERNET hRequest)
{
    if (!hRequest)
        return FALSE;

    DWORD dwSize = 0, dwBytesReaded = 0;
    LPSTR pszOutBufferA = NULL;
    BOOL bResults = FALSE;

    do 
    {
        dwSize = 0;
        dwBytesReaded = 0;

        bResults = ::WinHttpQueryDataAvailable(hRequest, &dwSize);
        if (!bResults)
        {
            SetStatus(HttpsErrCode_HttpQueryDataAvailable);
            SetErrCode(::GetLastError());
            break;
        }

        pszOutBufferA = new (std::nothrow) char[dwSize + 1];
        if (!pszOutBufferA)
        {
            bResults = FALSE;
            SetStatus(HttpsErrCode_HttpNewBufferFailed);
            break;
        }

        ZeroMemory(pszOutBufferA, dwSize + 1);

        bResults = ::WinHttpReadData(hRequest
            , (LPVOID)pszOutBufferA, dwSize
            , &dwBytesReaded);
        if (!bResults)
        {
            SetStatus(HttpsErrCode_HttpReadBody);
            SetErrCode(::GetLastError());

            delete [] pszOutBufferA;
            pszOutBufferA = NULL;

            break;
        }

        for (DWORD i = 0; i < dwBytesReaded; ++i)
        {
            m_vecBody.push_back(pszOutBufferA[i]);
        }

        delete [] pszOutBufferA;
        pszOutBufferA = NULL;

    } while (dwSize > 0);

    if (!bResults)
    {
        // read body failed
        m_vecBody.clear();
    }

    return bResults;
}

void CHttpsClient::Clear()
{
    m_vecBody.clear();
    m_iStatusCode = HTTPS_INVALID_STATUS;
    if (m_pbyteBodyReturned)
    {
        delete [] m_pbyteBodyReturned;
        m_pbyteBodyReturned = NULL;
    }

    m_iHttpsErrCode = HttpsErrCode_Invalid;
    m_iWinErrCode = 0;
}
