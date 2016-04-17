//-----------------------------------------------------------------------+
//                                                                       |
// This file is part of Z1                                               |
//                                                                       |
// file: z1_log.h                                                        |
// module: common                                                        |
//                                                                       |
// There is no Copyright on this file. It is in the public domain.       |
//                                                                       |
//-----------------------------------------------------------------------+

#ifndef __Z1_LOG__
#define __Z1_LOG__

#include <pthread.h>
#include <glib.h>

#include "z1_types.h"

// global variables (see z1_log.c for explanations)
extern pthread_mutex_t g_mutexLog;
extern FILE* g_fOutLogCsv;
extern GRegex *g_regexLog1;
extern GRegex *g_regexLog2;

// functions
extern void z1_initLog();               // Logging initialization
extern char* z1_displayPointer(U64 p);  // Display a pointer in 4 digit hex groups

// CAUTION CAUTION CAUTION CAUTION
// this function must NEVER be called except by Z1_LOG_OUTPUT_CSV
extern void z1_logCsv(gchar* p_message);

// Logging levels
#define Z1_LOGLV_PRODUCTION 0
#define Z1_LOGLV_CRITICAL 10
#define Z1_LOGLV_ERROR 20
#define Z1_LOGLV_WARNING 30
#define Z1_LOGLV_INFO 40
#define Z1_LOGLV_DEBUG 50

// Default values for the main logging level and the screen logging level
// All messages below Z1_LOG_LEVEL are logged to the csv file
// but only thsoe below Z1_LOG_LEVEL_SCREEN are displayed to stdout 
#define Z1_LOG_LEVEL Z1_LOGLV_DEBUG
#define Z1_LOG_LEVEL_SCREEN Z1_LOGLV_DEBUG

// to log a message, call the appropriate level-specific logging macro:
// Z1_LOG_PRODUCTION, Z1_LOG_CRITICAL, Z1_LOG_ERROR, Z1_LOG_WARNING, Z1_LOG_INFO or Z1_LOG_DEBUG
// Each of these macro calls Z1_LOG_COMMON
// This is made necessary by the interplay of 2 factors:
// - The macros must expand to nothing if their level is below Z1_LOG_LEVEL
// - The screen display level must be selectable independently

// Output to the CSV file (see z1_logCsv in z1_log.c for details)
// Only called at the end of Z1_LOG_COMMON because it cleans the string and add quotes and \n at the end
// All other csv output is done directly with fprintf(g_fOutLogCsv, ...)
#define Z1_LOG_OUTPUT_CSV(...) z1_logCsv(g_strdup_printf(__VA_ARGS__));

// Common functions of all level-specific macros
// \033[0m --> Escape sequence to set text attributes back to default
// (undo the effect of the escape sequence passed in p_color)
#define Z1_LOG_COMMON(p_level, p_color, p_type, ...){ \
    pthread_mutex_lock(&g_mutexLog); \
    \
    if(p_level <= Z1_LOG_LEVEL_SCREEN){ \
        printf("[%s:%d][%s] %s%s\033[0m: ", __FILE__, __LINE__, __func__, p_color, p_type); \
        printf(__VA_ARGS__); \
        printf("\n"); \
    } \
    \
    fprintf(g_fOutLogCsv, "\"%s\";%d;\"%s\";\"%s\";", __FILE__, __LINE__, __func__, p_type); \
    Z1_LOG_OUTPUT_CSV(__VA_ARGS__) \
    \
    pthread_mutex_unlock(&g_mutexLog); \
}

// Level specific logging macros
// PRODUCTION --> green  (standard color)
#if Z1_LOGLV_PRODUCTION <= Z1_LOG_LEVEL
#define Z1_LOG_PRODUCTION(...) \
    Z1_LOG_COMMON(Z1_LOGLV_PRODUCTION, "\033[32m", "PROD", __VA_ARGS__) 
#else
#define Z1_LOG_PRODUCTION(...)
#endif

// CRITICAL --> intense red  (standard color + intense flag)
#if Z1_LOGLV_CRITICAL <= Z1_LOG_LEVEL
#define Z1_LOG_CRITICAL(...) \
    Z1_LOG_COMMON(Z1_LOGLV_CRITICAL, "\033[31;1m", "CRITICAL", __VA_ARGS__) 
#else
#define Z1_LOG_CRITICAL(...)
#endif

// ERROR --> red  (standard color)
#if Z1_LOGLV_ERROR <= Z1_LOG_LEVEL
#define Z1_LOG_ERROR(...) \
    Z1_LOG_COMMON(Z1_LOGLV_ERROR, "\033[31m", "ERROR", __VA_ARGS__) 
#else
#define Z1_LOG_ERROR(...)
#endif

// WARNING --> orange  (256 color set))
#if Z1_LOGLV_WARNING <= Z1_LOG_LEVEL
#define Z1_LOG_WARNING(...) \
    Z1_LOG_COMMON(Z1_LOGLV_WARNING, "\033[38;5;208m", "WARNING", __VA_ARGS__) 
#else
#define Z1_LOG_WARNING(...)
#endif

// INFO --> blue (256 color set))
#if Z1_LOGLV_INFO <= Z1_LOG_LEVEL
#define Z1_LOG_INFO(...) \
    Z1_LOG_COMMON(Z1_LOGLV_INFO, "\033[38;5;27m", "INFO", __VA_ARGS__) 
#else
#define Z1_LOG_INFO(...)
#endif

// DEBUG --> gray (standard color) 
#if Z1_LOGLV_DEBUG <= Z1_LOG_LEVEL
#define Z1_LOG_DEBUG(...) \
    Z1_LOG_COMMON(Z1_LOGLV_DEBUG, "\033[37m", "DEBUG", __VA_ARGS__) 
#else
#define Z1_LOG_DEBUG(...)
#endif

// for color escape code info, see:
// https://en.wikipedia.org/wiki/ANSI_escape_code

#endif // __Z1_LOG__
