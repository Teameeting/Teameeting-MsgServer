//
//  GRTChannelManager.h
//  MsgServerGrouper
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerGrouper__GRTChannelManager__
#define __MsgServerGrouper__GRTChannelManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include "RTSingleton.h"

#include "GRTSubChannel.h"


class GRTChannelManager : public RTSingleton< GRTChannelManager >{
    friend class RTSingleton< GRTChannelManager >;
public:

    bool InitManager()
    {
        return true;
    }

    bool UninManager()
    {
        CleanupChannel();
        return true;
    }

    GRTSubChannel* GenSubChannel(const std::string& ip, int port, const std::string& channel, std::string& chKey)
    {
        GRTSubChannel* ch = new GRTSubChannel(ip, port, channel);
        if (!ch)
            return nullptr;
        if (!ch->Init())
        {
            delete ch;
            return nullptr;
        }
        char ck[256] = {0};
        sprintf(ck, "sub:ip:%s:port:%d:chl:%s", ip.c_str(), port, channel.c_str());
        chKey.assign(ck);
        return ch;
    }

    void AddChannel(const std::string& chKey, GRTSubChannel* pCh)
    {
        OSMutexLocker locker(&m_mutexChannel);
        m_SubChannelMap.insert(make_pair(chKey, pCh));
    }

    void DelChannel(const std::string& chKey)
    {
        OSMutexLocker locker(&m_mutexChannel);
        SubChannelMapIt it = m_SubChannelMap.find(chKey);
        if (it != m_SubChannelMap.end())
        {
            GRTSubChannel *pCh = (*it).second;
            if (pCh)
            {
                pCh->Stop();
                pCh->Unit();
                delete pCh;
                pCh = nullptr;
            }
        }
        m_SubChannelMap.erase(chKey);
    }

    void CleanupChannel()
    {
        LI("GRTChannelManager::CleanupChannel was called\n");
        OSMutexLocker locker(&m_mutexChannel);
        for(SubChannelMapIt it=m_SubChannelMap.begin();it != m_SubChannelMap.end();++it)
        {
            GRTSubChannel *pCh = (*it).second;
            if (pCh)
            {
                pCh->Stop();
                pCh->Unit();
                delete pCh;
                pCh = nullptr;
            }
            m_SubChannelMap.erase((*it).first);
        }
    }


protected:
    GRTChannelManager()
    {

    }

    ~GRTChannelManager()
    {

    }
private:
    // <channelKey, GRTSubChannel*>
    // channekKey--->"sub:ip:%s:port:%d:chl:%s"
    typedef std::unordered_map<std::string, GRTSubChannel*>     SubChannelMap;
    typedef SubChannelMap::iterator                             SubChannelMapIt;
private:
    SubChannelMap                           m_SubChannelMap;
    OSMutex                                 m_mutexChannel;
};

#endif /* defined(__MsgServerGrouper__GRTChannelManager__) */
