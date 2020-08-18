
#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef __ABEEP_H__
#define __ABEEP_H__

#include <windows.h>

BOOL WINAPI OpenBeepDevice (VOID);
BOOL WINAPI AsyncMakeBeep (ULONG Frequency);
BOOL WINAPI AsyncBeep (ULONG Frequency, ULONG Duration);
BOOL WINAPI CloseBeepDevice (VOID);

#endif /* __ABEEP_H__ */
