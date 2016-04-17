//-----------------------------------------------------------------------+
//                                                                       |
// This file is part of Z1                                               |
//                                                                       |
// file: z1_log.c                                                        |
// module: common                                                        |
//                                                                       |
// There is no Copyright on this file. It is in the public domain.       |
//                                                                       |
//-----------------------------------------------------------------------+

#include "z1_log.h"

pthread_mutex_t g_mutexLog;     // To make all logging operations thread-safe
FILE* g_fOutLogCsv;             // File to the CSV output
GRegex *g_regexLog1;            // Regex to normalize spaces (\s+ --> ' ')
GRegex *g_regexLog2;            // Regex to duplicate double quotes 

// Initialize all logging related globals
void z1_initLog(){
    // logging critical section access mutex
    pthread_mutex_init(&g_mutexLog, NULL);
    
    // CSV output file
    g_fOutLogCsv = fopen("z1_log.csv", "w");
    fprintf(g_fOutLogCsv, "\"FILE\";\"LINE\";\"FUNCTION\";\"TYPE\";\"MESSAGE\"\n");
    
    // Glib Regex Syntax : https://developer.gnome.org/glib/stable/glib-regex-syntax.html
    // Glib Regex creation flags: 
    // https://developer.gnome.org/glib/stable/glib-Perl-compatible-regular-expressions.html#GRegexCompileFlags
    // Glib Regex examples:
    // http://lzone.de/examples/Glib%20GRegex
    // https://ewgeny.wordpress.com/2012/04/07/mighty-perl-like-regular-expressions-regex-with-glib-and-vala/
    GError *l_error = NULL;
    // regex for \s+ --> ' ' 
    g_regexLog1 = g_regex_new( "\\s+", G_REGEX_OPTIMIZE, 0, &l_error );  
    // regex for " --> ""
    g_regexLog2 = g_regex_new( "\"", G_REGEX_OPTIMIZE, 0, &l_error );  

    // Regex test
    gchar* l_new = g_regex_replace (g_regexLog1, "toto\ntutu", -1, 0, " ", 0, &l_error );
    Z1_LOG_DEBUG("Regex spaces replacement test result [%s] (and \" too!)", l_new);
    g_free(l_new);

    // test of all message levels
    Z1_LOG_DEBUG("Test")
    Z1_LOG_INFO("Test")
    Z1_LOG_WARNING("Test")
    Z1_LOG_ERROR("Test")
    Z1_LOG_CRITICAL("Test")
    Z1_LOG_PRODUCTION("Test")
}

// Message output to the CSV file :
// Normalize spaces (no \n, \t etc and only single spaces)
// CAUTION : this function must only be called from the macro Z1_LOG_OUTPUT_CSV
// It assumes that the message string has just been allocated (by g_strdup_printf)
// and therefore frees it in the end
void z1_logCsv(gchar* p_message){
    GError *l_error = NULL; 
    
    // \s+ --> ' '
    gchar* l_newLogBufer1 = 
        g_regex_replace (g_regexLog1, p_message, -1, 0, " ", 0, &l_error );
    // " --> ""
    gchar* l_newLogBufer2 = 
        g_regex_replace (g_regexLog2, l_newLogBufer1, -1, 0, "\"\"", 0, &l_error ); 
        
    fprintf(g_fOutLogCsv, "\"%s\"\n", l_newLogBufer2); 
    
    g_free(l_newLogBufer1);
    g_free(l_newLogBufer2);
    
    // freeing the parameter string CAUTION CAUTION CAUTION CAUTION CAUTION
    g_free(p_message); 
}

// Displays a 64b pointer in groups of four hexa digits
char* z1_displayPointer(U64 p){
    // static is ok here because logging is protected by a mutex
    // therefore, only one thread at a time can be using this function
    static char l_display[20];
    
    sprintf(l_display, "%04llX %04llX %04llX %04llX", 
        p/0x1000000000000,
        (p/0x100000000)%0x10000,
        (p/0x10000)%0x10000,
        p%0x10000
    );
    
    return l_display;
}