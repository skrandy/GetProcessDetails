#pragma once
#include"Common.h"
#define DEVICE_NAME L"\\Device\\SnA1lGoTestDeviceName"
#define KERNEL_SYMBOLICLINK_NAME L"\\??\\SnA1lGoTestSymBolicLinkName"


void DriverUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS GetProcessDetailsCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS GetProcessDetailsDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

NTSTATUS InputUserBuffer(_In_ PLIST_ENTRY pListHeader, _Inout_ PVOID pUserBuffer, _In_ ULONG userBufferSize,_Out_ ULONG* doSize);