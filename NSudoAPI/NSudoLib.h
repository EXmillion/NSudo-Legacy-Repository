//NSudo 3.2
//NSudo Team. All rights reserved.

#pragma once

#pragma comment(lib,"NSudoLib.lib")

//��ȡһ�����̵�PID
DWORD NSudoGetProcessID(LPCWSTR lpProcessName, bool bUnderCurrentSessionID);

//�������ƴ������̣���CreateProcess��CreateEnvironmentBlock�ķ�װ��������ҪSE_ASSIGNPRIMARYTOKEN_NAME��Ȩ��
bool NSudoCreateProcess(HANDLE hToken, LPCWSTR lpCommandLine);

//һ��������ر�������Ȩ
void NSudoAdjustAllTokenPrivileges(HANDLE TokenHandle, bool bEnable);

//ģ�⵱ǰ���̵�Ȩ������ΪSystemȨ��(�����ȡ��ģ�⣬���Ե���RevertToSelf)
bool NSudoImpersonateSystemToken();

//��ȡSystemȨ������(��ҪSE_DEBUG_NAME��Ȩ)
bool NSudoGetSystemToken(PHANDLE hNewToken);

//��ȡTrustedInstallerȨ������(��ҪSE_DEBUG_NAME��Ȩ)
bool NSudoGetTrustedInstallerToken(PHANDLE hNewToken);

//��������Ȩ��
bool SetTokenPrivilege(HANDLE TokenHandle, LPCWSTR lpName, bool bEnable);

//���õ�ǰ��������Ȩ��
bool SetCurrentProcessPrivilege(LPCWSTR lpName, bool bEnable);

//��ȡ��ǰ�û�������(��ҪSE_DEBUG_NAME��Ȩ)
bool NSudoGetCurrentUserToken(PHANDLE hNewToken);

//��������������
bool SetTokenIntegrity(HANDLE hToken, LPCWSTR szIntegritySID);

//��ȡ��ǰ���̵�����
bool NSudoGetCurrentProcessToken(PHANDLE hNewToken);

//�Ե�ǰ���̽��н�Ȩ����ȡ����
bool NSudoCreateLUAToken(PHANDLE hNewToken);