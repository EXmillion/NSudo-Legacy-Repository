//NSudo 3.0 (Build 825)
//(C) 2015 NSudo Team. All rights reserved.

#pragma once

extern "C"
{
	//��������Ȩ��
	bool SetTokenPrivilege(HANDLE TokenHandle, LPCWSTR lpName, bool bEnable);

	//���õ�ǰ��������Ȩ��
	bool SetCurrentProcessPrivilege(LPCWSTR lpName, bool bEnable);

	//һ������ȫ��Ȩ������
	void EnableAllTokenPrivileges(HANDLE TokenHandle);

	//��ȡSystemȨ������(��ҪDebug��Ȩ)
	bool GetSystemToken(PHANDLE hNewToken);

	//��ȡ��ǰ�ỰID��winlogon��PID
	DWORD GetWinLogonProcessID();

	//��ȡTrustedInstaller��PID
	DWORD GetTrustedInstallerProcessID();

	//�ж��Ƿ���WoW64������
	bool IsWoW64();

	//��ȡ��ǰ�û�����(��ҪDebug��Ȩ)
	bool GetCurrentUserToken(PHANDLE hNewToken);

	//��ȡTrustedInstallerȨ������(��ҪDebug��Ȩ)
	bool GetTIToken(PHANDLE hNewToken);

	//һ������ȫ��Ȩ������
	void DisableAllTokenPrivileges(HANDLE TokenHandle);

	//��������������
	bool SetTokenIntegrity(HANDLE hToken, LPCWSTR szIntegritySID);

	//������ָ������һ���Ļ�����
	bool CreateEnvironmentBlockFromProcess(DWORD dwPID, LPVOID* lpEnv);
}