#ifndef LOG_HPP_
#define LOG_HPP_

#include "stdio.h"
#include <stdarg.h>

// basic logging mechanism
#define DEBUG_ERROR(...)		log_debug_error(__FILE__,__LINE__,__VA_ARGS__)
#define DEBUG_WARN(...) 		log_debug_warning(__FILE__,__LINE__,__VA_ARGS__)
#define DEBUG_INFO(...)			log_debug_info(__FILE__,__LINE__,__VA_ARGS__)

#define RELEASE_ERROR(...)		log_release_error(__VA_ARGS__)
#define RELEASE_WARN(...) 	log_release_warning(__VA_ARGS__)
#define RELEASE_INFO(...)		log_release_info(__VA_ARGS__)

static inline void log_debug_error(const char * file, int line,
		const char * msg, ...) {
	va_list ap;
	va_start(ap, msg);
	fprintf(stderr, "[DEBUG ERROR]: ");
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\t\t[%s l%d]\n", file, line);
}

static inline void log_debug_warning(const char * file, int line,
		const char * msg, ...) {
	va_list ap;
	va_start(ap, msg);
	fprintf(stderr, "[DEBUG WARN]: ");
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\t\t[%s l%d]\n", file, line);
}

static inline void log_debug_info(const char * file, int line, const char * msg,
		...) {
	va_list ap;
	va_start(ap, msg);
	fprintf(stderr, "[DEBUG INFO]: ");
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\t\t[%s l%d]\n", file, line);
}

static inline void log_release_error(const char * msg, ...) {
	va_list ap;
	va_start(ap, msg);
	fprintf(stderr, "[RELEASE ERROR]: ");
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");
}

static inline void log_release_warning(const char * msg, ...) {
	va_list ap;
	va_start(ap, msg);
	fprintf(stderr, "[RELEASE WARN]: ");
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");
}

static inline void log_release_info(const char * msg, ...) {
	va_list ap;
	va_start(ap, msg);
	fprintf(stderr, "[RELEASE INFO]: ");
	vfprintf(stderr, msg, ap);
	fprintf(stderr, "\n");
}

#endif /* LOG_HPP_ */
