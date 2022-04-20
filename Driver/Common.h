#pragma once

#define USER_SYMBOLICLINK_NAME L"\\\\.\\SnA1lGoTestSymBolicLinkName"
#define GETPROCESS_DEVICE	0x8000
#define IOCTL_GETPROCESS_DETAILS	CTL_CODE(GETPROCESS_DEVICE,0x800,\
 METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct ProcessDetails
{
	ULONG pid{ 0 };
	void* processPath{ 0 };//PUNICODT_STRING
}*PProcessDetails;


