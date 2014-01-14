#ifndef _LOGGER_H_
#define _LOGGER_H_

// Standard C dependencies
#include <stdio.h>

#define LOG_EMERGENCY_ON
#define LOG_ALERT_ON
#define LOG_CRITICAL_ON
#define LOG_ERROR_ON
#define LOG_WARNING_ON
#define LOG_NOTICE_ON
#define LOG_INFO_ON
#define LOG_DEBUG_ON

#ifdef LOG_EMERGENCY_ON
    #define LOG_EMERGENCY(fmt,...)      LOG_MESSAGE("EMERGENCY", "\t", fmt,##__VA_ARGS__)
#else
    #define LOG_EMERGENCY(fmt,...)
#endif

#ifdef LOG_ALERT_ON
    #define LOG_ALERT(fmt,...)          LOG_MESSAGE("ALERT", "\t\t", fmt,##__VA_ARGS__)
#else
    #define LOG_ALERT
#endif

#ifdef LOG_CRITICAL_ON
    #define LOG_CRITICAL(fmt,...)       LOG_MESSAGE("CRITICAL", "\t", fmt,##__VA_ARGS__)
#else
    #define LOG_CRITICAL(fmt,...)
#endif

#ifdef LOG_ERROR_ON
    #define LOG_ERROR(fmt,...)          LOG_MESSAGE("ERROR", "\t\t", fmt,##__VA_ARGS__)
#else
    #define LOG_ERROR(fmt,...)
#endif

#ifdef LOG_WARNING_ON
    #define LOG_WARNING(fmt,...)        LOG_MESSAGE("WARNING", "\t", fmt,##__VA_ARGS__)
#else
    #define LOG_WARNING(fmt,...)
#endif

#ifdef LOG_NOTICE_ON
    #define LOG_NOTICE(fmt,...)         LOG_MESSAGE("NOTICE", "\t", fmt,##__VA_ARGS__)
#else
    #define LOG_NOTICE(fmt,...)
#endif

#ifdef LOG_INFO_ON
    #define LOG_INFO(fmt,...)           LOG_MESSAGE("INFO", "\t\t", fmt,##__VA_ARGS__)
#else
    #define LOG_INFO(fmt,...)
#endif

#ifdef LOG_DEBUG_ON
    #define LOG_DEBUG(fmt,...)          LOG_MESSAGE("DEBUG", "\t\t", fmt,##__VA_ARGS__)
#else
    #define LOG_DEBUG(fmt,...)
#endif

#define LOG_MESSAGE(level,tabs,format,...) { printf("[%s]%s", level, tabs); printf(format, ##__VA_ARGS__); printf("\n"); }

#endif /* _LOGGER_H_ */
