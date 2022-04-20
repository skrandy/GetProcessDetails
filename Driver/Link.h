#pragma once
#include"Common.h"
typedef struct _LinkNode :ProcessDetails
{
	LIST_ENTRY linkNodeEntry{ 0 };
}LinkNode, * PLinkNode;

typedef struct  _Link
{
	_Link();
	~_Link();
	LIST_ENTRY linkHeader{ 0 };

}Link, * PLink;


/*
	Author			 :	SnA1lGo
	Data			 :	2022/4/20
	Action			 :	Get a new LinkNode
	Parameters		 :
		pUni		 :	Pointer to Process's Dos Path Unicode_string
		pLinkNode    :	Pointer to New LinkNode
		pid          :	Process's pid

	Remarks		:
		need to free new LinkNode
	Return Value:
		returns STATUS_SUCCESS or an error NTSTATUS value
*/
NTSTATUS GetNode(_In_ PUNICODE_STRING pUni, _Out_ PLinkNode* pLinkNode,_In_ ULONG pid);
/*
	Author			 :	SnA1lGo
	Data			 :	2022/4/20
	Action			 :	Insert LinkNode to pLinkHeader
	Parameters		 :
		pLinkHeader	 :	Pointer to Link's Header
		pNode        :	Pointer to LinkNode
	Return Value:
		returns STATUS_SUCCESS or an error NTSTATUS value
*/
NTSTATUS InsertLink(_In_ PLIST_ENTRY pLinkHeader, _In_ PLinkNode pNode);
/*
	Author			 :	SnA1lGo
	Data			 :	2022/4/20
	Action			 :	Traverse the processes in the system, 
						and turn each process information into a node and insert it into the linked list
	Parameters		 :
		pListHeader	 :	Pointer to Link's Header
	Return Value:
		returns STATUS_SUCCESS or an error NTSTATUS value
*/
NTSTATUS CreateProcessDetailsLink(_In_ PLIST_ENTRY pListHeader);
/*
	Author				:	SnA1lGo
	Data				:	2022/4/20
	Action				:	Get All Process's details Size 
						
	Parameters			:
		pListHeader		:	Pointer to Link's Header
		ListContentSize :	Pointer to a ULONG value
	Return Value:
		returns STATUS_SUCCESS or an error NTSTATUS value
*/
NTSTATUS GetLinkNodeContentSize(_In_ PLIST_ENTRY pListHeader, _Out_ ULONG* ListContentSize);
