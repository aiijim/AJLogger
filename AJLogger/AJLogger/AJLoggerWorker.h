//
//  AJLoggerWorker.h
//  AJLogger
//
//  Created by aiijim on 16/8/15.
//  Copyright © 2016年 aiijim. All rights reserved.
//

#ifndef AJLoggerWorker_h
#define AJLoggerWorker_h

#include "AJLoggerPlus.h"

namespace AJLoggerPlus
{
    class AJLoggerWorker
    {
    public:
        explicit AJLoggerWorker();
        ~AJLoggerWorker();
        void start();
        void stop();
        void asyncWriteLog(AJLogMessage* const pMsg);
        bool addLogger(AJAbstractLogger* ptlogger);
        
    private:
        std::queue<AJLogMessage *> m_logTasks;
        pthread_t m_tid;
        bool m_isRunning;
        pthread_mutex_t m_lock;
        std::vector<AJAbstractLogger *> m_loggerArray;
        
        static void* thread_func(void *);
        void writeLog();
        bool workQueueIsEmpty();
    };
}

#endif /* AJLoggerWorker_h */
