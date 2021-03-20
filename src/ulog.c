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
 * \file ulog.c
 *
 * \brief uLog: lightweight logging for embedded systems
 *
 * See ulog.h for sparse documentation.
 */

#include "ulog.h"
#include "ulog_config.h"

#if (ULOG_ENABLED == 1)  // whole file...

#include <stdio.h>
#include <string.h>
#include <stdarg.h>


// =============================================================================
// types and definitions

typedef struct {
  ulog_function_t fn;
  ulog_level_t threshold;
} subscriber_t;

// =============================================================================
// local storage

static struct {
  subscriber_t subscribers[ULOG_MAX_SUBSCRIBERS];
  char msg[ULOG_MAX_MESSAGE_LENGTH];
  bool quite;
  ulog_lock_t lock_fn;
} ulog_config;


// =============================================================================
// local functions

static void lock(bool lock) {
  if(ulog_config.lock_fn != NULL) {
    ulog_config.lock_fn(lock);
  }
}

// =============================================================================
// user-visible code

void ulog_init() {
  memset(ulog_config.subscribers, 0, sizeof(ulog_config.subscribers));
  memset(ulog_config.msg, 0, ULOG_MAX_MESSAGE_LENGTH);
  ulog_config.quite = false;
  ulog_config.lock_fn = NULL;
}

// search the subscribers table to install or update fn
ulog_err_t ulog_subscribe(ulog_function_t fn, ulog_level_t threshold) {
  int available_slot = -1;
  lock(true);
  for (int i=0; i<ULOG_MAX_SUBSCRIBERS; i++) {
    if (ulog_config.subscribers[i].fn == fn) {
      // already subscribed: update threshold and return immediately.
      ulog_config.subscribers[i].threshold = threshold;
      lock(false);
      return ULOG_ERR_NONE;

    } else if (ulog_config.subscribers[i].fn == NULL) {
      // found a free slot
      available_slot = i;
    }
  }
  // fn is not yet a subscriber.  assign if possible.
  if (available_slot == -1) {
    lock(false);
    return ULOG_ERR_SUBSCRIBERS_EXCEEDED;
  }
  ulog_config.subscribers[available_slot].fn = fn;
  ulog_config.subscribers[available_slot].threshold = threshold;
  lock(false);
  return ULOG_ERR_NONE;
}

// search the subscribers table to remove
ulog_err_t ulog_unsubscribe(ulog_function_t fn) {
  ulog_err_t ret = ULOG_ERR_NOT_SUBSCRIBED;
  lock(true);
  for (int i=0; i<ULOG_MAX_SUBSCRIBERS; i++) {
    if (ulog_config.subscribers[i].fn == fn) {
      ulog_config.subscribers[i].fn = NULL;    // mark as empty
      ret = ULOG_ERR_NONE;
      break;
    }
  }
  lock(false);
  return ret;
}

void ulog_set_lock(ulog_lock_t lock_fn) {
  ulog_config.lock_fn = lock_fn;
}

const char *ulog_level_name(ulog_level_t severity) {
  switch(severity) {
   case ULOG_TRACE_LEVEL: return "TRACE";
   case ULOG_DEBUG_LEVEL: return "DEBUG";
   case ULOG_INFO_LEVEL: return "INFO";
   case ULOG_WARNING_LEVEL: return "WARNING";
   case ULOG_ERROR_LEVEL: return "ERROR";
   case ULOG_CRITICAL_LEVEL: return "CRITICAL";
   default: return "UNKNOWN";
  }
}

void ulog_set_quite(bool set) {
  ulog_config.quite = set;
}

void ulog_message(ulog_level_t severity, const char *file, int line, const char *fmt, ...) {
  if(ulog_config.quite) {
    return;
  }
  lock(true);
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(ulog_config.msg, ULOG_MAX_MESSAGE_LENGTH, fmt, ap);
  va_end(ap);

  for (int i=0; i<ULOG_MAX_SUBSCRIBERS; i++) {
    if (ulog_config.subscribers[i].fn != NULL) {
      if (severity >= ulog_config.subscribers[i].threshold) {
        ulog_config.subscribers[i].fn(severity, file, line, ulog_config.msg);
      }
    }
  }
  lock(false);
}

// =============================================================================
// private code

#endif  // #ifdef ULOG_ENABLED
