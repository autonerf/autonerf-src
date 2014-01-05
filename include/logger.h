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
    #define LOG_EMERGENCY(message)      printf("[EMERGENCY]\t%s\n", message)
#else
    #define LOG_EMERGENCY(message)
#endif

#ifdef LOG_ALERT_ON
    #define LOG_ALERT(message)          printf("[ALERT]\t\t%s\n", message)
#else
    #define LOG_ALERT
#endif

#ifdef LOG_CRITICAL_ON
    #define LOG_CRITICAL(message)       printf("[CRITICAL]\t%s\n", message)
#else
    #define LOG_CRITICAL(message)
#endif

#ifdef LOG_ERROR_ON
    #define LOG_ERROR(message)          printf("[ERROR]\t\t%s\n", message)
#else
    #define LOG_ERROR(message)
#endif

#ifdef LOG_WARNING_ON
    #define LOG_WARNING(message)        printf("[WARNING]\t%s\n", message)
#else
    #define LOG_WARNING(message)
#endif

#ifdef LOG_NOTICE_ON
    #define LOG_NOTICE(message)         printf("[NOTICE]\t%s\n", message)
#else
    #define LOG_NOTICE(message)
#endif

#ifdef LOG_INFO_ON
    #define LOG_INFO(message)           printf("[INFO]\t\t%s\n", message)
#else
    #define LOG_INFO(message)
#endif

#ifdef LOG_DEBUG_ON
    #define LOG_DEBUG(message)          printf("[DEBUG]\t\t%s\n", message)
#else
    #define LOG_DEBUG(message)
#endif

#endif /* _LOGGER_H_ */
