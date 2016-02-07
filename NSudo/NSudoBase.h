//NSudo ���Ŀ�

#pragma once

enum TokenPrivilegesList
{
	SeMinWellKnownPrivilege = 2,
	SeCreateTokenPrivilege = 2,
	SeAssignPrimaryTokenPrivilege,
	SeLockMemoryPrivilege,
	SeIncreaseQuotaPrivilege,
	SeMachineAccountPrivilege,
	SeTcbPrivilege,
	SeSecurityPrivilege,
	SeTakeOwnershipPrivilege,
	SeLoadDriverPrivilege,
	SeSystemProfilePrivilege,
	SeSystemtimePrivilege,
	SeProfileSingleProcessPrivilege,
	SeIncreaseBasePriorityPrivilege,
	SeCreatePagefilePrivilege,
	SeCreatePermanentPrivilege,
	SeBackupPrivilege,
	SeRestorePrivilege,
	SeShutdownPrivilege,
	SeDebugPrivilege,
	SeAuditPrivilege,
	SeSystemEnvironmentPrivilege,
	SeChangeNotifyPrivilege,
	SeRemoteShutdownPrivilege,
	SeUndockPrivilege,
	SeSyncAgentPrivilege,
	SeEnableDelegationPrivilege,
	SeManageVolumePrivilege,
	SeImpersonatePrivilege,
	SeCreateGlobalPrivilege,
	SeTrustedCredManAccessPrivilege,
	SeRelabelPrivilege,
	SeIncreaseWorkingSetPrivilege,
	SeTimeZonePrivilege,
	SeCreateSymbolicLinkPrivilege,
	SeMaxWellKnownPrivilege = SeCreateSymbolicLinkPrivilege
};

// Integrity Level
enum IntegrityLevel
{
	Untrusted = SECURITY_MANDATORY_UNTRUSTED_RID, // S-1-16-0
	Low = SECURITY_MANDATORY_LOW_RID, // S-1-16-4096
	Medium = SECURITY_MANDATORY_MEDIUM_RID, // S-1-16-8192
	MediumPlus = SECURITY_MANDATORY_MEDIUM_PLUS_RID, // S-1-16-8448
	High = SECURITY_MANDATORY_HIGH_RID, // S-1-16-12288
	System = SECURITY_MANDATORY_SYSTEM_RID, // S-1-16-16384
	Protected = SECURITY_MANDATORY_PROTECTED_PROCESS_RID // S-1-16-20480
};

//��ȡ������Ϣ��С
#define SuTokenGetInfoSize(Handle, TokenInfoClass, ReturnLength) \
	NtQueryInformationToken(Handle, TokenInfoClass, NULL, NULL, ReturnLength)

//��ȡ��������
#define SuGetPsToken(ProcessHandle, TokenHandle) \
	NtOpenProcessToken(ProcessHandle, MAXIMUM_ALLOWED, TokenHandle)

//��ȡ��ǰ��������
#define SuGetCurrentPsToken(TokenHandle) \
	SuGetPsToken(NtCurrentProcess(), TokenHandle)

//��ȡ���̾��
NTSTATUS SuOpenProcess(
	_In_ DWORD dwProcessId,
	_Out_ PHANDLE ProcessHandle);

//�������Ƹ���
NTSTATUS WINAPI SuDuplicateToken(
	_In_ HANDLE ExistingToken,
	_Out_ PHANDLE NewTokenHandle);

//��ȡ������Ϣ(�Զ����������ڴ�)
NTSTATUS WINAPI SuGetTokenInfo(
	_In_ HANDLE TokenHandle,
	_In_ TOKEN_INFORMATION_CLASS TokenInformationClass,
	_Out_ void*& TokenInformation,
	_Out_ PULONG ReturnLength);

//��������������Ȩ
NTSTATUS WINAPI SuEnableAllTokenPrivileges(
	_In_ HANDLE TokenHandle);

//�Ƴ��󲿷�������Ȩ
#define SuRemoveMaximumTokenPrivileges(ExistingTokenHandle, NewTokenHandle) \
	NtFilterToken(ExistingTokenHandle, \
		DISABLE_MAX_PRIVILEGE, NULL, NULL, NULL, NewTokenHandle)

//��ȡһ�����̵�PID
DWORD WINAPI SuGetProcessID(
	_In_ LPCWSTR lpProcessName,
	_In_ bool bUnderCurrentSessionID);

//����һ�����񲢷��ط������ID
DWORD WINAPI SuStartService(
	_In_ LPCWSTR lpServiceName);

//��ȡSystemȨ������(��ҪSE_DEBUG_NAME��Ȩ)
bool SuGetSystemToken(
	_In_ PHANDLE hNewToken);

//ģ�⵱ǰ���̵�Ȩ������ΪSystemȨ��(�����ȡ��ģ�⣬���Ե���RevertToSelf)
bool SuImpersonateSystemToken();

//��ȡTrustedInstaller����
bool SuGetTrustedInstallerToken(
	_In_ PHANDLE hNewToken);

//��ȡ��ǰ�û�������(��ҪSE_DEBUG_NAME��Ȩ)
bool SuGetCurrentUserToken(
	_In_ PHANDLE hNewToken);

//��������������
NTSTATUS WINAPI SuSetTokenIL(
	_In_ HANDLE TokenHandle,
	_In_ DWORD IntegrityLabelRID);

//����һ����Ӧ�Ľ�Ȩ����
NTSTATUS WINAPI SuCreateLUAToken(
	_In_ HANDLE ExistingTokenHandle,
	_Out_ PHANDLE NewTokenHandle);

//Ϊ����������Ȩ
bool SuSetTokenPrivilege(
	_In_ HANDLE TokenHandle,
	_In_ TokenPrivilegesList Privilege,
	_In_ bool bEnable);

//���õ�ǰ��������Ȩ��
bool SuSetCurrentPsPrivilege(
	_In_ TokenPrivilegesList Privilege,
	_In_ bool bEnable);