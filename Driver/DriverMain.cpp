#include"pch.h"
#include"DriverMain.h"
#include"GetProcessInformation.h"
#include"Link.h"




extern"C"
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	NTSTATUS su = STATUS_SUCCESS;
	UNICODE_STRING tempDevName{ 0 };
	UNICODE_STRING tempSymName{ 0 };
	PDEVICE_OBJECT pDevObj{ NULL };

	UNREFERENCED_PARAMETER(RegistryPath);
	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = GetProcessDetailsCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = GetProcessDetailsCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = GetProcessDetailsDeviceControl;


	do
	{
		RtlInitUnicodeString(&tempDevName, DEVICE_NAME);
		RtlInitUnicodeString(&tempSymName, KERNEL_SYMBOLICLINK_NAME);
		su = IoCreateDevice(DriverObject,
			sizeof(DEVOBJ_EXTENSION),
			&tempDevName,
			FILE_DEVICE_UNKNOWN,
			0,
			FALSE,
			&pDevObj
		);
		if (!NT_SUCCESS(su))
		{
			break;
		}
		//set device I/O way
		pDevObj->Flags |= DO_BUFFERED_IO;

		su = IoCreateSymbolicLink(&tempSymName, &tempDevName);
		if (!NT_SUCCESS(su))
		{
			break;
		}
	} while (false);

	return su;
}

void DriverUnload(PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNICODE_STRING tempSymName{ 0 };
	RtlInitUnicodeString(&tempSymName, KERNEL_SYMBOLICLINK_NAME);
	IoDeleteSymbolicLink(&tempSymName);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS GetProcessDetailsCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)		
{
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS GetProcessDetailsDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	NTSTATUS su = STATUS_SUCCESS;
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG userOutBufferSize = stack->Parameters.DeviceIoControl.OutputBufferLength;
	PVOID pUserOutBuffer = (PVOID)Irp->AssociatedIrp.SystemBuffer;
	ULONG processBufferSize = 0;
	ULONG doSize = 0;
	Link tempLink;

	switch (stack->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_GETPROCESS_DETAILS:
		do
		{
			su = CreateProcessDetailsLink(&tempLink.linkHeader);
			if (!NT_SUCCESS(su))
			{
				break;
			}
			//Get Buffer Size
			if (pUserOutBuffer == NULL)
			{
				su = GetLinkNodeContentSize(&tempLink.linkHeader, &processBufferSize);
				if (!NT_SUCCESS(su))
				{
					break;
				}
			}
			//Input content to Out Buffer
			else
			{
				su = InputUserBuffer(&tempLink.linkHeader,pUserOutBuffer, userOutBufferSize,&doSize);
				if (su == STATUS_BUFFER_OVERFLOW)
				{
					break;
				}
			}
		} while (false);

		break;
	default:
		break;
	}

	if(pUserOutBuffer)
		Irp->IoStatus.Information = doSize;
	else
		Irp->IoStatus.Information = processBufferSize;
	Irp->IoStatus.Status = su;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return su;
}

NTSTATUS InputUserBuffer(_In_ PLIST_ENTRY pListHeader, _Inout_ PVOID pUserBuffer, _In_ ULONG userBufferSize,_Out_ ULONG* doSize)
{
	NTSTATUS su = STATUS_SUCCESS;
	PLIST_ENTRY moveEntry = pListHeader->Flink;
	PLinkNode tempNode = NULL;
	PUNICODE_STRING tempUni = NULL;
	ULONG tempNodeSize = 0;
	ULONG moveStep = 0;
	while (moveEntry != pListHeader)
	{
		tempNode = CONTAINING_RECORD(moveEntry, LinkNode, linkNodeEntry);
		tempUni = (PUNICODE_STRING)tempNode->processPath;
		tempNodeSize = tempUni->MaximumLength + sizeof(UNICODE_STRING)+sizeof(ULONG);


		if (userBufferSize <tempNodeSize)
		{
			//do not copy any
			//memcpy((PVOID)((ULONG)pUserBuffer + moveStep), (PVOID)tempNode, userBufferSize);
			userBufferSize = 0;
			//moveStep += userBufferSize;

			su = STATUS_BUFFER_OVERFLOW;
			break;
		}
		else
		{
			//copy a Node and unicode_string
			memcpy((PVOID)((ULONG)pUserBuffer + moveStep), tempNode, sizeof(ULONG));
			memcpy(
				(PVOID)((ULONG)pUserBuffer + moveStep + sizeof(ULONG)),
				tempUni,
				tempNodeSize - sizeof(ULONG)
			);
			
			moveStep += tempNodeSize;
			userBufferSize -= tempNodeSize;
		}

		moveEntry = moveEntry->Flink;
	}

	*doSize = moveStep;
	return su;
}


