#ifndef __COMPAT_H__
#define __COMPAT_H__

#ifdef WIN32

#include <windows.h>

static inline void sleep(int secs)
{
	Sleep(secs * 1000);
}

#endif /* WIN32 */

#endif /* __COMPAT_H__ */
