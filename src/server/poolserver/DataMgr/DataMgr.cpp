#include "DataMgr.h"
#include "ServerDatabaseEnv.h"
#include "Util.h"
#include "Log.h"

DataMgr* DataMgr::singleton = 0;

void DataMgr::Upload()
{
    sLog.Info(LOG_SERVER, "We have %u shares", Size());
    
    while (Size() > BULK_MIN)
    {
        sLog.Info(LOG_SERVER, "Uploading %u shares to database", Size());
        
        std::string query("INSERT INTO `shares` (`rem_host`, `username`, `our_result`, `upstream_result`, `reason`, `time`, `difficulty`) VALUES ");
        for (int i = 0; i < BULK_COUNT; ++i)
        {
            Share share = Pop();
            
            query += Util::FS("(INET_NTOA(%u), '%s', %u, 0, '%s', FROM_UNIXTIME(%u), %u)", share.host, share.username.c_str(), share.result, share.reason.c_str(), share.time, share.diff);
            
            if (!Size())
                break;
            
            if (i != BULK_COUNT-1)
                query += ',';
        }
        
        sLog.Debug(LOG_SERVER, "Query: %s", query.c_str());
        
        sDatabase.ExecuteAsync(query.c_str());
    }
}