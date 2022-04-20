#include"pch.h"
#include"Link.h"
#include"GetProcessInformation.h"
#include"PathConversion.h"


_Link::_Link()
{
	InitializeListHead(&this->linkHeader);
}

_Link::~_Link()
{
	PLIST_ENTRY pTempEntry = NULL;
	PLinkNode pTempLinkNode = NULL;

	while (!IsListEmpty(&this->linkHeader))
	{
		pTempEntry = RemoveHeadList(&this->linkHeader);
		pTempLinkNode = CONTAINING_RECORD(pTempEntry, LinkNode, linkNodeEntry);
		if (pTempLinkNode->processPath)
			ExFreePool(pTempLinkNode->processPath);
		ExFreePool(pTempLinkNode);
	}
}
NTSTATUS CreateProcessDetailsLink(_In_ PLIST_ENTRY pListHeader)
{
	PEPROCESS tempEPROCESS = NULL;
	NTSTATUS su = STATUS_SUCCESS;
	UNICODE_STRING tempDevicePath{ 0 };
	UNICODE_STRING tempDosPath{ 0 };
	HANDLE* pids = NULL;
	ULONG numberOfPid = 0;
	PLinkNode pTempNode = NULL;

	do
	{
		su = GetSystemPids(&pids,&numberOfPid);
		if (!NT_SUCCESS(su))
			break;
		for (ULONG i = 0; i < numberOfPid; i++)
		{
			if (pids[i] == 0 || pids[i] == (HANDLE)4)
				continue;
			do
			{
				su = PsLookupProcessByProcessId(pids[i], &tempEPROCESS);
				if (!NT_SUCCESS(su))
				{
					continue;
				}
				su = GetProcessDevicePath(tempEPROCESS, &tempDevicePath);
				if (!NT_SUCCESS(su))
				{
					break;
				}
				su = GetDosPathFromDevicePath(&tempDevicePath, &tempDosPath);
				if (!NT_SUCCESS(su))
				{
					break;
				}
				su = GetNode(&tempDosPath, &pTempNode, (ULONG)pids[i]);
				if (!NT_SUCCESS(su))
				{
					break;
				}
				su = InsertLink(pListHeader, pTempNode);
				if (!NT_SUCCESS(su))
				{
					break;
				}
			} while (false);
			if (tempDevicePath.Buffer != NULL)
				RtlFreeUnicodeString(&tempDevicePath);
			if (tempDosPath.Buffer != NULL)
				RtlFreeUnicodeString(&tempDosPath);
		}
	}while(false);
	if (pids)
		ExFreePool(pids);
	return su;
}
NTSTATUS GetNode(_In_ PUNICODE_STRING pUni, _Out_ PLinkNode* pLinkNode,_In_ ULONG pid)
{
	NTSTATUS su = STATUS_SUCCESS;
	ULONG sizeUni = sizeof(UNICODE_STRING);
	PVOID processPathBuffer{ 0 }, tempNodeBuffer{ 0 };
	PUNICODE_STRING tempUni = NULL;
	PLinkNode pTempNode = NULL;
	do
	{
		processPathBuffer = ExAllocatePool(PagedPool, pUni->MaximumLength + sizeUni);
		if (processPathBuffer == NULL)
		{
			su = STATUS_NO_MEMORY;
			break;
		}
		memcpy(processPathBuffer, pUni, sizeUni);
		memcpy((PVOID)((ULONG)processPathBuffer + sizeUni), (PVOID)pUni->Buffer, pUni->MaximumLength);
		tempUni = (PUNICODE_STRING)processPathBuffer;
		tempUni->Buffer = (PWCH)((ULONG)processPathBuffer + sizeUni);

		tempNodeBuffer = ExAllocatePool(PagedPool, sizeof(LinkNode));
		if (tempNodeBuffer == NULL)
		{
			su = STATUS_NO_MEMORY;
			break;
		}
		pTempNode = (PLinkNode)tempNodeBuffer;
		pTempNode->processPath = processPathBuffer;
		pTempNode->linkNodeEntry.Flink = pTempNode->linkNodeEntry.Blink = NULL;
		pTempNode->pid = pid;
	} while (false);

	if (!NT_SUCCESS(su))
	{
		if (processPathBuffer)
			ExFreePool(processPathBuffer);
		if (tempNodeBuffer)
			ExFreePool(tempNodeBuffer);
		return su;
	}
	*pLinkNode = pTempNode;
	return su;
}
NTSTATUS InsertLink(_In_ PLIST_ENTRY pLinkHeader, _In_ PLinkNode pNode)
{
	auto su = STATUS_SUCCESS;
	do
	{
		InsertHeadList(pLinkHeader, &pNode->linkNodeEntry);
	} while (false);
	return su;
}
NTSTATUS GetLinkNodeContentSize(_In_ PLIST_ENTRY pListHeader,_Out_ ULONG* ListContentSize)
{
	NTSTATUS su = STATUS_SUCCESS;
	ULONG tempSize = 0;
	PLinkNode pTempNode;
	PLIST_ENTRY moveEntry = NULL;
	PUNICODE_STRING pTempUni{ 0 };

	moveEntry = pListHeader->Flink;
	while (moveEntry != pListHeader)
	{
		pTempNode = CONTAINING_RECORD(moveEntry, _LinkNode, linkNodeEntry);
		pTempUni = (PUNICODE_STRING)pTempNode->processPath;
		tempSize += pTempUni->MaximumLength + sizeof(UNICODE_STRING)+sizeof(ULONG);
		moveEntry = moveEntry->Flink;
	}
	*ListContentSize = tempSize;

	return su;
}
