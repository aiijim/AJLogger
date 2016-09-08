//
//  AJLoggerPlus.h
//  AJLogger
//
//  Created by aiijim on 16/8/3.
//  Copyright © 2016年 aiijim. All rights reserved.
//

#ifndef AJLoggerPlus_h
#define AJLoggerPlus_h

#include <iostream>
#include <pthread.h>
#include <queue>

#ifndef BYTE
#define BYTE unsigned char
#endif

typedef enum{
    ErrorType,
    WarningType,
    InfoType,
    DebugType,
    VerboseType,
} LogType;

//forward declaration
namespace SQLite
{
    class Database;
    class Transaction;
}

namespace AJLoggerPlus {
    struct AJLogMessage;
    class AJLogFilterChain;
    class AJLoggerWorker;
}
typedef bool (*FilterCallback) (AJLoggerPlus::AJLogMessage* ptMsg);

#define AJ_LogFunction(mdl) AJLoggerPlus::AJFuncLogger localLogger(mdl, __FUNCTION__);

#define AJ_LogError(module,fmt,...) do {  \
   AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();   \
   logger->logStr(ErrorType,__LINE__,__FILE__,__FUNCTION__,module,time(NULL),fmt,##__VA_ARGS__); \
} while(0)

#define AJ_LogWarning(module,fmt,...) do {  \
   AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();   \
   logger->logStr(WarningType,__LINE__,__FILE__,__FUNCTION__,module,time(NULL),fmt,##__VA_ARGS__); \
} while(0)

#define AJ_LogInfo(module,fmt,...) do {  \
   AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();   \
   logger->logStr(InfoType,__LINE__,__FILE__,__FUNCTION__,module,time(NULL),fmt,##__VA_ARGS__); \
} while(0)

#define AJ_LogDebug(module,fmt,...) do {  \
   AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();   \
   logger->logStr(DebugType,__LINE__,__FILE__,__FUNCTION__,module,time(NULL),fmt,##__VA_ARGS__); \
} while(0)

#define AJ_LogVerbose(module,fmt,...) do {  \
   AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();   \
   logger->logStr(VerboseType,__LINE__,__FILE__,__FUNCTION__,module,time(NULL),fmt,##__VA_ARGS__); \
} while(0)

#define AJ_LogBinError(module,buffer, size) do {  \
   AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();   \
   logger->logBin(ErrorType, __LINE__, __FILE__, __FUNCTION__, module, time(NULL), buffer, size); \
} while(0)

#define AJ_LogBinWarning(module,buffer, size) do {  \
   AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();   \
   logger->logBin(WarningType,__LINE__,__FILE__,__FUNCTION__,module,time(NULL),buffer, size); \
} while(0)

#define AJ_LogBinInfo(module,buffer, size) do {  \
   AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();   \
   logger->logBin(InfoType,__LINE__,__FILE__,__FUNCTION__,module,time(NULL),buffer, size); \
} while(0)

#define AJ_LogBinDebug(module,buffer, size) do {  \
   AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();   \
   logger->logBin(DebugType,__LINE__,__FILE__,__FUNCTION__,module,time(NULL),buffer, size); \
} while(0)

#define AJ_LogBinVerbose(module,buffer, size) do {  \
   AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();   \
   logger->logBin(VerboseType,__LINE__,__FILE__,__FUNCTION__,module,time(NULL),buffer, size); \
} while(0)



namespace AJLoggerPlus
{
    struct AJLogMessage
    {
        LogType m_type;
        int m_lineNum;
        int m_tid;
        std::string m_srcFileName;
        std::string m_functionName;
        std::string m_moduleName;
        long m_time;
        bool m_isBinaryData;
        std::string m_info;
        std::vector<BYTE> m_binaryInfo;
    };
    
    class AJAbstractLogger
    {
    public:
        AJAbstractLogger(){};
        virtual void beginTransaction(){};
        virtual void logMessage(AJLogMessage* pMsg){};
        virtual void endTransaction(){};
        virtual std::string getLogFileName(){ return ""; };
        virtual ~AJAbstractLogger(){};
    };
    
    class AJFileLogger : public AJAbstractLogger
    {
    public:
        AJFileLogger():m_fileName(""),m_pFile(NULL){};
        explicit AJFileLogger(std::string fileName);
        void beginTransaction(){};
        void logMessage(AJLogMessage* pMsg);
        void endTransaction();
        std::string getLogFileName(){ return m_fileName; };
        ~AJFileLogger();
    private:
        std::string m_fileName;
        FILE* m_pFile;
    };
    
    class AJSQLiteLogger : public AJAbstractLogger
    {
    public:
        explicit AJSQLiteLogger(std::string fileName);
        void beginTransaction();
        void logMessage(AJLogMessage* pMsg);
        void endTransaction();
        std::string getLogFileName(){ return m_fileName; };
        ~AJSQLiteLogger();
    private:
        SQLite::Database* m_pdb;
        SQLite::Transaction* m_pTransaction;
        std::string m_fileName;
        std::string m_tableStr;
    };
    
    class AJUtil
    {
    public:
        static std::string getCurrentTimeString();
        static std::string getCurrentTimeString(const long time);
        static const char* getFileName(const char* fileAbusoluteName);
        static std::string getLogPrefix(const LogType type);
        static std::string makeLogText(AJLogMessage* const pMsg);
        static AJLogMessage* makeLogMessage(LogType type,int line,int tid,std::string srcFileName,std::string functionName,std::string moduleName,long time,std::string info);
        static AJLogMessage* makeLogMessage(LogType type,int line,int tid,std::string srcFileName,std::string functionName,std::string moduleName,long time,std::vector<BYTE> info);
    };
    
    class AJLogFilter
    {
    public:
        AJLogFilter(FilterCallback callback):m_callback(callback){};
        //return true:日志被过滤 false：不需要过滤
        virtual bool doFilter(AJLogMessage* ptMsg,AJLogFilterChain* filterChain);
    protected:
        AJLogFilter(){};
        FilterCallback m_callback;
    };
    
    class AJLogModuleFilter : public AJLogFilter
    {
    public:
        AJLogModuleFilter();
        bool doFilter(AJLogMessage* ptMsg,AJLogFilterChain* filterChain);
        void setModuleFilter(std::vector<std::string> &mdls);
        void setModuleFilter(const char* mdls[], int len);
        void setModuleFilter(int count, ...);
        //true:过滤数组中包含的模块将给过滤，false:不包含在过滤数组中的模块将被过滤
        bool isExcludeMode(){ return m_excludeMode;} ;
        void setExcludeMode(bool isExcludeMode) { m_excludeMode = isExcludeMode; };
        
    private:
        std::vector<std::string> m_mdlFilter;
        bool m_excludeMode;
    };
    
    class AJLogFilterChain : public AJLogFilter
    {
    public:
        AJLogFilterChain():m_index(0){};
        static void addFilter(AJLogFilter* filter);
        static void removeLogFilter(AJLogFilter* filter);
        bool doFilter(AJLogMessage* ptMsg,AJLogFilterChain* filterChain);
    private:
        static std::vector<AJLogFilter*> m_filterChain;
        int m_index;
    };
    
    class AJLogger
    {
    public:
        static AJLogger* getSharedInstance();
        void open();
        void close();
        void destory();
        bool addLocalLogger(AJAbstractLogger* ptlogger);
        void addLogFilter(AJLogFilter* filter);
        void removeLogFilter(AJLogFilter* filter);
        void logStr(LogType type,int line,std::string srcFileName,std::string functionName,std::string moduleName,long time,std::string info);
        void logStr(LogType type,int line,std::string srcFileName,std::string functionName,std::string moduleName,long time,char const* fmt,...);
        void logBin(LogType type,int line,std::string srcFileName,std::string functionName,std::string moduleName,long time,const BYTE* buffer, int size);
        void logBin(LogType type,int line,std::string srcFileName,std::string functionName,std::string moduleName,long time,std::vector<BYTE> info);
        void logMsg(AJLogMessage* pMsg);
        ~AJLogger();
        
    private:
        AJLogger();
        AJLogger(const AJLogger& logger);
        AJLogger& operator=(AJLogger& rhs);
        AJLoggerWorker * m_loggerWorker;
        bool m_logEnabled;
        
        static AJLogger* m_instance;
        static pthread_mutex_t m_singleMutex;
    };
    
    class AJFuncLogger
    {
    public:
        AJFuncLogger(std::string module, std::string funcName):m_funcName(funcName),m_module(module)
        {
            AJ_LogInfo(m_module,"func ----> %s",m_funcName.c_str());
        };
        ~AJFuncLogger()
        {
            AJ_LogInfo(m_module,"func <---- %s",m_funcName.c_str());
        };
    private:
        std::string m_funcName;
        std::string m_module;
    };
};

#endif /* AJLoggerPlus_h */

