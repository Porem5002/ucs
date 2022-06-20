/**
 * LOGGER
 * 
 * Define PURGE_LOGGER to deactivate all logger messages
 * Define PURGE_LOGGER_ASSERT to deactivate all logger ASSERTIONS
 * Define PURGE_LOGGER_ERROR to deactivate all logger ERROR messages
 * Define PURGE_LOGGER_LOG to deactivate all logger LOG messages
 * 
 * Logger functions ending with S print a string as a message to the LOGGER_DESTINATION
 * Logger functions ending with F take extra arguments and print a formated message like fprintf to the LOGGER_DESTINATION
*/

#ifndef DEF_LOGGER_HEADER
#define DEF_LOGGER_HEADER 

#include <stdlib.h>
#include <stdio.h>

#ifndef PURGE_LOGGER

#ifndef LOGGER_DESTINATION
#define LOGGER_DESTINATION stderr
#endif

#ifndef PURGE_LOGGER_ASSERT
#define LOGGER_ASSERTF(CONDITION, FORMAT, ...)  if(!(CONDITION))                                                                    \
                                                {                                                                                   \
                                                    fprintf(LOGGER_DESTINATION, "ASSERTION FAILED (" ##CONDITION "): " FORMAT "\n", \
                                                            __VA_ARGS__);                                                           \
                                                    exit(EXIT_FAILURE);                                                                        \
                                                }                                                                                   

#define LOGGER_ASSERTS(CONDITION, STRING)   if(!(CONDITION))                                                                   \
                                            {                                                                                  \
                                                fputs("ASSERTION FAILED (" #CONDITION "): " STRING "\n", LOGGER_DESTINATION);  \
                                                exit(EXIT_FAILURE);                                                                       \
                                            }      
#else
#define LOGGER_ASSERTF(CONDITION, FORMAT, ...)
#define LOGGER_ASSERTS(CONDITION, STRING)
#endif

#ifndef PURGE_LOGGER_ERROR
#define LOGGER_ERRORF(FORMAT, ...)  fprintf(LOGGER_DESTINATION, "ERROR: " FORMAT "\n", __VA_ARGS__) 
#define LOGGER_ERRORS(STRING)       fputs("ERROR: " STRING "\n", LOGGER_DESTINATION)
#else
#define LOGGER_ERRORF(FORMAT, ...)
#define LOGGER_ERRORS(STRING)
#endif

#ifndef PURGE_LOGGER_LOG
#define LOGGER_LOGF(FORMAT, ...)    fprintf(LOGGER_DESTINATION, "LOG: " FORMAT "\n", __VA_ARGS__)  
#define LOGGER_LOGS(STRING)         fputs("LOG: " STRING "\n", LOGGER_DESTINATION)
#else
#define LOGGER_LOGF(FORMAT, ...)  
#define LOGGER_LOGS(STRING)
#endif

#else

#define LOGGER_ASSERTF(CONDITION, FORMAT, ...)
#define LOGGER_ASSERTS(CONDITION, STRING)
#define LOGGER_ERRORF(FORMAT, ...)
#define LOGGER_ERRORS(STRING)
#define LOGGER_LOGF(FORMAT, ...)  
#define LOGGER_LOGS(STRING)

#endif

#endif