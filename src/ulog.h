/**
MIT License

Copyright (c) 2019 R. Dunbar Poor <rdpoor@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * \file
 *
 * \brief uLog: lightweight logging for embedded systems
 *
 * A quick intro by example:
 *
 *     #include "ulog.h"
 *
 *     // To use uLog, you must define a function to process logging messages.
 *     // It can write the messages to a console, to a file, to an in-memory
 *     // buffer: the choice is yours.  And you get to choose the format of
 *     // the message.  This example prints to the console.  One caveat: msg
 *     // is a static string and will be over-written at the next call to ULOG.
 *     // You may print it or copy it, but saving a pointer to it will lead to
 *     // confusion and astonishment.
 *     //
 *     void my_console_logger(ulog_level_t level, const char *msg) {
 *         printf("%s [%s]: %s\n",
 *             get_timestamp(),
 *             ulog_level_name(level),
 *             msg);
 *     }
 *
 *     int main() {
 *         ULOG_INIT();
 *
 *         // log to the console messages that are WARNING or more severe.  You
 *         // can re-subscribe at any point to change the severity level.
 *         ULOG_SUBSCRIBE(my_console_logger, ULOG_WARNING);
 *
 *         // log to a file messages that are DEBUG or more severe
 *         ULOG_SUBSCRIBE(my_file_logger, ULOG_DEBUG);
 *
 *         int arg = 42;
 *         ULOG_INFO("Arg is %d", arg);  // logs to file but not console
 *     }
 */

#ifndef ULOG_H_
#define ULOG_H_

#include <stdbool.h>
#include "ulog_config.h"

#ifdef __cplusplus
extern "C" {
    #endif

typedef enum {
  ULOG_TRACE_LEVEL,
  ULOG_DEBUG_LEVEL,
  ULOG_INFO_LEVEL,
  ULOG_WARNING_LEVEL,
  ULOG_ERROR_LEVEL,
  ULOG_CRITICAL_LEVEL,
  ULOG_LEVEL_N
} ulog_level_t;

#if (ULOG_ENABLED == 1)
  #define ULOG_INIT() ulog_init()
  #define ULOG_SUBSCRIBE(a, b) ulog_subscribe(a, b)
  #define ULOG_UNSUBSCRIBE(a) ulog_unsubscribe(a)
  #define ulog_level_name(a) ulog_level_name(a)
  #define ulog_set_quite(a) ulog_set_quite(a)
  #define ulog_set_lock(a) ulog_set_lock(a)
  #define ULOG_TRACE(...) ulog_message(ULOG_TRACE_LEVEL, __FILE__, __LINE__, __VA_ARGS__)
  #define ULOG_DEBUG(...) ulog_message(ULOG_DEBUG_LEVEL, __FILE__, __LINE__, __VA_ARGS__)
  #define ULOG_INFO(...) ulog_message(ULOG_INFO_LEVEL, __FILE__, __LINE__, __VA_ARGS__)
  #define ULOG_WARNING(...) ulog_message(ULOG_WARNING_LEVEL, __FILE__, __LINE__, __VA_ARGS__)
  #define ULOG_ERROR(...) ulog_message(ULOG_ERROR_LEVEL, __FILE__, __LINE__, __VA_ARGS__)
  #define ULOG_CRITICAL(...) ulog_message(ULOG_CRITICAL_LEVEL, __FILE__, __LINE__, __VA_ARGS__)
#else
  // uLog vanishes when disabled at compile time...
  #define ULOG_INIT()
  #define ULOG_SUBSCRIBE(a, b)
  #define ULOG_UNSUBSCRIBE(a)
  #define ulog_level_name(a)
  #define ulog_set_quite(a)
  #define ulog_set_lock(a)
  #define ULOG_TRACE(f, ...)
  #define ULOG_DEBUG(f, ...)
  #define ULOG_INFO(f, ...)
  #define ULOG_WARNING(f, ...)
  #define ULOG_ERROR(f, ...)
  #define ULOG_CRITICAL(f, ...)
#endif

typedef enum {
  ULOG_ERR_NONE = 0,
  ULOG_ERR_SUBSCRIBERS_EXCEEDED,
  ULOG_ERR_NOT_SUBSCRIBED,
} ulog_err_t;

/**
 * @brief: prototype for uLog subscribers.
 */
typedef void (*ulog_function_t)(ulog_level_t severity, const char *file, int line, char *msg);

/**
 * @brief: prototype for lock function.
 */
typedef void (*ulog_lock_t)(bool lock);


#if (ULOG_ENABLED == 1)
void ulog_init();
ulog_err_t ulog_subscribe(ulog_function_t fn, ulog_level_t threshold);
ulog_err_t ulog_unsubscribe(ulog_function_t fn);
void ulog_set_lock(ulog_lock_t lock_fn);
const char *ulog_level_name(ulog_level_t level);
void ulog_set_quite(bool set);
void ulog_message(ulog_level_t severity, const char *file, int line, const char *fmt, ...);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ULOG_H_ */
