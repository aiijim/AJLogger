//
//  AJOCLogger.m
//  AJLogger
//
//  Created by aiijim on 16/8/4.
//  Copyright © 2016年 aiijim. All rights reserved.
//

#import "AJOCLogger.h"
#include "AJLoggerPlus.h"

void funcLoggerCleanUp(__strong AJOCFuncLogger **ppLogger)
{
    AJOCFuncLogger* funcLogger = *ppLogger;
    [AJOCLogger logStr:InfoLevel
                  line:funcLogger.line
               srcFile:[funcLogger.file UTF8String]
              function:""
                module:[funcLogger.mdl UTF8String]
                format:@"func <---- %s",[funcLogger.func UTF8String]];
}

static LogType level_static = ErrorType;

@implementation AJOCLogger

#pragma mark -- public

+ (void) addLogOutput:(OutputType)type filePath:(NSString*)path
{
    AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();
    AJLoggerPlus::AJAbstractLogger* ptlogger = NULL;
    switch (type)
    {
        case ConsoleTye:
        {
            ptlogger = new AJLoggerPlus::AJFileLogger();
            break;
        }

        case FileType:
        {
            ptlogger = new AJLoggerPlus::AJFileLogger([path UTF8String]);
            break;
        }
            
        case SQLiteType:
        {
            try
            {
                ptlogger = new AJLoggerPlus::AJSQLiteLogger([path UTF8String]);
            }
            catch (std::exception& e)
            {
                fprintf(stderr, "sqlite exception:%s\n",e.what());
            }
            break;
        }
    }
    
    if((ptlogger != NULL) && !logger->addLocalLogger(ptlogger))
    {
        delete ptlogger;
    }
}

+ (void) openLogger
{
    AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();
    logger->open();
}

+ (void) closeLogger
{
    AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();
    logger->close();
}

+ (void) setModuleFilterEnabled:(BOOL)enable mode:(BOOL)excludeMode mdls:(NSArray*)mdls;
{
    static AJLoggerPlus::AJLogModuleFilter* filter = NULL;
    AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();
    if (enable)
    {
        if (filter == NULL)
        {
            filter = new AJLoggerPlus::AJLogModuleFilter();
        }
        
        std::vector<std::string> temp;
        for (NSString* mdl in mdls)
        {
            temp.push_back([mdl UTF8String]);
        }
        filter->setModuleFilter(temp);
        filter->setExcludeMode(excludeMode ? true : false);
        logger->addLogFilter(filter);
    }
    else
    {
        if (filter != NULL)
        {
            logger->removeLogFilter(filter);
            delete filter;
            filter = NULL;
        }
    }
}

+ (void) setTypeFilterEnabled:(BOOL)enable
{
    static AJLoggerPlus::AJLogFilter* levelFilter = NULL;
    AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();
    if (enable)
    {
        if (levelFilter == NULL)
        {
            levelFilter = new AJLoggerPlus::AJLogFilter([](AJLoggerPlus::AJLogMessage* pMsg){
                
                if (pMsg->m_type > level_static)
                {
                    return true;
                }
                
                return false;
            });
            
            logger->addLogFilter(levelFilter);
        }
    }
    else
    {
        if (levelFilter != NULL)
        {
            logger->removeLogFilter(levelFilter);
            delete levelFilter;
            levelFilter = NULL;
        }
    }
}

+ (void) setLogLevel:(LogLevel) lvl
{
    level_static = (LogType)lvl;
}

+ (void) logStr:(LogLevel)level
           line:(int)line
        srcFile:(const char *const)srcFileName
       function:(const char *const)functionName
         module:(const char *const)moduleName
         format:(NSString*)fmt, ... NS_FORMAT_FUNCTION(6,7)
{
    AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();
    long time_info = time(NULL);

    va_list ap;
    if (fmt)
    {
        va_start(ap, fmt);
        NSString* message = [[NSString alloc] initWithFormat:fmt arguments:ap];
        va_end(ap);
        std::string info = [message UTF8String];
        logger->logStr((LogType)level, line, AJLoggerPlus::AJUtil::getFileName(srcFileName), functionName, moduleName, time_info, info);
    }
}

+ (void) logBin:(LogLevel)level
         module:(const char *)moduleName
           line:(int)line
        srcFile:(const char *)srcFileName
       function:(const char *)functionName
         binary:(NSData*)binary
{
    AJLoggerPlus::AJLogger * logger = AJLoggerPlus::AJLogger::getSharedInstance();
    if (binary)
    {
        logger->logBin((LogType)level, line, AJLoggerPlus::AJUtil::getFileName(srcFileName), functionName, moduleName, time(NULL), (BYTE*)[binary bytes], (int)binary.length);
    }
}


@end

@implementation AJOCFuncLogger

- (instancetype) initWithModule:(const char*)mdl
                           func:(const char*)func
                           file:(const char*)file
                           line:(int)line
{
    self = [super init];
    if (self)
    {
        [AJOCLogger logStr:InfoLevel line:line srcFile:file function:"" module:mdl format:@"func ----> %s",func];
        self.mdl = [NSString stringWithCString:mdl encoding:NSUTF8StringEncoding];
        self.func = [NSString stringWithCString:func encoding:NSUTF8StringEncoding];
        self.file = [NSString stringWithCString:file encoding:NSUTF8StringEncoding];
        self.line = line;
    }
    return self;
}

@end
