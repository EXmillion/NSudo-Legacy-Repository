//NSudo 3.0 (Build 810)
//(C) 2015 NSudo Team. All rights reserved.

#pragma once

//#include <Windows.h>

extern "C"
{
	//��������Ȩ��
	bool SetTokenPrivilege(HANDLE TokenHandle, LPCWSTR lpName, bool bEnable);

	//���õ�ǰ��������Ȩ��
	bool SetCurrentProcessPrivilege(LPCWSTR lpName, bool bEnable);

	//һ������ȫ��Ȩ������
	void EnableAllTokenPrivileges(HANDLE TokenHandle);

	//��ȡSystemȨ������
	bool GetSystemToken(PHANDLE hNewToken);

	//��ȡ��ǰ�ỰID��winlogon��PID
	DWORD GetWinLogonProcessID();

	//��ȡTrustedInstaller��PID
	DWORD GetTrustedInstallerProcessID();
}