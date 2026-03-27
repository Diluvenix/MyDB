#ifndef LOGGING_H
#define LOGGING_H

#if defined (_DEBUG) || defined (_INFO) || defined (_WARNING) || defined (_ERROR)
#include <stdio.h>
#include <string.h>
#include <errno.h>
#define LOGGING_error(format, ...) { fprintf(stderr, "[ERROR (" __FILE__ ":%d)] " format "\n", __LINE__ __VA_OPT__(,) __VA_ARGS__); }
#define LOGGING_perror(format, ...) { fprintf(stderr, "[ERROR (" __FILE__ ":%d)] " format ": %s\n", __LINE__ __VA_OPT__(,) __VA_ARGS__, strerror(errno)); }
#else
#define LOGGING_error(...) { ; }
#define LOGGING_perror(...) { ; }
#endif

#if defined (_DEBUG) || defined (_INFO) || defined (_WARNING)
#define LOGGING_warning(format, ...) { fprintf(stdout, "[WARNING (" __FILE__ ":%d)] " format "\n", __LINE__ __VA_OPT__(,) __VA_ARGS__); }
#else
#define LOGGING_warning(...) { ; }
#endif

#if defined (_DEBUG) || defined (_INFO)
#define LOGGING_info(format, ...) { fprintf(stdout, "[INFO (" __FILE__ ":%d)] " format "\n", __LINE__ __VA_OPT__(,) __VA_ARGS__); }
#define LOGGING_EXECUTE_ON_INFO(EXP) EXP;
#else
#define LOGGING_info(...) { ; }
#define LOGGING_EXECUTE_ON_INFO(EXP) { ; }
#endif

#ifdef _DEBUG
#define LOGGING_debug(format, ...) { fprintf(stdout, "[DEBUG (" __FILE__ ":%d)] " format "\n", __LINE__ __VA_OPT__(,) __VA_ARGS__); }
#else
#define LOGGING_debug(...) { ; }
#endif

#endif