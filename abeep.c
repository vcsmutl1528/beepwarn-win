
#include <windows.h>
#include <winternl.h>

#include "abeep.h"

#define DD_BEEP_DEVICE_NAME_U  L"\\Device\\Beep"
#define IOCTL_BEEP_SET         CTL_CODE(FILE_DEVICE_BEEP, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _BEEP_SET_PARAMETERS
{
	ULONG Frequency;
	ULONG Duration;
} BEEP_SET_PARAMETERS, *PBEEP_SET_PARAMETERS;

#ifndef InitializeObjectAttributes
#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }
#endif

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

static HANDLE hAsyncBeepDeviceHandle = INVALID_HANDLE_VALUE;

BOOL WINAPI OpenBeepDevice (VOID)
{
	NTSTATUS status;
	UNICODE_STRING beep_device_name;
	OBJECT_ATTRIBUTES attribs;
	IO_STATUS_BLOCK status_block;

	if (hAsyncBeepDeviceHandle != INVALID_HANDLE_VALUE) {
		SetLastError (ERROR_ALREADY_INITIALIZED);
		return FALSE;
	}

	RtlInitUnicodeString (&beep_device_name, DD_BEEP_DEVICE_NAME_U);
	InitializeObjectAttributes (&attribs, &beep_device_name, 0, NULL, NULL);
	status = NtCreateFile (&hAsyncBeepDeviceHandle, FILE_READ_DATA | FILE_WRITE_DATA, &attribs,
		&status_block, NULL, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, 0,
		NULL, 0);

	return NT_SUCCESS(status);
}

BOOL WINAPI AsyncMakeBeep (ULONG Frequency)
{
	return AsyncBeep (Frequency, -1);
}

BOOL WINAPI AsyncBeep (ULONG Frequency, ULONG Duration)
{
	BEEP_SET_PARAMETERS BeepSetParameters;
	IO_STATUS_BLOCK status_block;
	NTSTATUS lStatus;

	if (hAsyncBeepDeviceHandle == INVALID_HANDLE_VALUE)
	{
		SetLastError (ERROR_INVALID_HANDLE);
		return FALSE;
	}
	BeepSetParameters.Frequency = Frequency;
	BeepSetParameters.Duration = Duration;
	lStatus = NtDeviceIoControlFile (hAsyncBeepDeviceHandle, NULL, NULL, NULL, &status_block,
		IOCTL_BEEP_SET, &BeepSetParameters, sizeof(BEEP_SET_PARAMETERS),
		NULL, 0);
	return NT_SUCCESS(lStatus);
}

BOOL WINAPI CloseBeepDevice (VOID)
{
	NTSTATUS status;

	if (hAsyncBeepDeviceHandle == INVALID_HANDLE_VALUE)
	{
		SetLastError (ERROR_INVALID_HANDLE);
		return FALSE;
	}
	status = NtClose (hAsyncBeepDeviceHandle);
	hAsyncBeepDeviceHandle = INVALID_HANDLE_VALUE;
	return NT_SUCCESS(status);
}
