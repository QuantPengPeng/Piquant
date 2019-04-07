// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

//之所以要在IService.h中定义SERVICE_API宏,是因为无论是父类IService还是其派生类CtpService都会包含该头文件,都能够使用上该宏
//_WIN32及SERVICELAYER_EXPORTS都是预定义宏,可通过属性页下的C/C++下的preprocessor下的preprocessor definitions来自定义配置,可通过预定义宏来做些控制操作
#ifdef _WIN32
#ifdef SERVICELAYER_EXPORTS
#define SERVICE_API _declspec(dllexport) 
#else
#define SERVICE_API _declspec(dllimport)
#endif
#else
#define SERVICE_API //若在非win32平台下,如liunx下,则SERVICE_API为空
#endif
//SERVICE_API即是_declspec(dllexport)表明该父类是用于导出的,不能说仅仅导出派生类而不导出父类,因为派生类依赖于父类

// TODO: reference additional headers your program requires here
