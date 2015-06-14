//NSudo 3.0 (Build 810)
//(C) 2015 NSudo Project. All rights reserved.

#include "stdafx.h"

#include "NSudo.Core.h"

extern "C"
{

	//��������Ȩ��
	bool SetTokenPrivilege(HANDLE TokenHandle, LPCWSTR lpName, bool bEnable)
	{
		TOKEN_PRIVILEGES TP;

		TP.PrivilegeCount = 1;
		TP.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : NULL;

		if (LookupPrivilegeValueW(NULL, lpName, &TP.Privileges[0].Luid))
		{
			if (AdjustTokenPrivileges(TokenHandle, FALSE, &TP, sizeof(TP), NULL, NULL)) return true;
		}

		return false;
	}

	//���õ�ǰ��������Ȩ��
	bool SetCurrentProcessPrivilege(LPCWSTR lpName, bool bEnable)
	{
		bool bRet = false;
		HANDLE hCurrentProcessToken;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hCurrentProcessToken))
		{
			if (SetTokenPrivilege(hCurrentProcessToken, lpName, true)) bRet = true;
			CloseHandle(hCurrentProcessToken);
		}
		return bRet;
	}

	//һ������ȫ��Ȩ������
	void EnableAllTokenPrivileges(HANDLE TokenHandle)
	{
		wchar_t *PrivilegeList[] = {
			SE_CREATE_TOKEN_NAME,
			SE_ASSIGNPRIMARYTOKEN_NAME,
			SE_LOCK_MEMORY_NAME,
			SE_INCREASE_QUOTA_NAME,
			SE_UNSOLICITED_INPUT_NAME,
			SE_MACHINE_ACCOUNT_NAME,
			SE_TCB_NAME,
			SE_SECURITY_NAME,
			SE_TAKE_OWNERSHIP_NAME,
			SE_LOAD_DRIVER_NAME,
			SE_SYSTEM_PROFILE_NAME,
			SE_SYSTEMTIME_NAME,
			SE_PROF_SINGLE_PROCESS_NAME,
			SE_INC_BASE_PRIORITY_NAME,
			SE_CREATE_PAGEFILE_NAME,
			SE_CREATE_PERMANENT_NAME,
			SE_BACKUP_NAME,
			SE_RESTORE_NAME,
			SE_SHUTDOWN_NAME,
			SE_DEBUG_NAME,
			SE_AUDIT_NAME,
			SE_SYSTEM_ENVIRONMENT_NAME,
			SE_CHANGE_NOTIFY_NAME,
			SE_REMOTE_SHUTDOWN_NAME,
			SE_UNDOCK_NAME,
			SE_SYNC_AGENT_NAME,
			SE_ENABLE_DELEGATION_NAME,
			SE_MANAGE_VOLUME_NAME,
			SE_IMPERSONATE_NAME,
			SE_CREATE_GLOBAL_NAME,
			SE_TRUSTED_CREDMAN_ACCESS_NAME,
			SE_RELABEL_NAME,
			SE_INC_WORKING_SET_NAME,
			SE_TIME_ZONE_NAME,
			SE_CREATE_SYMBOLIC_LINK_NAME };

		for (int i = 0; i < sizeof(PrivilegeList) / sizeof(wchar_t*); i++)
		{
			SetTokenPrivilege(TokenHandle, PrivilegeList[i], true);
		}
	}

	//��ȡ��ǰ�ỰID��winlogon��PID
	DWORD GetWinLogonProcessID()
	{
		DWORD dwWinLogonPID = -1; //winlogon.exe��ProcessID
		
		//��ȡ��ǰ�ỰID
		DWORD dwUserSessionId = WTSGetActiveConsoleSessionId();
		if (dwUserSessionId != 0xFFFFFFFF)
		{
			//��ȡ��ǰ�ỰID�µ�winlogon��PID
			PROCESSENTRY32W ProcessEntry;
			ProcessEntry.dwSize = sizeof(PROCESSENTRY32W);
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //����CreateToolhelp32Snapshot�������н���
			if (hSnapshot != INVALID_HANDLE_VALUE)
			{
				if (Process32FirstW(hSnapshot, &ProcessEntry)) //��������
				{
					do
					{
						if (_wcsicmp(L"winlogon.exe", ProcessEntry.szExeFile) == 0) //Ѱ��winlogon����
						{
							DWORD dwSessionID;
							if (ProcessIdToSessionId(ProcessEntry.th32ProcessID, &dwSessionID)) //��ȡwinlogon�ĻỰID
							{
								if (dwSessionID != dwUserSessionId) continue; //�ж��Ƿ��ǵ�ǰ�û�ID
								dwWinLogonPID = ProcessEntry.th32ProcessID;
								break;
							}
						}
					} while (Process32NextW(hSnapshot, &ProcessEntry));
				}
				CloseHandle(hSnapshot);
			}
		}
		return dwWinLogonPID;
	}

	//��ȡTrustedInstaller��PID
	DWORD GetTrustedInstallerProcessID()
	{
		DWORD dwTIPID = -1; //TrustedInstaller.exe��ProcessID

		SC_HANDLE hSC = OpenSCManagerW(NULL, NULL, GENERIC_EXECUTE);
		if (hSC != NULL)
		{
			SC_HANDLE hSvc = OpenServiceW(hSC, L"TrustedInstaller", SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP);
			if (hSvc != NULL)
			{
				SERVICE_STATUS status;
				if (QueryServiceStatus(hSvc, &status))
				{
					if (status.dwCurrentState == SERVICE_STOPPED)
					{
						// ��������
						if (StartServiceW(hSvc, NULL, NULL))
						{
							// �ȴ���������
							while (::QueryServiceStatus(hSvc, &status) == TRUE)
							{
								Sleep(status.dwWaitHint);
								if (status.dwCurrentState == SERVICE_RUNNING) break;
							}
						}
					}
				}
				CloseServiceHandle(hSvc);
			}
			CloseServiceHandle(hSC);
		}

		PROCESSENTRY32W ProcessEntry;
		ProcessEntry.dwSize = sizeof(PROCESSENTRY32W);
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //����CreateToolhelp32Snapshot�������н���
		if (hSnapshot != INVALID_HANDLE_VALUE)
		{
			if (Process32FirstW(hSnapshot, &ProcessEntry)) //��������
			{
				do
				{
					if (_wcsicmp(L"TrustedInstaller.exe", ProcessEntry.szExeFile) == 0) //Ѱ��TrustedInstaller����
					{
						dwTIPID = ProcessEntry.th32ProcessID;
						break;
					}
				} while (Process32NextW(hSnapshot, &ProcessEntry));
			}
			CloseHandle(hSnapshot);
		}
		return dwTIPID;
	}

	//��ȡSystemȨ������
	bool GetSystemToken(PHANDLE hNewToken)
	{
		bool bRet = false;
		HANDLE hCurrentProcessToken;
		if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hCurrentProcessToken))
		{
			//��ȡ��ǰ�ỰID�µ�winlogon��PID
			DWORD dwWinLogonPID = GetWinLogonProcessID();
			if (dwWinLogonPID != -1)
			{
				HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwWinLogonPID);
				if (hProc != NULL)
				{
					HANDLE hToken;
					if (OpenProcessToken(hProc, TOKEN_DUPLICATE, &hToken))
					{
						if (DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, hNewToken))
						{
							bRet = true;
						}
						else wprintf(L"NSudo.Core.dll!GetWinLogonProcessID!DuplicateTokenEx Failed");
						CloseHandle(hToken);
					}
					else wprintf(L"NSudo.Core.dll!GetWinLogonProcessID!OpenProcessToken Failed");
					CloseHandle(hProc);
				}
				else wprintf(L"NSudo.Core.dll!GetWinLogonProcessID!OpenProcess Failed");
			}
			else wprintf(L"NSudo.Core.dll!GetWinLogonProcessID Failed");
			CloseHandle(hCurrentProcessToken);
		}
		return bRet;
	}

}