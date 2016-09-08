//
//  AJLogger.cpp
//  AJLogger
//
//  Created by aiijim on 16/8/4.
//  Copyright © 2016年 aiijim. All rights reserved.
//

#include "AJLoggerWorker.h"
#include <cassert>

namespace AJLoggerPlus
{
    AJLogger* AJLogger::m_instance = NULL;
    pthread_mutex_t AJLogger::m_singleMutex = PTHREAD_MUTEX_INITIALIZER;
    AJLogger* AJLogger::getSharedInstance()
    {
        pthread_mutex_lock(&m_singleMutex);
        if (m_instance == NULL)
        {
            m_instance = new AJLogger();
        }
        pthread_mutex_unlock(&m_singleMutex);
        return m_instance;
    }
    
    AJLogger::AJLogger()
    {
        m_logEnabled =false;
        m_loggerWorker = new AJLoggerWorker();
    }
    
    AJLogger::~AJLogger()
    {
        pthread_mutex_destroy(&m_singleMutex);
        if (m_loggerWorker != NULL)
        {
            delete m_loggerWorker;
            m_loggerWorker = NULL;
        }
    }
    
    void AJLogger::open()
    {
        m_loggerWorker->start();
        m_logEnabled = true;
    }
    
    void AJLogger::close()
    {
        m_loggerWorker->stop();
        m_logEnabled = false;
    }
    
    void AJLogger::destory()
    {
        pthread_mutex_lock(&m_singleMutex);
        if (m_instance != NULL)
        {
            delete m_instance;
            m_instance = NULL;
        }
        pthread_mutex_unlock(&m_singleMutex);
    }
    
    void AJLogger::logStr(LogType type,int line,std::string srcFileName,std::string functionName,std::string moduleName,long time,std::string info)
    {
        if (!m_logEnabled)
        {
            return;
        }
        
        int tid = (int)pthread_mach_thread_np(pthread_self());
        this->logMsg(AJUtil::makeLogMessage(type, line, tid, AJUtil::getFileName(srcFileName.c_str()), functionName, moduleName, time, info));
    }
    
    void AJLogger::logStr(LogType type,int line,std::string srcFileName,std::string functionName,std::string moduleName,long time,char const* fmt,...)
    {
        va_list ap,args;
        va_start(ap, fmt);
        va_copy(args, ap);
        int size = vsnprintf(NULL, 0, fmt, ap);
        char * buffer = (char *)malloc(size+1);
        memset(buffer, 0x00, size+1);
        vsnprintf(buffer, size+1, fmt, args);
        va_end(ap);
        std::string info = buffer;
        free(buffer);
        this->logStr(type, line, srcFileName, functionName, moduleName, time, info);
    }
    
    void AJLogger::logBin(LogType type,int line,std::string srcFileName,std::string functionName,std::string moduleName,long time,const BYTE* buffer, int size)
    {
        std::vector<BYTE> info;
        info.assign(buffer, buffer+size);
        this->logBin(type, line, srcFileName, functionName, moduleName, time, info);
    }
    
    void AJLogger::logBin(LogType type,int line,std::string srcFileName,std::string functionName,std::string moduleName,long time,std::vector<BYTE> info)
    {
        if (!m_logEnabled)
        {
            return;
        }
        
        int tid = (int)pthread_mach_thread_np(pthread_self());
        this->logMsg(AJUtil::makeLogMessage(type, line, tid, srcFileName, functionName, moduleName, time, info));
    }
    
    void AJLogger::logMsg(AJLogMessage* pMsg)
    {
        AJLogFilterChain filterChain;
        if (!filterChain.doFilter(pMsg, &filterChain))
        {
            m_loggerWorker->asyncWriteLog(pMsg);
        }
    }
    
    void AJLogger::addLogFilter(AJLogFilter* filter)
    {
        AJLogFilterChain::addFilter(filter);
    }
    
    void AJLogger::removeLogFilter(AJLogFilter* filter)
    {
        AJLogFilterChain::removeLogFilter(filter);
    }
    
    bool AJLogger::addLocalLogger(AJAbstractLogger* ptlogger)
    {
        assert(ptlogger != NULL);
        return m_loggerWorker->addLogger(ptlogger);
    }
}
