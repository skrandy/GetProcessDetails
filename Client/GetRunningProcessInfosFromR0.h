#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<list>
#include<string>
#include<Windows.h>
#include<SubAuth.h>
struct ProcessInfo
{
	ProcessInfo(ULONG pid, std::string tempstr) :_processPath(tempstr), _pid(pid) {}
	ULONG _pid;
	std::string	_processPath;
};

/*

	RETURN Value:
		0		:		OK
		else	:		errorCode
*/
int GetRunningProcessInfosFromR0(std::list<ProcessInfo>& listHeader);
