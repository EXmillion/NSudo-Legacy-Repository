//NSudo 3.0 (Build 810)
//(C) 2015 NSudo Project. All rights reserved.

#include "stdafx.h"
#include "NSudo.h"

#define NSudo_Title L"NSudo"
#define NSudo_Version L"3.0 (Build 810)"
#define NSudo_CopyRight L"\xA9 2015 NSudo Team. All rights reserved."

#define ReturnMessage(lpText) MessageBoxW(NULL, (lpText), NSudo_Title, NULL)
void About();
void GetSystemPrivilege(LPWSTR szCMDLine);
void GetTIToken(LPWSTR szCMDLine);

#include "..\\NSudo.Core\\NSudo.Core.h"
#pragma comment(lib,"..\\NSudo.Core\\Lib\\NSudo.Core.lib")

#include <set>

using namespace std;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	
	
	if (!SetCurrentProcessPrivilege(SE_DEBUG_NAME, true))
	{
		ReturnMessage(L"���̵���Ȩ�޻�ȡʧ��");
		return -1;
	}
	
	wchar_t szCMDPath[260], szCMDLineSystem[260], szCMDLineTI[260];

	GetSystemWindowsDirectoryW(szCMDPath, 260); //��ȡWindowsĿ¼

	if (GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "ZwWow64ReadVirtualMemory64")) //�ж��Ƿ���64λOS
	{
		wcscat_s(szCMDPath, 260, L"\\SysNative\\");  //64������ʾ��·��
	}
	else
	{
		wcscat_s(szCMDPath, 260, L"\\System32\\");  //32������ʾ��·��
	}

	wcscpy_s(szCMDLineSystem, 260, szCMDPath);
	wcscpy_s(szCMDLineTI, 260, szCMDPath);
	wcscat_s(szCMDLineSystem, 260, L"cmd.exe /K title " NSudo_Title L" - [System] & echo " NSudo_Title  L" " NSudo_Version L" & echo " NSudo_CopyRight);
	wcscat_s(szCMDLineTI, 260, L"cmd.exe /K title " NSudo_Title L" - [System With TrustedInstaller Token] & echo " NSudo_Title  L" " NSudo_Version L" & echo " NSudo_CopyRight);

	if (_wcsicmp(L"-TiShell", lpCmdLine) == 0)
	{	
		GetTIToken(szCMDLineTI);
		ExitProcess(0);
	}
	else if (_wcsicmp(L"-TI", lpCmdLine) == 0)
	{
		wchar_t szCMDLine[260];
		GetModuleFileNameW(NULL, szCMDLine, 260);
		wcscat_s(szCMDLine, 260, L" -TiShell");
		GetSystemPrivilege(szCMDLine);
		ExitProcess(0);
	}
	else if (_wcsicmp(L"-System", lpCmdLine) == 0)
	{
		GetSystemPrivilege(szCMDLineSystem);
		ExitProcess(0);
	}
	else if (_wcsicmp(L"-Help", lpCmdLine) == 0)
	{
		About();
		ExitProcess(0);
	}
	else
	{
		int nButtonPressed = 0, nRadioButton = 0;
		TASKDIALOGCONFIG config = { 0 };
		const TASKDIALOG_BUTTON buttons[] = {
			{ 101, L"����ѡȨ������������ʾ��(&C)" },
			{ 102, L"���ں������а���(&A)" },
		};
		const TASKDIALOG_BUTTON choosebuttons[] = {
			{ 201, L"SystemȨ��(����TrustedInstaller����)(&T)" },
			{ 202, L"��SystemȨ��(&S)" },
		};

		config.dwFlags = TDF_USE_HICON_MAIN | TDF_USE_HICON_FOOTER | TDF_EXPAND_FOOTER_AREA | TDF_ALLOW_DIALOG_CANCELLATION;
		config.cbSize = sizeof(config);
		config.pszWindowTitle = NSudo_Title L" " NSudo_Version;
		config.cRadioButtons = ARRAYSIZE(choosebuttons);
		config.pRadioButtons = choosebuttons;
		config.pszMainInstruction = L"��ѡ������Ҫ��Ȩ��";
		config.hInstance = hInstance;
		config.cButtons = ARRAYSIZE(buttons);
		config.pButtons = buttons;
		config.hMainIcon = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_NSUDO));
		LoadIconWithScaleDown(NULL, IDI_WARNING,GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),&config.hFooterIcon);
		config.pszFooter = L"���棺ʹ�ñ�������Ҫ��һ���ļ��������";

		TaskDialogIndirect(&config, &nButtonPressed, &nRadioButton, NULL);

		switch (nButtonPressed)
		{
		case 101:
			if (nRadioButton == 201)
			{
				wchar_t szCMDLine[260];
				GetModuleFileNameW(NULL, szCMDLine, 260);
				wcscat_s(szCMDLine, 260, L" -TiShell");
				GetSystemPrivilege(szCMDLine);
			}
			else
			{
				GetSystemPrivilege(szCMDLineSystem);
			}
			ExitProcess(0);
		case 102:
			About();
			_tWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
		default:
			break;
		}
	}
	
	return 0;
}




void About()
{
	ReturnMessage(NSudo_Title  L" " NSudo_Version L"\n" NSudo_CopyRight L"\n\n"
		L"NSudo [ѡ��]\n\nѡ��:\n-TI ��SystemȨ��(����TrustedInstaller����)����������ʾ��\n-System ��SystemȨ������������ʾ��\n-Help ���������а���");
}

void GetSystemPrivilege(LPWSTR szCMDLine)
{
	DWORD dwUserSessionId; //�û��ỰID
	DWORD dwWinLogonPID = -1; //winlogon.exe��ProcessID

	if ((dwUserSessionId = WTSGetActiveConsoleSessionId()) == 0xFFFFFFFF) //����û��ỰID
	{
		ReturnMessage( L"����û��ỰIDʧ��");
		return;
	}

	PROCESSENTRY32W ProcessEntry;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32W);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //����CreateToolhelp32Snapshot�������н���
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		ReturnMessage(L"CreateToolhelp32Snapshot����ʧ��");
		return;
	}

	if (Process32FirstW(hSnapshot, &ProcessEntry)) //��������
	{
		do
		{
			if (wcscmp(L"winlogon.exe", ProcessEntry.szExeFile) == 0) //Ѱ��winlogon����
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

	if (dwWinLogonPID == -1)
	{
		ReturnMessage(L"winlogon.exe����PID��ȡʧ��");
		return;
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwWinLogonPID);
	if (hProc != NULL)
	{
		HANDLE hToken, hDupToken;
		if (OpenProcessToken(hProc, TOKEN_DUPLICATE | TOKEN_QUERY, &hToken))
		{
			if (DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, &hDupToken))
			{
				LPVOID lpEnv; //������
				if (CreateEnvironmentBlock(&lpEnv, hToken, 1))
				{
					EnableAllTokenPrivileges(hDupToken);
					
					STARTUPINFOW StartupInfo = { 0 };
					PROCESS_INFORMATION ProcessInfo = { 0 };
					StartupInfo.lpDesktop = L"WinSta0\\Default";
					if (!CreateProcessWithTokenW(
						hDupToken,
						LOGON_WITH_PROFILE,
						NULL,
						szCMDLine,
						CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
						lpEnv,
						NULL,
						&StartupInfo,
						&ProcessInfo))
					{
						if (!CreateProcessAsUserW(hDupToken,
							NULL,
							szCMDLine,
							NULL,
							NULL,
							NULL,
							CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
							lpEnv,
							NULL,
							&StartupInfo,
							&ProcessInfo))
						{
							ReturnMessage(L"���̴���ʧ��");
						}
					}
					DestroyEnvironmentBlock(lpEnv);
				}
				else ReturnMessage(L"winlogon.exe���̻����鴴��ʧ��");
				CloseHandle(hDupToken);
			}
			else ReturnMessage(L"winlogon.exe���̾�����Ƹ���ʧ��");
			CloseHandle(hToken);
		}
		else ReturnMessage(L"winlogon.exe���̾�����ƴ�ʧ��");
		CloseHandle(hProc);
	}
	else ReturnMessage(L"winlogon.exe���̾����ʧ��");
}

void GetTIToken(LPWSTR szCMDLine)
{
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
					if (StartServiceW(hSvc, NULL, NULL) == FALSE)
					{
						ReturnMessage(L"TrustedInstaller��������ʧ��");
					}
					// �ȴ���������
					while (::QueryServiceStatus(hSvc, &status) == TRUE)
					{
						Sleep(status.dwWaitHint);
						if (status.dwCurrentState == SERVICE_RUNNING)
						{
							break;
						}
					}
				}
			}
			CloseServiceHandle(hSvc);
		}
		CloseServiceHandle(hSC);
	}

	DWORD dwTIPID = -1; //winlogon.exe��ProcessID

	PROCESSENTRY32W ProcessEntry;
	ProcessEntry.dwSize = sizeof(PROCESSENTRY32W);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //����CreateToolhelp32Snapshot�������н���
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		ReturnMessage(L"CreateToolhelp32Snapshot����ʧ��");
		return;
	}

	if (Process32FirstW(hSnapshot, &ProcessEntry)) //��������
	{
		do
		{
			if (wcscmp(L"TrustedInstaller.exe", ProcessEntry.szExeFile) == 0) //Ѱ��winlogon����
			{
				dwTIPID = ProcessEntry.th32ProcessID;
				break;
			}
		} while (Process32NextW(hSnapshot, &ProcessEntry));
	}
	CloseHandle(hSnapshot);

	if (dwTIPID == -1)
	{
		ReturnMessage(L"TrustedInstaller.exe����PID��ȡʧ��");
		return;
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwTIPID);
	if (hProc != NULL)
	{
		HANDLE hToken, hDupToken;
		if (OpenProcessToken(hProc, TOKEN_DUPLICATE | TOKEN_QUERY, &hToken))
		{
			if (DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, &hDupToken))
			{
				LPVOID lpEnv; //������
				if (CreateEnvironmentBlock(&lpEnv, hToken, 1))
				{
					EnableAllTokenPrivileges(hDupToken);
					
					STARTUPINFOW StartupInfo = { 0 };
					PROCESS_INFORMATION ProcessInfo = { 0 };
					StartupInfo.lpDesktop = L"WinSta0\\Default";
					if (!CreateProcessWithTokenW(
						hDupToken,
						LOGON_WITH_PROFILE,
						NULL,
						szCMDLine,
						CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
						lpEnv,
						NULL,
						&StartupInfo,
						&ProcessInfo))
					{
						if (!CreateProcessAsUserW(hDupToken,
							NULL,
							szCMDLine,
							NULL,
							NULL,
							NULL,
							CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT,
							lpEnv,
							NULL,
							&StartupInfo,
							&ProcessInfo))
						{
							ReturnMessage(L"���̴���ʧ��");
						}
					}
					DestroyEnvironmentBlock(lpEnv);
				}
				else ReturnMessage(L"TrustedInstaller.exe���̻����鴴��ʧ��");
				CloseHandle(hDupToken);
			}
			else ReturnMessage(L"TrustedInstaller.exe���̾�����Ƹ���ʧ��");
			CloseHandle(hToken);
		}
		else ReturnMessage(L"TrustedInstaller.exe���̾�����ƴ�ʧ��");
		CloseHandle(hProc);
	}
	else ReturnMessage(L"TrustedInstaller.exe���̾����ʧ��");
}

