#ifndef __ULOG_CONFIG_H
#define __ULOG_CONFIG_H

// Unless ULOG_ENABLED is defined at compile time, all logging is disabled and
// no logging code is generated.  To enable logging, uncomment the next line or
// add -DULOG_ENABLED=1 to your compiler switches.
#ifndef ULOG_ENABLED
  #define ULOG_ENABLED 1
#endif


// define the maximum number of concurrent subscribers
#define ULOG_MAX_SUBSCRIBERS 6
// maximum length of formatted log message
#define ULOG_MAX_MESSAGE_LENGTH 128
// enable file and line info
#define ULOG_PRINT_FILE_LINE_INFO 1


#endif