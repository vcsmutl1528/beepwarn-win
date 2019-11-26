
#ifndef __ABEEP_H__
#define __ABEEP_H__

#include <windows.h>

extern HANDLE AsyncBeepDeviceHandle;

BOOL WINAPI OpenBeepDevice (VOID);
BOOL WINAPI AsyncBeep (ULONG Frequency);
BOOL WINAPI AsyncBeepDuration (ULONG Frequency, ULONG Duration);
BOOL WINAPI CloseBeepDevice (VOID);

#endif /* __ABEEP_H__ */
