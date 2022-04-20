#pragma once


/*
	Author			 :	

	Data			 :	2022/3/31
	Action			 :	by EPROCESS get process's Device Path
	Remarks			 :
		Need to call ExFreePool to Free Process Device Path
	Return Value:
		returns STATUS_SUCCESS or an error NTSTATUS value
*/
NTSTATUS GetProcessDevicePath(_In_ PEPROCESS tempEPROCESS, PUNICODE_STRING processDevicePath);
/*
	Author			 :	SnA1lGo
	Data			 :	2022/4/20
	Action			 :	Get all process's pid in system
	Parameters		 :
		PIDS		 :	Pointer to array
		NumberOfPid  :	Number of pids
	Remarks		:
		Need to call ExFreePool to Free Pids
	Return Value:
		returns STATUS_SUCCESS or an error NTSTATUS value
*/
NTSTATUS GetSystemPids(_Out_ PHANDLE* PIDS, _Out_ ULONG* NumberOfPid);
