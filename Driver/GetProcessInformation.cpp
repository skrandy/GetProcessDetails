#include"pch.h"
#include"GetProcessInformation.h"
#include"PathConversion.h"
#include <minwindef.h>
#define SystemProcessInformation 0x5

typedef NTSTATUS
(*pfnZwQueryInformationProcess)(
	__in HANDLE ProcessHandle,
	__in PROCESSINFOCLASS ProcessInformationClass,
	__out_bcount(ProcessInformationLength) PVOID ProcessInformation,
	__in ULONG ProcessInformationLength,
	__out_opt PULONG ReturnLength
	);
pfnZwQueryInformationProcess ZwQueryInformationProcess = NULL;



NTSTATUS GetProcessDevicePath(_In_ PEPROCESS tempEPROCESS, PUNICODE_STRING processDevicePath)
{
	NTSTATUS su = STATUS_SUCCESS;
	HANDLE hProcess = NULL;
	ULONG realSize = 0;
	PVOID pBuffer = NULL;
	PUNICODE_STRING tempUni = NULL;
	do
	{
		su = ObOpenObjectByPointer(tempEPROCESS, 0, 0, 0, 0, KernelMode, &hProcess);
		if (!NT_SUCCESS(su))
		{
			break;
		}
		if (ZwQueryInformationProcess == NULL)
		{
			UNICODE_STRING tempFuncName{ 0 };
			RtlInitUnicodeString(&tempFuncName, L"ZwQueryInformationProcess");
			ZwQueryInformationProcess = (pfnZwQueryInformationProcess)MmGetSystemRoutineAddress(&tempFuncName);
			if (ZwQueryInformationProcess == NULL)
			{
				su = STATUS_UNSUCCESSFUL;
				break;
			}
		}
		su = ZwQueryInformationProcess(hProcess,
			ProcessImageFileName,
			0,
			0,
			&realSize
		);
		if (su != STATUS_INFO_LENGTH_MISMATCH)
		{
			break;
		}
		pBuffer = ExAllocatePool(PagedPool, realSize);
		if (pBuffer == NULL)
		{
			su = STATUS_NO_MEMORY;
			break;
		}
		su = ZwQueryInformationProcess(hProcess,
			ProcessImageFileName,
			pBuffer,
			realSize,
			&realSize
		);
		while (su == STATUS_INFO_LENGTH_MISMATCH)
		{
			ExFreePool(pBuffer);
			pBuffer = ExAllocatePool(PagedPool, realSize);
			if (pBuffer == NULL)
			{
				su = STATUS_NO_MEMORY;
				break;
			}
			su = ZwQueryInformationProcess(hProcess,
				ProcessImageFileName,
				pBuffer,
				realSize,
				&realSize);
		}
		if (!NT_SUCCESS(su))
		{
			break;
		}

		tempUni = (PUNICODE_STRING)pBuffer;
		processDevicePath->Length = tempUni->Length;
		processDevicePath->MaximumLength = tempUni->MaximumLength;
		processDevicePath->Buffer = (PWCH)ExAllocatePool(PagedPool, tempUni->MaximumLength);
		if (processDevicePath->Buffer == NULL)
		{
			su = STATUS_NO_MEMORY;
			break;
		}
		memcpy(processDevicePath->Buffer, tempUni->Buffer, tempUni->Length);
	} while (false);

	if (hProcess)
		ZwClose(hProcess);
	if (pBuffer)
		ExFreePool(pBuffer);
	return su;
}



extern "C" NTKERNELAPI  NTSTATUS 
ZwQuerySystemInformation(IN ULONG SystemInformationClass,
	IN OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL);

typedef struct _SYSTEM_PROCESS_INFORMATION {
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	UCHAR Reserved1[48];
	UNICODE_STRING ImageName;
	KPRIORITY BasePriority;
	HANDLE UniqueProcessId;
	PVOID Reserved2;
	ULONG HandleCount;
	ULONG SessionId;
	PVOID Reserved3;
	SIZE_T PeakVirtualSize;
	SIZE_T VirtualSize;
	ULONG Reserved4;
	SIZE_T PeakWorkingSetSize;
	SIZE_T WorkingSetSize;
	PVOID Reserved5;
	SIZE_T QuotaPagedPoolUsage;
	PVOID Reserved6;
	SIZE_T QuotaNonPagedPoolUsage;
	SIZE_T PagefileUsage;
	SIZE_T PeakPagefileUsage;
	SIZE_T PrivatePageCount;
	LARGE_INTEGER Reserved7[6];
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;
NTSTATUS GetPids(HANDLE* pids, ULONG pidsLength, ULONG* ReturnLength);
NTSTATUS GetSystemPids(_Out_ PHANDLE* PIDS, _Out_ ULONG* NumberOfPid)
{
	HANDLE* pids = NULL;
	ULONG pidSize = 0;
	NTSTATUS su = STATUS_SUCCESS;
	do
	{
		su = GetPids(pids, 0x0, &pidSize);
		while (su == STATUS_INFO_LENGTH_MISMATCH)
		{
			if (pids)
				ExFreePool(pids);
			pids = (HANDLE*)ExAllocatePool(PagedPool, pidSize);
			if (pids == NULL)
			{
				su = STATUS_NO_MEMORY;
				break;
			}
			memset(pids, 0x0, pidSize);

			su = GetPids(pids, pidSize, &pidSize);
		}
		if (!NT_SUCCESS(su))
			break;


	} while (false);
	*PIDS = pids;
	*NumberOfPid = pidSize / sizeof(HANDLE);
	return su;
}
NTSTATUS GetPids(HANDLE* pids, ULONG pidsLength, ULONG* ReturnLength)
{
	UNICODE_STRING funUni{ 0 };
	NTSTATUS su = STATUS_SUCCESS;
	ULONG realSize = 0;
	PVOID pBuffer = NULL;
	ULONG moveBuffer = NULL;
	ULONG pidNum = 0;
	PSYSTEM_PROCESS_INFORMATION tempPro = NULL;
	ULONG tempNext = 0;
	do
	{

		//Get SystemProcessInformation Buffer
		su = ZwQuerySystemInformation(
			SystemProcessInformation,
			NULL,
			NULL,
			&realSize
		);
		while (su == STATUS_INFO_LENGTH_MISMATCH)
		{
			if (pBuffer)
				ExFreePool(pBuffer);
			pBuffer = ExAllocatePool(PagedPool, realSize);
			if (pBuffer == NULL)
			{
				su = STATUS_NO_MEMORY;
				break;
			}
			memset(pBuffer, 0x0, realSize);
			su = ZwQuerySystemInformation(
				SystemProcessInformation,
				pBuffer,
				realSize,
				&realSize
			);
		}
		if (!NT_SUCCESS(su))
			break;

		//Parse Buffer
		moveBuffer = (ULONG)pBuffer;

		if (pids == NULL)
		{
			do// Get Number of Process
			{
				tempPro = (PSYSTEM_PROCESS_INFORMATION)moveBuffer;
				tempNext = tempPro->NextEntryOffset;
				pidNum++;
				moveBuffer += tempNext;
			} while (tempNext != 0);
			*ReturnLength = pidNum * sizeof(HANDLE);
			su = STATUS_INFO_LENGTH_MISMATCH;
			break;
		}
		//Get Number of Process And Copy Pids to Buffer
		do
		{
			tempPro = (PSYSTEM_PROCESS_INFORMATION)moveBuffer;
			tempNext = tempPro->NextEntryOffset;
			if (pidNum * 4 < pidsLength)
			{
				pids[pidNum] = tempPro->UniqueProcessId;
			}
			pidNum++;
			moveBuffer += tempNext;
		} while (tempNext != 0);
		if (pidsLength >= pidNum)
			*ReturnLength = 0;
		*ReturnLength = pidNum * sizeof(HANDLE);
	} while (false);



	if (pBuffer)
		ExFreePool(pBuffer);
	return su;
}
