//
//  AJFileLogger.cpp
//  AJLogger
//
//  Created by aiijim on 16/8/4.
//  Copyright © 2016年 aiijim. All rights reserved.
//

#include "AJLoggerPlus.h"
#include <cassert>

namespace AJLoggerPlus
{
    AJFileLogger::AJFileLogger(std::string fileName)
    {
        m_pFile = fopen(fileName.c_str(), "a+");
        if (m_pFile != NULL)
        {
            m_fileName = fileName;
            fseek(m_pFile, 0, SEEK_END);
        }
    }
    
    void AJFileLogger::logMessage(AJLogMessage* pMsg)
    {
        assert(NULL != pMsg);
        std::string msgInfo = AJUtil::makeLogText(pMsg);
        if (m_pFile == NULL)
        {
            fprintf(stdout, "%s", msgInfo.c_str());
        }
        else
        {
            fprintf(m_pFile, "%s", msgInfo.c_str());
        }
    }
    
    void AJFileLogger::endTransaction()
    {
        if (m_pFile == NULL)
        {
            fflush(stdout);
        }
        else
        {
            fflush(m_pFile);
        }
    }
    
    AJFileLogger::~AJFileLogger()
    {
        if(m_pFile != NULL)
        {
           fclose(m_pFile);
           m_pFile = NULL;
        }
    };
}
