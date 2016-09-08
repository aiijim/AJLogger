//
//  AJUtil.cpp
//  AJLogger
//
//  Created by aiijim on 16/8/4.
//  Copyright © 2016年 aiijim. All rights reserved.
//

#include "AJLoggerPlus.h"
#include "unistd.h"
#include <sstream>
#include <cassert>

namespace AJLoggerPlus
{
    static const int BufferSize = 64;
    static const char* fmt = "[tid:%d][%s {%s}][%s:%d {%s}]%s";
    
    std::string AJUtil::getCurrentTimeString()
    {
        time_t currentTime = time(NULL);
        struct tm * timeinfo = localtime(&currentTime);
        std::ostringstream sstr;
        sstr<<timeinfo->tm_year+1900<<timeinfo->tm_mon+1<<timeinfo->tm_mday;
        return sstr.str();
    }
    
    std::string AJUtil::getCurrentTimeString(long time)
    {
        struct tm * timeinfo = localtime(&time);
        std::string currentStr;
        char buffer [BufferSize] = {0};
        strftime(buffer,BufferSize,"%F %T",timeinfo);
        return std::string(buffer);
    }
    
    const char* AJUtil::getFileName(const char* fileAbusoluteName)
    {
        assert(NULL != fileAbusoluteName);
        const char * pFileName = strrchr(fileAbusoluteName, '/');
        if (pFileName == NULL)
        {
            pFileName = fileAbusoluteName;
        }
        else
        {
            pFileName++;
        }
        return pFileName;
    }
    
    std::string AJUtil::getLogPrefix(LogType type)
    {
        std::string prefix[] = {" < Error > "," < Warning > "," < Info > "," < Debug > "," < Verbose > "};
        return prefix[type];
    }
    
    std::string AJUtil::makeLogText(AJLogMessage* const pMsg)
    {
        assert(NULL != pMsg);
        std::ostringstream sstr;
//        sstr<<"["<<getCurrentTimeString(pMsg->m_time)<<" (tid:"<<pMsg->m_tid<<"){"<<pMsg->m_moduleName<<"}]";
//        sstr<<"["<<pMsg->m_srcFileName<<"("<<pMsg->m_functionName<<"):"<<pMsg->m_lineNum<<"]"<<getLogPrefix(pMsg->m_type);
        int size = snprintf(NULL, 0, fmt, pMsg->m_tid,getCurrentTimeString(pMsg->m_time).c_str(),pMsg->m_moduleName.c_str(),
                            pMsg->m_srcFileName.c_str(),pMsg->m_lineNum,pMsg->m_functionName.c_str(),getLogPrefix(pMsg->m_type).c_str()) + 1;
        char* buffer = (char*)malloc(size);
        memset(buffer, 0x00, size);
        snprintf(buffer, size, fmt, pMsg->m_tid,getCurrentTimeString(pMsg->m_time).c_str(),pMsg->m_moduleName.c_str(),
                 pMsg->m_srcFileName.c_str(),pMsg->m_lineNum,pMsg->m_functionName.c_str(),getLogPrefix(pMsg->m_type).c_str());
        sstr<<buffer;
        free(buffer);
        if(pMsg->m_isBinaryData)
        {
            static char index[] = "0123456789ABCDEF";
            for (std::vector<BYTE>::iterator iter = pMsg->m_binaryInfo.begin(); iter != pMsg->m_binaryInfo.end(); iter++)
            {
                BYTE value = *iter;
                int high = (value & 0xF0) >> 4;
                int low = (value & 0x0F);
                sstr<<index[high]<<index[low]<<" ";
            }
        }
        else
        {
            sstr<<pMsg->m_info;
        }
        
        sstr<<std::endl;
        return sstr.str();
    }
    
    AJLogMessage* AJUtil::makeLogMessage(LogType type,int line,int tid,std::string srcFileName,std::string functionName,std::string moduleName,long time,std::string info)
    {
        AJLogMessage* pMsg = new AJLogMessage();
        pMsg->m_type = type;
        pMsg->m_tid = tid;
        pMsg->m_lineNum = line;
        pMsg->m_srcFileName = srcFileName;
        pMsg->m_functionName = functionName;
        pMsg->m_moduleName = moduleName;
        pMsg->m_time = time;
        pMsg->m_isBinaryData = false;
        pMsg->m_info = info;
        
        return pMsg;
    }
    
    AJLogMessage* AJUtil::makeLogMessage(LogType type,int line,int tid,std::string srcFileName,std::string functionName,std::string moduleName,long time,std::vector<BYTE> info)
    {
        AJLogMessage* pMsg = new AJLogMessage();;
        pMsg->m_type = type;
        pMsg->m_tid = tid;
        pMsg->m_lineNum = line;
        pMsg->m_srcFileName = srcFileName;
        pMsg->m_functionName = functionName;
        pMsg->m_moduleName = moduleName;
        pMsg->m_time = time;
        pMsg->m_isBinaryData = true;
        pMsg->m_binaryInfo = info;
        
        return pMsg;
    }
}
