/* ixlib_config.hh.  Generated automatically by configure.  */
/* ixlib_config.hh.in.  Generated automatically from configure.in by autoheader.  */
/* Special definitions, processed by autoheader.
   Copyright (C) 1995, 1996, 1997 Free Software Foundation.
   Ulrich Drepper <drepper@gnu.ai.mit.edu>, 1995.  */

/* Default value for alignment of strings in .mo file.  */
#define DEFAULT_OUTPUT_ALIGNMENT 1

#ifndef PARAMS
# if __STDC__
#  define PARAMS(args) args
# else
#  define PARAMS(args) ()
# endif
#endif



/* Define to the name of the distribution.  */
#define PACKAGE "ixlib"

/* Define to the version of the distribution.  */
#define VERSION "0.96.0"

/* Define if you have obstacks.  */
/* #undef HAVE_OBSTACK */

/* Define if <stddef.h> defines ptrdiff_t.  */
/* #undef HAVE_PTRDIFF_T */

/* Define if your locale.h file contains LC_MESSAGES.  */
#define HAVE_LC_MESSAGES 1

/* Define if you have the parse_printf_format function.  */
/* #undef HAVE_PARSE_PRINTF_FORMAT */

/* Define to 1 if NLS is requested.  */
#define ENABLE_NLS 1

/* Define as 1 if you have catgets and don't want to use GNU gettext.  */
/* #undef HAVE_CATGETS */

/* Define as 1 if you have gettext and don't want to use GNU gettext.  */
#define HAVE_GETTEXT 1

/* Define as 1 if you have the stpcpy function.  */
#define HAVE_STPCPY 1


/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Define if using `alloca.c'. */
/* #undef C_ALLOCA */

/* Define if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix). */
#define HAVE_ALLOCA_H 1

/* Define if you have the <argz.h> header file. */
#define HAVE_ARGZ_H 1

/* Define if you have the `dcgettext' function. */
#define HAVE_DCGETTEXT 1

/* Define if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if you have the `getcwd' function. */
#define HAVE_GETCWD 1

/* Define if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have the `i' library (-li). */
/* #undef HAVE_LIBI */

/* Define if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define if you have the <locale.h> header file. */
#define HAVE_LOCALE_H 1

/* Define if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* Define if you have the `munmap' function. */
#define HAVE_MUNMAP 1

/* Define if you have the <nl_types.h> header file. */
#define HAVE_NL_TYPES_H 1

/* Define if you have the `putenv' function. */
#define HAVE_PUTENV 1

/* Define if you have the `setenv' function. */
#define HAVE_SETENV 1

/* Define if you have the `setlocale' function. */
#define HAVE_SETLOCALE 1

/* Define if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define if you have the `stpcpy' function. */
#define HAVE_STPCPY 1

/* Define if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if you have the `__argz_count' function. */
#define HAVE___ARGZ_COUNT 1

/* Define if you have the `__argz_next' function. */
#define HAVE___ARGZ_NEXT 1

/* Define if you have the `__argz_stringify' function. */
#define HAVE___ARGZ_STRINGIFY 1

/* Select DOS line ends. */
/* #undef IXLIB_DOS_LINES */

/* The major version of the library. */
#define IXLIB_MAJOR_VERSION 0

/* The micro version of the library. */
#define IXLIB_MICRO_VERSION 0

/* The minor version of the library. */
#define IXLIB_MINOR_VERSION 96

/* Name of package */
#define PACKAGE "ixlib"

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
        STACK_DIRECTION > 0 => grows toward higher addresses
        STACK_DIRECTION < 0 => grows toward lower addresses
        STACK_DIRECTION = 0 => direction of growth unknown */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.96.0"

/* Define if `lex' declares `yytext' as a `char *' by default, not a `char[]'.
   */
#define YYTEXT_POINTER 1

/* Define if you need to in order for stat and other things to work. */
/* #undef _POSIX_SOURCE */

/* Substitute 'bool' by 'unsigned char' if necessary */
/* #undef bool */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Substitute 'false' by '0' if necessary */
/* #undef false */

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
   if it is not supported. */
/* #undef inline */

/* Define to `long' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

/* Substitute 'true' by '1' if necessary */
/* #undef true */

/* We don't test for the basename function but still want to use the
   version in the libc when compiling for a system using glibc.  */
#ifdef __GNU_LIBRARY__
# define HAVE_BASENAME	1
#endif

/* On GNU systems we want to use the extensions.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE	1
#endif


/* A file name cannot consist of any character possible.  INVALID_PATH_CHAR
   contains the characters not allowed.  */
#ifndef MSDOS
# define	INVALID_PATH_CHAR "\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37 \177/"
#else
/* Something like this for MSDOG.  */
# define	INVALID_PATH_CHAR "\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37 \177\\:."
#endif

/* Length from which starting on warnings about too long strings are given.
   Several systems have limits for strings itself, more have problems with
   strings in their tools (important here: gencat).  1024 bytes is a
   conservative limit.  Because many translation let the message size grow
   (German translations are always bigger) choose a length < 1024.  */
#define WARN_ID_LEN 900

/* This is the page width for the message_print function.  It should
   not be set to more than 79 characters (Emacs users will appreciate
   it).  It is used to wrap the msgid and msgstr strings, and also to
   wrap the file position (#:) comments.  */
#define PAGE_WIDTH 79
