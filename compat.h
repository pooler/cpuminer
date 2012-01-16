#ifndef __COMPAT_H__
#define __COMPAT_H__

#ifdef WIN32

#include <windows.h>

static inline void sleep(int secs)
{
	Sleep(secs * 1000);
}

enum {
	PRIO_PROCESS		= 0,
};

static inline int setpriority(int which, int who, int prio)
{
	return -!SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
}

#endif /* WIN32 */

#endif /* __COMPAT_H__ */
