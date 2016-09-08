//
//  AJOCLogger.h
//  AJLogger
//
//  Created by aiijim on 16/8/4.
//  Copyright © 2016年 aiijim. All rights reserved.
//

#import <Foundation/Foundation.h>

@class AJOCFuncLogger;

typedef NS_ENUM(NSUInteger, OutputType) {
    ConsoleTye,
    FileType,
    SQLiteType,
};

typedef NS_ENUM(NSUInteger, LogLevel){
    ErrorLevel,
    WarningLevel,
    InfoLevel,
    DebugLevel,
    VerboseLevel,
};

#ifndef LOG_SHORTCUT_MARCO
#define LOG_SHORTCUT_MARCO

#ifdef __cplusplus
extern "C" {
#endif
    void funcLoggerCleanUp(__strong AJOCFuncLogger **ppLogger);
#ifdef __cplusplus
}
#endif


#define AJOCFuncLog(mdl) __strong AJOCFuncLogger* logger __attribute__((cleanup(funcLoggerCleanUp), unused)) = [[AJOCFuncLogger alloc] initWithModule:#mdl func:__PRETTY_FUNCTION__ file:__FILE__ line:__LINE__]

#define AJOCLogError(mdl, frmt, ... )                        \
       [AJOCLogger      logStr:ErrorLevel                    \
                         line : __LINE__                     \
                      srcFile : __FILE__                     \
                     function : __PRETTY_FUNCTION__          \
                        module: #mdl                         \
                       format : (frmt), ##__VA_ARGS__]

#define AJOCLogWarning(mdl, frmt, ... )                      \
       [AJOCLogger      logStr:WarningLevel                  \
                         line : __LINE__                     \
                      srcFile : __FILE__                     \
                     function : __PRETTY_FUNCTION__          \
                        module: #mdl                         \
                       format : (frmt), ##__VA_ARGS__]

#define AJOCLogInfo(mdl, frmt, ... )                         \
        [AJOCLogger      logStr:InfoLevel                    \
                          line : __LINE__                    \
                       srcFile : __FILE__                    \
                      function : __PRETTY_FUNCTION__         \
                         module: #mdl                        \
                        format : (frmt), ##__VA_ARGS__]


#define AJOCLogDebug(mdl, frmt, ... )                        \
       [AJOCLogger      logStr:DebugLevel                    \
                         line : __LINE__                     \
                      srcFile : __FILE__                     \
                     function : __PRETTY_FUNCTION__          \
                        module: #mdl                         \
                       format : (frmt), ##__VA_ARGS__]


#define AJOCLogVerbose(mdl, frmt, ... )                      \
       [AJOCLogger      logStr:VerboseLevel                  \
                         line : __LINE__                     \
                      srcFile : __FILE__                     \
                     function : __PRETTY_FUNCTION__          \
                        module: #mdl                         \
                       format : (frmt), ##__VA_ARGS__]

#define AJOCLogBinError(mdl, bin)                            \
      [AJOCLogger        logBin : ErrorLevel                 \
                         module : #mdl                       \
                           line : __LINE__                   \
                        srcFile : __FILE__                   \
                       function : __PRETTY_FUNCTION__        \
                         binary : bin]

#define AJOCLogBinWarning(mdl, bin)                          \
      [AJOCLogger        logBin : WarningLevel               \
                         module : #mdl                       \
                           line : __LINE__                   \
                        srcFile : __FILE__                   \
                       function : __PRETTY_FUNCTION__        \
                         binary : bin]

#define AJOCLogBinInfo(mdl, bin)                             \
      [AJOCLogger        logBin : InfoLevel                  \
                         module : #mdl                       \
                           line : __LINE__                   \
                        srcFile : __FILE__                   \
                       function : __PRETTY_FUNCTION__        \
                         binary : bin]

#define AJOCLogBinDebug(mdl, bin)                            \
        [AJOCLogger        logBin : DebugLevel               \
                           module : #mdl                     \
                             line : __LINE__                 \
                          srcFile : __FILE__                 \
                         function : __PRETTY_FUNCTION__      \
                           binary : bin]

#define AJOCLogBinVerbose(mdl, bin)                          \
      [AJOCLogger        logBin : VerboseLevel               \
                         module : #mdl                       \
                           line : __LINE__                   \
                        srcFile : __FILE__                   \
                       function : __PRETTY_FUNCTION__        \
                         binary : bin]


#endif   /*end LOG_SHORTCUT_MARCO define*/

@interface AJOCLogger : NSObject

/**
 *  添加日志输出口
 *
 *  @param type 输出类型：ConsoleTye控制台,FileType文件,SQLiteType数据库,
 *  @param path 文件/数据库文件绝对地址，ConsoleTye忽略该参数
 */
+ (void) addLogOutput:(OutputType)type filePath:(NSString*)path;

/**
 *  开启日志功能
 */
+ (void) openLogger;

/**
 *  关闭日志功能
 */
+ (void) closeLogger;

/**
 *  启用/禁用 模块（Tag）过滤
 *
 *  @param enable      YES:启用 NO:禁用
 *  @param excludeMode YES:过滤模块数组的模块 NO:过滤非模块数组中的模块
 *  @param mdls        模块(Tags)数组
 */
+ (void) setModuleFilterEnabled:(BOOL)enable mode:(BOOL)excludeMode mdls:(NSArray*)mdls;

/**
 *  启用/禁用日志级别过滤
 */
+ (void) setTypeFilterEnabled:(BOOL)enable;

/**
 *  设置日志的记录级别
 *  @param level 日志的级别 error:0 warnging:1 info:2 debug:3 verbose:4
 */
+ (void) setLogLevel:(LogLevel) level;

/**
 *  记录字符串日志
 *
 *  @param level        日志级别
 *  @param line         行号
 *  @param srcFileName  源文件名
 *  @param functionName 函数名
 *  @param moduleName   模块名／Tag
 *  @param fmt          格式化字符串
 */
+ (void) logStr:(LogLevel)level
           line:(int)line
        srcFile:(const char *const)srcFileName
       function:(const char *const)functionName
         module:(const char *const)moduleName
         format:(NSString*)fmt, ... NS_FORMAT_FUNCTION(6,7);

/**
 *  记录二进制日志
 *
 *  @param level        日志级别
 *  @param moduleName   模块名／Tag
 *  @param line         行号
 *  @param srcFileName  源文件名
 *  @param functionName 函数名
 *  @param binary       二进制数据
 */
+ (void) logBin:(LogLevel)level
         module:(const char *)moduleName
           line:(int)line
        srcFile:(const char *)srcFileName
       function:(const char *)functionName
         binary:(NSData*)binary;

@end

/**
 *  记录函数出入的类
 */
@interface AJOCFuncLogger : NSObject

@property (nonatomic, strong) NSString* mdl;
@property (nonatomic, strong) NSString* func;
@property (nonatomic, strong) NSString* file;
@property (nonatomic, assign) int line;

- (instancetype) initWithModule:(const char*)mdl
                           func:(const char*)func
                           file:(const char*)file
                           line:(int)line;

@end
