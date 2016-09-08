//
//  AJLoggerWorker.cpp
//  AJLogger
//
//  Created by aiijim on 16/8/3.
//  Copyright © 2016年 aiijim. All rights reserved.
//

#include "AJLoggerWorker.h"
#include "unistd.h"
#include <cassert>

namespace AJLoggerPlus
{
    void* AJLoggerWorker::thread_func(void * parm)
    {
        AJLoggerWorker* worker = (AJLoggerWorker*)parm;
        while (worker->m_isRunning)
        {
            if (worker->workQueueIsEmpty())
            {
                usleep(1000);
                continue;
            }
            
            worker->writeLog();
        }
        return (void *)0;
    }
    
    AJLoggerWorker::AJLoggerWorker()
    {
        m_isRunning = false;
        pthread_mutex_init(&m_lock, NULL);
    }
    
    AJLoggerWorker::~AJLoggerWorker()
    {
        pthread_mutex_destroy(&m_lock);
        for (std::vector<AJAbstractLogger*>::iterator iter = m_loggerArray.begin(); iter != m_loggerArray.end(); iter++)
        {
            delete (*iter);
        }
    }
    
    void AJLoggerWorker::start()
    {
        m_isRunning = true;
        pthread_create(&m_tid, NULL, AJLoggerWorker::thread_func, this);
    }
    
    void AJLoggerWorker::stop()
    {
        m_isRunning = false;
        void* status;
        pthread_join(m_tid, &status);
    }
    
    bool AJLoggerWorker::workQueueIsEmpty()
    {
        return m_logTasks.empty();
    }
    bool AJLoggerWorker::addLogger(AJAbstractLogger* ptlogger)
    {
        for (std::vector<AJAbstractLogger *>::iterator iter = m_loggerArray.begin(); iter != m_loggerArray.end(); iter++)
        {
            if ((*iter)->getLogFileName() == ptlogger->getLogFileName())
            {
                return false;
            }
        }
        m_loggerArray.push_back(ptlogger);
        return true;
    }
    
    void AJLoggerWorker::writeLog()
    {
        pthread_mutex_lock(&m_lock);
        bool isFirst = true;
        
        while (!m_logTasks.empty())
        {
            AJLogMessage* pMsg = m_logTasks.front();
            for (std::vector<AJAbstractLogger*>::iterator iter = m_loggerArray.begin(); iter != m_loggerArray.end(); iter++)
            {
                if (isFirst)
                {
                    (*iter)->beginTransaction();
                    isFirst = false;
                }
                
                (*iter)->logMessage(pMsg);
            }
            m_logTasks.pop();
            delete pMsg;
        }
        
        if (!isFirst)
        {
            for (std::vector<AJAbstractLogger*>::iterator iter = m_loggerArray.begin(); iter != m_loggerArray.end(); iter++)
            {
                (*iter)->endTransaction();
            }
        }
        pthread_mutex_unlock(&m_lock);
    }
    
    void AJLoggerWorker::asyncWriteLog(AJLogMessage* const pMsg)
    {
        assert(NULL != pMsg);
        pthread_mutex_lock(&m_lock);
        m_logTasks.push(pMsg);
        pthread_mutex_unlock(&m_lock);
    }
}
