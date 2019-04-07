// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

//定义TOOL_API宏,是为了导出该dll中的类及友元函数
//_WIN32及TOOL_EXPORTS都是预定义宏,可通过属性页下的C/C++下的preprocessor下的preprocessor definitions来自定义配置,可通过预定义宏来做些控制操作
#ifdef _WIN32
#ifdef TOOL_EXPORTS
#define TOOL_API _declspec(dllexport) 
#else
#define TOOL_API _declspec(dllimport)
#endif
#else
#define TOOL_API //若在非win32平台下,如liunx下,则TOOL_API为空
#endif

// TODO: reference additional headers your program requires here
