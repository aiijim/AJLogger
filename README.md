# AJLogger
AJLogger is a log library on iOS/Mac, it implement use c++, supply object-c and c++ interface.

##Usage For Object-C
set output to console

```
[AJOCLogger addLogOutput:ConsoleTye filePath:nil];
```

set output to file

```
NSString* dirPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject]; 
NSString* filePath = [dirPath stringByAppendingPathComponent:@"test.log"]; 
[AJOCLogger addLogOutput:FileType filePath:filePath];
```

set output to sqlite database

```
NSString* dirPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject]; 
NSString* filePath = [dirPath stringByAppendingPathComponent:@"test.sql"];
[AJOCLogger addLogOutput:SQLiteType filePath:filePath];
```

open log 

```
[AJOCLogger openLogger];
```

close log

```
[AJOCLogger closeLogger];
```

log string

```
AJOCLogError("tag", @"haha");	
AJOCLogWarning("tag", @"haha");	
AJOCLogInfo("tag", @"haha");	
AJOCLogDebug("tag", @"haha");	
AJOCLogVerbose("tag", @"haha");	
```

log function entry 

```
AJOCFuncLog("tag");
```
###Log filter
tag(module) filter

```
[AJOCLogger setModuleFilterEnabled:YES mode:NO mdls:@[@"tag"]];
```
log level filter

```
[AJOCLogger setTypeFilterEnabled:YES];
[AJOCLogger setLogLevel:InfoLevel];
```