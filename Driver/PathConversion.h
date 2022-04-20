#pragma once


/*
	Author		:	SnA1lGo
	Data		:	2022/3/31
	Action		:	Get File Dos Path From File Nt Path
	Parameters	:
		filePath :	File Nt Path
		dosPath  :	File Dos Path
	Remarks		:
		Need to call ExFreePool to Free File Dos Path
	Return Value:
		returns STATUS_SUCCESS or an error NTSTATUS value
*/
NTSTATUS GetDosPathFromFilePath(_In_ PUNICODE_STRING filePath, _Out_ PUNICODE_STRING dosPath);



/*
	Author		:	SnA1lGo
	Data		:	2022/3/31
	Action		:	Get Device Dos Path From Device Nt Path
	Parameters	:
		devicePath£º		Device Nt Path
		dosPath£º			Device Dos Path
	Remarks		:
		Need to call ExFreePool to Free Nt Dos Path
	Return Value:
		returns STATUS_SUCCESS or an error NTSTATUS value
*/
NTSTATUS GetDosPathFromDevicePath(_In_ PUNICODE_STRING devicePath, _Out_ PUNICODE_STRING dosPath);