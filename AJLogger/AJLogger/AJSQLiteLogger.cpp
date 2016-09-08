//
//  AJSQLiteLogger.cpp
//  AJLogger
//
//  Created by aiijim on 16/8/9.
//  Copyright © 2016年 aiijim. All rights reserved.
//

#include "AJLoggerPlus.h"
#include <sstream>
#include <cassert>
#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Transaction.h"

namespace AJLoggerPlus
{
    AJSQLiteLogger::AJSQLiteLogger(std::string fileName):m_fileName(fileName)
    {
        m_pdb = new SQLite::Database(fileName, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
        std::string timeStr = AJUtil::getCurrentTimeString();
        m_tableStr = "tb" + timeStr;
        if(!m_pdb->tableExists(m_tableStr.c_str()))
        {
            std::ostringstream sstr;
            sstr<<"CREATE TABLE "<<m_tableStr
            <<" (id INTEGER PRIMARY KEY, type INTEGER, line INTEGER, tid INTEGER, file TEXT, function TEXT, mdl TEXT, time INTEGER, isBinary INTEGER, info TEXT, binary BLOB)";
            printf("%s",sstr.str().c_str());
            m_pdb->exec(sstr.str());
        }
    }
    
    AJSQLiteLogger::~AJSQLiteLogger()
    {
        if (m_pTransaction != NULL)
        {
            delete m_pTransaction;
            m_pTransaction = NULL;
        }
        
        if (m_pdb != NULL)
        {
            delete m_pdb;
            m_pdb = NULL;
        }
    }
    
    void AJSQLiteLogger::beginTransaction()
    {
        if (m_pTransaction == NULL)
        {
            m_pTransaction = new SQLite::Transaction(*m_pdb);
        }
    }
    
    void AJSQLiteLogger::logMessage(AJLogMessage* pMsg)
    {
        assert(NULL != pMsg);
        std::ostringstream sstr;
        sstr<<"INSERT INTO " <<m_tableStr<<" VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
        SQLite::Statement   query(*m_pdb, sstr.str());
        
        query.bind(1, pMsg->m_type);
        query.bind(2, pMsg->m_lineNum);
        query.bind(3, pMsg->m_tid);
        query.bind(4, pMsg->m_srcFileName);
        query.bind(5, pMsg->m_functionName);
        query.bind(6, pMsg->m_moduleName);
        query.bind(7, (int)pMsg->m_time);
        query.bind(8, pMsg->m_isBinaryData);
        query.bind(9, pMsg->m_info);
        query.bind(10, pMsg->m_binaryInfo.data(), (int)pMsg->m_binaryInfo.size());
        
        query.exec();
    }
    
    void AJSQLiteLogger::endTransaction()
    {
        if(m_pTransaction != NULL)
        {
            m_pTransaction->commit();
            delete m_pTransaction;
            m_pTransaction = NULL;
        }
    }
}
