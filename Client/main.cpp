#include"GetRunningProcessInfosFromR0.h"
#include<iostream>
using namespace std;






DWORD WINAPI ShowWindowsProcessPath(LPVOID lpParameter)
{
	
	list<ProcessInfo> MyList;
	int errorCode = 0;
	errorCode = GetRunningProcessInfosFromR0(MyList);
	do
	{
		if (errorCode != 0)
			break;
		for (list<ProcessInfo>::const_iterator begin = MyList.begin(); begin != MyList.end(); begin++)
		{
			cout << "PID:" << begin->_pid << "    " << "Process Path:" << begin->_processPath << endl;
		}
		return 0;
	} while (false);
	
	return errorCode;
}

int main()
{
	HANDLE hThread[10]{ 0 };
	while (true)
	{
		for (int i = 0; i < 10; i++)
		{
			hThread[i] = CreateThread(
				NULL,
				NULL,
				ShowWindowsProcessPath,
				0,
				0,
				NULL
			);
		}
		WaitForSingleObject(hThread[0], INFINITE);
		WaitForSingleObject(hThread[1], INFINITE);
		WaitForSingleObject(hThread[2], INFINITE);
		WaitForSingleObject(hThread[3], INFINITE);
		WaitForSingleObject(hThread[4], INFINITE);
		WaitForSingleObject(hThread[5], INFINITE);
		WaitForSingleObject(hThread[6], INFINITE);
		WaitForSingleObject(hThread[7], INFINITE);
		WaitForSingleObject(hThread[8], INFINITE);
		WaitForSingleObject(hThread[9], INFINITE);
		for (int i = 0; i < 10; i++)
		{
			CloseHandle(hThread[i]);
		}
	}
	return 0;
}