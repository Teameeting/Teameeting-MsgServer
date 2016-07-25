//RTC_Export.h

#ifndef __RTC_EXPORT_H__
#define __RTC_EXPORT_H__

#ifdef RTC_EXPORTS
#define RTC_API _declspec(dllexport)
#elif RTC_DLL
#define RTC_API _declspec(dllimport)
#else
#define RTC_API
#endif

#endif	// __RTC_EXPORT_H__