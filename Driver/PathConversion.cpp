#include"pch.h"
#include"PathConversion.h"



NTSTATUS GetDosPathFromFilePath(_In_ PUNICODE_STRING filePath, _Out_ PUNICODE_STRING dosPath)
{
	NTSTATUS su = STATUS_UNSUCCESSFUL;
	PFILE_OBJECT pFileObject = NULL;
	POBJECT_NAME_INFORMATION pObjectNameInformation = NULL;
	HANDLE hFile = NULL;
	IO_STATUS_BLOCK ioStausBlock = { 0 };
	OBJECT_ATTRIBUTES oa = { 0 };
	do
	{
		if (!filePath)
		{
			su = STATUS_INVALID_PARAMETER;
			break;
		}
		InitializeObjectAttributes(&oa, filePath, OBJ_KERNEL_HANDLE, NULL, NULL);
		su = ZwOpenFile(&hFile, SYNCHRONIZE, &oa, &ioStausBlock, FILE_SHARE_READ, FILE_DIRECTORY_FILE);
		if (!NT_SUCCESS(su))
		{
			break;
		}
		su = ObReferenceObjectByHandle(hFile, FILE_ALL_ACCESS, *IoFileObjectType, KernelMode, (PVOID*)&pFileObject, NULL);
		if (!NT_SUCCESS(su))
		{
			break;
		}
		su = IoQueryFileDosDeviceName(pFileObject, &pObjectNameInformation);
		if (!NT_SUCCESS(su))
		{
			break;
		}
		dosPath->Length = pObjectNameInformation->Name.Length;
		dosPath->MaximumLength = pObjectNameInformation->Name.MaximumLength;
		dosPath->Buffer = (PWCH)ExAllocatePool(PagedPool, pObjectNameInformation->Name.MaximumLength);
		if (dosPath->Buffer == NULL)
		{
			su = STATUS_NO_MEMORY;
			break;
		}
		memcpy(dosPath->Buffer, pObjectNameInformation->Name.Buffer, dosPath->Length);
	} while (FALSE);

	if (hFile)
	{
		ZwClose(hFile);
	}

	if (pFileObject)
	{
		ObDereferenceObject(pFileObject);
	}

	if (pObjectNameInformation)
	{
		ExFreePool(pObjectNameInformation);
	}

	return su;
}


NTSTATUS GetDosPathFromDevicePath(_In_ PUNICODE_STRING devicePath, _Out_ PUNICODE_STRING dosPath)
{
	/*
	Remarks	:
		When both PFILE_OBJECT and PDEVICE_OBJECT are obtained through IoGetDeviceObjectPointer,
		only PFILE_OBJECT needs to be released. 
	*/
	NTSTATUS su = STATUS_SUCCESS;
	PFILE_OBJECT pFileObject = NULL;
	PDEVICE_OBJECT pDeviceObject = NULL;
	POBJECT_NAME_INFORMATION pObjectNameInformation = NULL;

	do
	{
		su = IoGetDeviceObjectPointer(devicePath, FILE_READ_DATA, &pFileObject, &pDeviceObject);
		if (!NT_SUCCESS(su))
		{
			break;
		}
		su = IoQueryFileDosDeviceName(pFileObject, &pObjectNameInformation);
		if (!NT_SUCCESS(su))
		{
			break;
		}
		dosPath->Length = pObjectNameInformation->Name.Length;
		dosPath->MaximumLength = pObjectNameInformation->Name.MaximumLength;
		dosPath->Buffer = (PWCH)ExAllocatePool(PagedPool, pObjectNameInformation->Name.MaximumLength);
		if (dosPath->Buffer == NULL)
		{
			break;
		}
		memcpy(dosPath->Buffer, pObjectNameInformation->Name.Buffer, pObjectNameInformation->Name.Length);
	} while (false);
	if (pFileObject != NULL)
		ObDereferenceObject(pFileObject);
	if (pObjectNameInformation != NULL)
		ExFreePool(pObjectNameInformation);
	return su;
}

