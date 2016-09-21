/* Copyright Avcon, Inc. All rights reserved.
 *
 * File:rtklog.h
 *
 * Auth: Maozongwu
 * Create: 2014-6-30
 * Modify: 2014-6-30
 */
#ifndef __RTK_LOG_H__
#define __RTK_LOG_H__
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef RTK_LOG_EXPORT
#define RTK_LOG_API _declspec(dllexport)
#elif RTK_LOG_DLL
#define RTK_LOG_API _declspec(dllimport)
#else
#define RTK_LOG_API
#endif

enum LogLevel{L_ALL = 0, L_NORMAL, L_INFO, L_WARNING, L_ERROR};

// RtkLog fuction.
RTK_LOG_API void L_Init(int logLevel, const char* pathName);
RTK_LOG_API void L_Printf(const char*file, int line, int level, const char *format,...);
RTK_LOG_API void L_Deinit();

// Marco for easy use.
#define LA(format, ...) L_Printf(__FILE__, __LINE__,L_ALL,format, ##__VA_ARGS__)
#define LN(format, ...) L_Printf(__FILE__, __LINE__,L_NORMAL,format, ##__VA_ARGS__)
#define LI(format, ...) L_Printf(__FILE__, __LINE__,L_INFO,format, ##__VA_ARGS__)
#define LW(format, ...) L_Printf(__FILE__, __LINE__,L_WARNING,format, ##__VA_ARGS__)
#define LE(format, ...) L_Printf(__FILE__, __LINE__,L_ERROR,format, ##__VA_ARGS__)

#endif	//	__RTK_LOG_H__

