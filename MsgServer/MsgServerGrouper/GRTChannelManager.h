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
#include "RTType.h"
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
        return true;
    }

    void AddChannel(const std::string& channel, GRTSubChannel* pCh)
    {
        OSMutexLocker locker(&m_mutexChannel);
        m_SubChannelMap.insert(make_pair(channel, pCh));
    }

    bool SubChannel(const std::string& channel)
    {
        OSMutexLocker locker(&m_mutexChannel);
        SubChannelMapIt it = m_SubChannelMap.find(channel);
        if (it == m_SubChannelMap.end()) {
            LE("SubChannel not find channel:%s\n", channel.c_str());
            return false;
        } else {
            it->second->Subscribe();
            return true;
        }
    }

    bool UnsubChannel(const std::string& channel)
    {
        OSMutexLocker locker(&m_mutexChannel);
        SubChannelMapIt it = m_SubChannelMap.find(channel);
        if (it == m_SubChannelMap.end()) {
            LE("UnsubChannel not find channel:%s\n", channel.c_str());
            return false;
        } else {
            it->second->Unsubscribe();
            return true;
        }
    }

    void DelChannel(const std::string& channel)
    {
        OSMutexLocker locker(&m_mutexChannel);
        m_SubChannelMap.erase(channel);
    }


protected:
    GRTChannelManager()
    {

    }

    ~GRTChannelManager()
    {

    }
private:
    typedef std::unordered_map<std::string, GRTSubChannel*>     SubChannelMap;
    typedef SubChannelMap::iterator                             SubChannelMapIt;
private:
    SubChannelMap                           m_SubChannelMap;
    OSMutex                                 m_mutexChannel;
};

#endif /* defined(__MsgServerGrouper__GRTChannelManager__) */
