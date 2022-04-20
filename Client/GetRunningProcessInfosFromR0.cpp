#include"GetRunningProcessInfosFromR0.h"
#include"../Driver/Common.h"
int GetProcessPathBuffer(PVOID* processBuffer, ULONG* realSize)
{
	HANDLE hFile = NULL;
	bool ret = false;
	int errorCode = 0;
	PVOID tempBuffer = 0;
	do
	{
		hFile = CreateFile(USER_SYMBOLICLINK_NAME,
			GENERIC_READ || GENERIC_WRITE,
			0,
			0,
			OPEN_EXISTING,
			0,
			0);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			errorCode = GetLastError();
			break;
		}
		ret = DeviceIoControl(hFile,
			IOCTL_GETPROCESS_DETAILS,
			NULL,
			NULL,
			NULL,
			NULL,
			realSize,
			NULL);
		if (!ret)
		{
			errorCode = GetLastError();
			break;
		}
		tempBuffer = malloc(*realSize);
		if (!tempBuffer)
		{
			errorCode = GetLastError();
			break;
		}
		ret = DeviceIoControl(hFile,
			IOCTL_GETPROCESS_DETAILS,
			0,
			0,
			tempBuffer,
			*realSize,
			realSize,
			NULL);
		if (!ret)
		{
			errorCode = GetLastError();
			while (errorCode == 234 && ret != true)
			{
				//if errorCode ==234 allocate more memory
				free(tempBuffer);
				*realSize += 0x1000;
				tempBuffer = malloc(*realSize);
				if (!tempBuffer)
				{
					errorCode = GetLastError();
					break;
				}
				ret = DeviceIoControl(hFile,
					IOCTL_GETPROCESS_DETAILS,
					0,
					0,
					tempBuffer,
					*realSize,
					realSize,
					NULL);
				errorCode = GetLastError();
			}
			if (ret == false)
			{
				//errorCode not 234 
				break;
			}
		}
		*processBuffer = tempBuffer;
	} while (false);

	if (hFile)
		CloseHandle(hFile);

	return errorCode;

}
void Wchar_tToString(std::string& szDst, wchar_t* wchar, int len)
{
	len /= 2;
	char* char_str = new char[len + 1];
	memset(char_str, 0x0, len + 1);
	wcstombs(char_str, wchar, len);
	szDst = char_str;
	delete[] char_str;
}
int InsertLink(std::list<ProcessInfo>& listHeader, _In_ PVOID processBuffer, ULONG realSize)
{
	PUNICODE_STRING tempUni = NULL;
	DWORD errorCode = 0;
	wchar_t* tempWchar = NULL;
	std::string tempString{ 0 };
	ULONG moveStep = 0;
	ULONG tempHappen = 0;
	ULONG pid = 0;



	while (moveStep < realSize)
	{
		tempHappen = (ULONG)processBuffer + moveStep;
		pid = *((ULONG*)tempHappen);
		tempUni = (PUNICODE_STRING)(tempHappen + sizeof(ULONG));
		tempUni->Buffer = (PWCH)((ULONG)tempUni + sizeof(UNICODE_STRING));
		moveStep += tempUni->MaximumLength + sizeof(UNICODE_STRING) + sizeof(ULONG);
		Wchar_tToString(tempString, tempUni->Buffer, tempUni->Length);
		ProcessInfo tempProInfo(pid, tempString);
		listHeader.push_back(tempProInfo);
	}


	return errorCode;
}
int GetRunningProcessInfosFromR0(std::list<ProcessInfo>& listHeader)
{
	listHeader.clear();
	int errorCode = 0;
	PVOID processBuffer = 0;
	ULONG realSize = 0;
	do
	{
		errorCode = GetProcessPathBuffer(&processBuffer, &realSize);
		if (errorCode != 0)
			break;
		//Create ProcessInfo and add to listHeader
		errorCode = InsertLink(listHeader, processBuffer, realSize);
		if (errorCode != 0)
			break;
	} while (false);

	if (processBuffer)
		free(processBuffer);
	return errorCode;
}