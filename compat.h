#ifndef __COMPAT_H__
#define __COMPAT_H__

#ifndef ATTRALIGN
#if defined(__GNUC__) || defined(HAVE_ATTRIBUTE_ALIGNED)
#define ATTRALIGN(x) __attribute__((aligned(x)))
#elif defined(_MSC_VER)
#define ATTRALIGN(x) __declspec(align(x))
#else
#define ATTRALIGN(x)
#endif /* __GNUC__ */
#endif /* ATTRALIGN */

#ifdef WIN32

#include <windows.h>

#define sleep(secs) Sleep((secs) * 1000)

enum {
	PRIO_PROCESS		= 0,
};

static inline int setpriority(int which, int who, int prio)
{
	return -!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
}

#endif /* WIN32 */

#ifdef _MSC_VER

#ifndef strcasecmp
#define strcasecmp _stricmp
#endif

#ifndef strncasecmp
#define strncasecmp _strnicmp
#endif

#include <time.h>

#ifndef HAVE_GETTIMEOFDAY
static inline int gettimeofday(struct timeval* tv, void* tz)
{
	tv->tv_sec = (long)time(NULL);
	tv->tv_usec = 0;

	return (0);
}
#endif /* HAVE_GETTIMEOFDAY */

#ifdef HAVE_BASETSD_H
#ifndef SSIZE_T
#include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif
#else
#ifndef HAVE_SSIZE_T
typedef long ssize_t;
#endif
#endif

#ifndef HAVE_GETOPT_H
#define HAVE_GETOPT_LONG 1
//#include "getopt.h"
#endif

#endif /* _MSC_VER */

#endif /* __COMPAT_H__ */
