//NSudo 3.0 (Build 825)
//(C) 2015 NSudo Team. All rights reserved.

#include "stdafx.h"
#include "NSudo.h"

#define NSudo_Title L"NSudo"
#define NSudo_Version L"3.0 Internal Alpha 1(Build 8)"
#define NSudo_CopyRight L"\xA9 2015 NSudo Team. All rights reserved."

#define NSudo_Text_Default L"Ĭ��"

#define NSudo_Text_TI L"TrustedInstaller"
#define NSudo_Text_Sys L"System"
#define NSudo_Text_CU L"��ǰ�û�(δ��Ȩ����)"
#define NSudo_Text_CUP L"��ǰ�û�"

#define NSudo_Text_EnableAll L"����������Ȩ"
#define NSudo_Text_DisableAll L"����������Ȩ"

#define NSudo_Text_Low L"��"
#define NSudo_Text_Medium L"��"
#define NSudo_Text_High L"��"
#define NSudo_Text_System L"ϵͳ"

#include "..\\NSudoAPI\\NSudoAPI.h"
#pragma comment(lib,"NSudoAPI.lib")

#define ReturnMessage(lpText) MessageBoxW(NULL, (lpText), L"NSudo 3.0 M1", NULL)

#include "NSudo_Run.h"

//#include <set>

//using namespace std;

INT_PTR CALLBACK NSudoDlgCallBack(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void NSudoBrowseDialog(HWND hWnd, wchar_t* szPath)
{
	OPENFILENAME ofn = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrFile = szPath;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

	GetOpenFileNameW(&ofn);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	if (!(SetCurrentProcessPrivilege(SE_ASSIGNPRIMARYTOKEN_NAME, true)
		&& SetCurrentProcessPrivilege(SE_INCREASE_QUOTA_NAME, true)
		&& SetCurrentProcessPrivilege(SE_DEBUG_NAME, true)))
	{
		ReturnMessage(L"���̵���Ȩ�޻�ȡʧ��");
		return -1;
	}

	DialogBoxParamW(hInstance, MAKEINTRESOURCEW(IDD_NSudoDlg), NULL, NSudoDlgCallBack, 0L);

	return 0;
}

INT_PTR CALLBACK NSudoDlgCallBack(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hUserName = GetDlgItem(hDlg, IDC_UserName);
	HWND hTokenPrivilege = GetDlgItem(hDlg, IDC_TokenPrivilege);
	HWND hMandatoryLabel = GetDlgItem(hDlg, IDC_MandatoryLabel);
	HWND hszPath = GetDlgItem(hDlg, IDC_szPath);

	wchar_t szCMDLine[260], szUser[260], szPrivilege[260], szMandatory[260], szBuffer[260];

	switch (message)
	{
	case WM_INITDIALOG:
		
		// Show NSudo Logo
		SendMessageW(
			GetDlgItem(hDlg, IDC_NSudoLogo),
			STM_SETIMAGE,
			IMAGE_ICON,
			LPARAM(LoadImageW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_NSUDO), IMAGE_ICON, 0, 0, LR_COPYFROMRESOURCE)));

		//Show Warning Icon
		SendMessageW(
			GetDlgItem(hDlg, IDC_Icon),
			STM_SETIMAGE,
			IMAGE_ICON,
			LPARAM(LoadIconW(NULL, IDI_WARNING)));

		SendMessageW(hUserName, CB_INSERTSTRING, 0, (LPARAM)NSudo_Text_TI);
		SendMessageW(hUserName, CB_INSERTSTRING, 0, (LPARAM)NSudo_Text_Sys);
		SendMessageW(hUserName, CB_INSERTSTRING, 0, (LPARAM)NSudo_Text_CUP);
		SendMessageW(hUserName, CB_INSERTSTRING, 0, (LPARAM)NSudo_Text_CU);
		SendMessageW(hUserName, CB_SETCURSEL, 3, 0); //����Ĭ����"TrustedInstaller"
		
		SendMessageW(hTokenPrivilege, CB_INSERTSTRING, 0, (LPARAM)NSudo_Text_Default);
		SendMessageW(hTokenPrivilege, CB_INSERTSTRING, 0, (LPARAM)NSudo_Text_EnableAll);
		SendMessageW(hTokenPrivilege, CB_INSERTSTRING, 0, (LPARAM)NSudo_Text_DisableAll);
		SendMessageW(hTokenPrivilege, CB_SETCURSEL, 2, 0); //����Ĭ����"Ĭ��"

		SendMessageW(hMandatoryLabel, CB_INSERTSTRING, 0, (LPARAM)NSudo_Text_Low);
		SendMessageW(hMandatoryLabel, CB_INSERTSTRING, 0, (LPARAM)NSudo_Text_Medium);
		SendMessageW(hMandatoryLabel, CB_INSERTSTRING, 0, (LPARAM)NSudo_Text_High);
		SendMessageW(hMandatoryLabel, CB_INSERTSTRING, 0, (LPARAM)NSudo_Text_System);
		SendMessageW(hMandatoryLabel, CB_INSERTSTRING, 0, (LPARAM)NSudo_Text_Default);
		SendMessageW(hMandatoryLabel, CB_SETCURSEL, 0, 0); //����Ĭ����"Ĭ��"

		SendMessageW(hszPath, CB_INSERTSTRING, 0, (LPARAM)L"������ʾ��");
		SendMessageW(hszPath, CB_INSERTSTRING, 0, (LPARAM)L"PowerShell");
		SendMessageW(hszPath, CB_INSERTSTRING, 0, (LPARAM)L"PowerShell ISE");

		return (INT_PTR)TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_Run:
			GetDlgItemTextW(hDlg, IDC_UserName, szUser, sizeof(szUser));
			GetDlgItemTextW(hDlg, IDC_TokenPrivilege, szPrivilege, sizeof(szPrivilege));
			GetDlgItemTextW(hDlg, IDC_MandatoryLabel, szMandatory, sizeof(szMandatory));
			GetDlgItemTextW(hDlg, IDC_szPath, szCMDLine, sizeof(szCMDLine));

			NSudo_Run(hDlg,szUser, szPrivilege, szMandatory, szCMDLine);
			break;
		case IDC_About:
			ReturnMessage(
				L"NSudo 3.0 M1 (Build 825)\n"
				L"\xA9 2015 NSudo Team. All rights reserved.\n\n"
				L"��лcjy__05,mhxkx,tangmigoId,wondersnefu,xy137425740,�¹��Ĵ���֧�֣�����Ӣ����ĸ��ƴ������ĸ����");
			break;
		case IDC_Browse:
			wcscpy_s(szBuffer, 260, L"");
			NSudoBrowseDialog(hDlg, szBuffer);
			SetDlgItemTextW(hDlg, IDC_szPath, szBuffer);
			break;
		}
		break;
	case WM_SYSCOMMAND:
		switch (LOWORD(wParam))
		{
		case SC_CLOSE:
			PostQuitMessage(0);
			break;
		}
		break;
	}
	return 0;
}