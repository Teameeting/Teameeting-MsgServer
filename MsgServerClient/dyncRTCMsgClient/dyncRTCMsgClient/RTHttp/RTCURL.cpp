//
//  RTCURL.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "RTCURL.hpp"

RTCURL::RTCURL()
{
    pCurl = curl_easy_init();
}

RTCURL::~RTCURL()
{
    if (pCurl) {
        curl_easy_cleanup(pCurl);
    }
}

void RTCURL::RTCurlPostUserInit(const std::string& strPath, const std::string& strData, std::string& strResp)
{
    std::string strCont;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(pCurl, CURLOPT_HEADER, headers);   // 指定headers
    curl_easy_setopt(pCurl, CURLOPT_URL, strPath.c_str());   // 指定url
    curl_easy_setopt(pCurl, CURLOPT_POST, 1);
    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, strData.c_str());    // 指定post内容
    curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 0);
    curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, &RTCURL::UserInitCallback);
    curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)&strCont);
    curlCode = curl_easy_perform(pCurl);
    if(curlCode == CURLcode::CURLE_OK)
    {
        long responseCode = 0;
        long headerSize = 0;
        curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &responseCode);
        curl_easy_getinfo(pCurl, CURLINFO_HEADER_SIZE, &headerSize);
        if (responseCode < 200 || responseCode >= 300 || strData.empty())
        {
            printf("curl_easy_getinfo response code:%ld error!\n", responseCode);
            return;
        }
        //printf("get response headerSize:%ld, str.len:%d\n", headerSize, (int)strCont.length());
        strResp = strCont.substr((headerSize));
    } else {
        strResp = "";
    }
}               

size_t RTCURL::UserInitCallback(void* data, size_t size, size_t nmemb, void* content)
{
    long totalSize = size*nmemb;
    std::string* symbolBuffer = (std::string*)content;
    if(symbolBuffer)
    {
        symbolBuffer->append((char *)data, ((char*)data)+totalSize);
    }
    return totalSize;
}

void RTCURL::RTCurlPostApplyRoom(const std::string& strPath, const std::string& strData, std::string& strResp)
{
    std::string strCont;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(pCurl, CURLOPT_HEADER, headers);   // 指定headers
    curl_easy_setopt(pCurl, CURLOPT_URL, strPath.c_str());   // 指定url
    curl_easy_setopt(pCurl, CURLOPT_POST, 1);
    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, strData.c_str());    // 指定post内容
    curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 0);
    curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, &RTCURL::ApplyRoomCallback);
    curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, (void*)&strCont);
    curlCode = curl_easy_perform(pCurl);
    //printf("curl_easy_perform curlCode:%d\n", curlCode);
    if(curlCode == CURLcode::CURLE_OK)
    {
        long responseCode = 0;
        long headerSize = 0;
        curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &responseCode);
        curl_easy_getinfo(pCurl, CURLINFO_HEADER_SIZE, &headerSize);
        if (responseCode < 200 || responseCode >= 300 || strData.empty())
        {
            printf("curl_easy_getinfo response code:%ld error!\n", responseCode);
            return;
        }
        //printf("get response headerSize:%ld, str.len:%d\n", headerSize, (int)strCont.length());
        strResp = strCont.substr((headerSize));
    } else {
        strResp = "";
    }
}

size_t RTCURL::ApplyRoomCallback(void* data, size_t size, size_t nmemb, void* content)
{
    long totalSize = size*nmemb;
    std::string* symbolBuffer = (std::string*)content;
    if(symbolBuffer)
    {
        symbolBuffer->append((char *)data, ((char*)data)+totalSize);
    }
    return totalSize;
}