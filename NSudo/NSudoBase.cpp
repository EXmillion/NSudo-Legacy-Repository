//NSudo ���Ŀ�

#include "stdafx.h"

#include "NSudoBase.h"

//�������Ƹ���
NTSTATUS WINAPI SuDuplicateToken(
	_In_ HANDLE ExistingToken,
	_Out_ PHANDLE NewTokenHandle)
{
	SECURITY_QUALITY_OF_SERVICE SecurityQualityOfService =
	{
		sizeof(SECURITY_QUALITY_OF_SERVICE), // Length
		SecurityIdentification, // ImpersonationLevel
		FALSE, // ContextTrackingMode
		FALSE // EffectiveOnly
	};
	OBJECT_ATTRIBUTES ObjectAttributes =
	{
		sizeof(OBJECT_ATTRIBUTES), // Length
		NULL, // RootDirectory
		NULL, // ObjectName
		NULL, // Attributes
		NULL, // SecurityDescriptor
		&SecurityQualityOfService // SecurityQualityOfService
	};

	//�������ƶ���
	return NtDuplicateToken(
		ExistingToken,
		MAXIMUM_ALLOWED,
		&ObjectAttributes,
		FALSE,
		TokenPrimary,
		NewTokenHandle);
}

//��ȡ������Ϣ(�Զ����������ڴ�)
NTSTATUS WINAPI SuGetTokenInfo(
	_In_ HANDLE TokenHandle,
	_In_ TOKEN_INFORMATION_CLASS TokenInformationClass,
	_Out_ void*& TokenInformation,
	_Out_ PULONG ReturnLength)
{
	//��ȡ��С
	SuTokenGetInfoSize(TokenHandle, TokenInformationClass, ReturnLength);

	//�����ڴ�
	TokenInformation = MemAlloc(*ReturnLength);
	if (!TokenInformation) return STATUS_NO_MEMORY;

	//��ȡ��Ϣ
	return NtQueryInformationToken(
		TokenHandle, TokenInformationClass, TokenInformation,
		*ReturnLength, ReturnLength);
}

//��ȡ���̾��
NTSTATUS SuOpenProcess(
	_In_ DWORD dwProcessId,
	_Out_ PHANDLE ProcessHandle
	)
{
	OBJECT_ATTRIBUTES ObjAttr;
	CLIENT_ID ClientId =
	{
		(HANDLE)dwProcessId, // UniqueProcess;
		NULL // UniqueThread;
	};

	InitObjAttr(&ObjAttr, NULL, NULL, NULL, NULL, NULL);

	return NtOpenProcess(
		ProcessHandle, MAXIMUM_ALLOWED, &ObjAttr, &ClientId);
}

//��������������Ȩ
NTSTATUS WINAPI SuEnableAllTokenPrivileges(
	_In_ HANDLE TokenHandle)
{
	//�������
	NTSTATUS status = NULL;
	DWORD Length = NULL;
	PTOKEN_PRIVILEGES pTPs = NULL;

	//��ȡ��Ȩ��Ϣ
	status = SuGetTokenInfo(
		TokenHandle, TokenPrivileges, (void*&)pTPs, &Length);
	if (!NT_SUCCESS(status)) goto FuncEnd;

	//������Ȩ��Ϣ
	for (DWORD i = 0; i < pTPs->PrivilegeCount; i++)
		pTPs->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;

	//����ȫ����Ȩ
	status = NtAdjustPrivilegesToken(
		TokenHandle, FALSE, pTPs, NULL, NULL, NULL);

FuncEnd:
	MemFree(pTPs);

	return status;
}

//��ȡһ�����̵�PID
DWORD WINAPI SuGetProcessID(
	_In_ LPCWSTR lpProcessName,
	_In_ bool bUnderCurrentSessionID)
{
	//�������
	DWORD dwPID = -1;
	DWORD dwReturnLength = 0;
	PVOID lpBuffer = NULL;
	PSYSTEM_PROCESS_INFORMATION pSPI = NULL;

	//��ȡ��С
	NtQuerySystemInformation(
		SystemProcessInformation, NULL, NULL, &dwReturnLength);

	//�����ڴ�
	lpBuffer = MemAlloc(dwReturnLength);
	if (!lpBuffer) return STATUS_NO_MEMORY;
	pSPI = (PSYSTEM_PROCESS_INFORMATION)lpBuffer;

	//��ȡ������Ϣ
	if (!NT_SUCCESS(NtQuerySystemInformation(
		SystemProcessInformation, pSPI, dwReturnLength, &dwReturnLength)))
		goto FuncEnd;

	//��������
	while (true)
	{
		if (pSPI->ImageName.Buffer &&
			wcscmp(lpProcessName, pSPI->ImageName.Buffer) == 0)
		{
			if (!(bUnderCurrentSessionID
				&& pSPI->SessionId != USER_SHARED_DATA->ActiveConsoleId))
			{
				dwPID = HandleToUlong(pSPI->UniqueProcessId);
				break;
			}
		}

		if (pSPI->NextEntryOffset != 0)
			pSPI = (PSYSTEM_PROCESS_INFORMATION)((char *)pSPI + pSPI->NextEntryOffset);
	}

FuncEnd:
	MemFree(lpBuffer);
	return dwPID;
}

//����һ�����񲢷��ط������ID
DWORD WINAPI SuStartService(
	_In_ LPCWSTR lpServiceName)
{
	SERVICE_STATUS_PROCESS ssStatus;
	SC_HANDLE schSCManager, schService;
	//��SCM���������
	if ((schSCManager = OpenSCManagerW(NULL, NULL, GENERIC_EXECUTE)) != NULL)
	{
		//�򿪷�����
		if ((schService = OpenServiceW(
			schSCManager,
			lpServiceName,
			SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_STOP)) != NULL)
		{
			DWORD dwBytesNeeded;
			bool bStarted = false;
			//��ѯ״̬
			while (QueryServiceStatusEx(
				schService,
				SC_STATUS_PROCESS_INFO,
				(LPBYTE)&ssStatus,
				sizeof(SERVICE_STATUS_PROCESS),
				&dwBytesNeeded))
			{
				//���������ֹͣ״̬����û�е���StartServiceW
				if (ssStatus.dwCurrentState == SERVICE_STOPPED && !bStarted)
				{
					bStarted = true;
					if (StartServiceW(schService, 0, NULL)) continue;
				}
				//��������ڼ��غ�ж�ع����У�����Ҫ�ȴ�
				else if (ssStatus.dwCurrentState == SERVICE_START_PENDING
					|| ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
				{
					Sleep(ssStatus.dwWaitHint);
					continue;
				}
				//�����������break
				break;
			}
			CloseServiceHandle(schService); //�رշ�����
		}
		CloseServiceHandle(schSCManager); //�رշ�����������
	}
	//����������û��������У��򷵻ط����Ӧ����PID�����򷵻�"-1"
	if (ssStatus.dwCurrentState != SERVICE_STOPPED &&
		ssStatus.dwCurrentState != SERVICE_STOP_PENDING) 
		return ssStatus.dwProcessId;
	else return -1;
}

//��ȡSystemȨ������(��ҪSE_DEBUG_NAME��Ȩ)
bool SuGetSystemToken(
	_In_ PHANDLE hNewToken)
{
	bool bRet = false;

	//��ȡ��ǰ�ỰID�µ�winlogon��PID
	DWORD dwWinLogonPID = SuGetProcessID(L"winlogon.exe", true);
	if (dwWinLogonPID != -1)
	{
		HANDLE hProc;
		if (NT_SUCCESS(SuOpenProcess(dwWinLogonPID, &hProc)))
		{
			HANDLE hToken;
			if (NT_SUCCESS(SuGetPsToken(hProc, &hToken)))
			{
				bRet = NT_SUCCESS(SuDuplicateToken(hToken, hNewToken));
				NtClose(hToken);
			}
			NtClose(hProc);
		}
	}
	return bRet;
}

//ģ�⵱ǰ���̵�Ȩ������ΪSystemȨ��(�����ȡ��ģ�⣬���Ե���RevertToSelf)
bool SuImpersonateSystemToken()
{
	bool bRet = false;

	HANDLE hSystemToken;
	if (SuGetSystemToken(&hSystemToken)) //��ȡSystemȨ������
	{
		SuEnableAllTokenPrivileges(hSystemToken); //��ģ�������Ͽ���������Ȩ
		bRet = ImpersonateLoggedOnUser(hSystemToken); //ģ������
		NtClose(hSystemToken);
	}
	return bRet;
}

//��ȡTrustedInstaller����
bool SuGetTrustedInstallerToken(
	_In_ PHANDLE hNewToken)
{
	bool bRet = false;

	if (SuImpersonateSystemToken())
	{
		//����TrustedInstaller���񲢻�ȡSID
		DWORD dwTIPID = SuStartService(L"TrustedInstaller");

		if (dwTIPID != -1)
		{
			HANDLE hProc;
			if (NT_SUCCESS(SuOpenProcess(dwTIPID, &hProc)))
			{
				HANDLE hToken;
				if (NT_SUCCESS(SuGetPsToken(hProc, &hToken)))
				{
					bRet = NT_SUCCESS(SuDuplicateToken(hToken, hNewToken));
					NtClose(hToken);
				}
				NtClose(hProc);
			}
		}

		::RevertToSelf();
	}

	return bRet;
}

//��ȡ��ǰ�û�������(��ҪSE_DEBUG_NAME��Ȩ)
bool SuGetCurrentUserToken(
	_In_ PHANDLE hNewToken)
{
	bool bRet = false;

	if (SuImpersonateSystemToken())
	{
		WINSTATIONUSERTOKEN WSUT = { 0 };
		DWORD ccbInfo = NULL;
		if (WinStationQueryInformationW(
			SERVERNAME_CURRENT,
			LOGONID_CURRENT,
			WinStationUserToken,
			&WSUT,
			sizeof(WINSTATIONUSERTOKEN),
			&ccbInfo))
		{
			bRet = NT_SUCCESS(SuDuplicateToken(WSUT.UserToken, hNewToken));
		}
		RevertToSelf();
	}

	return bRet;
}

//��������������
NTSTATUS WINAPI SuSetTokenIL(
	_In_ HANDLE TokenHandle,
	_In_ DWORD IntegrityLabelRID)
{
	//�������
	NTSTATUS status = NULL;
	TOKEN_MANDATORY_LABEL TML = { 0 };
	SID_IDENTIFIER_AUTHORITY SIDAuthority = SECURITY_MANDATORY_LABEL_AUTHORITY;

	//��ʼ��SID
	status = RtlAllocateAndInitializeSid(
		&SIDAuthority, 1, IntegrityLabelRID,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, &TML.Label.Sid);
	if (!NT_SUCCESS(status)) goto FuncEnd;

	//��ʼ��TOKEN_MANDATORY_LABEL
	TML.Label.Attributes = SE_GROUP_INTEGRITY;

	//�������ƶ���
	status = NtSetInformationToken(
		TokenHandle, TokenIntegrityLevel, &TML, sizeof(TML));
	if (!NT_SUCCESS(status)) goto FuncEnd;

	//ɨβ
FuncEnd:
	if (TML.Label.Sid) RtlFreeSid(TML.Label.Sid);

	return status;
}

//����һ����Ӧ�Ľ�Ȩ����
NTSTATUS WINAPI SuCreateLUAToken(
	_In_ HANDLE ExistingTokenHandle,
	_Out_ PHANDLE NewTokenHandle)
{
	//�������
	NTSTATUS status = NULL;

	DWORD Length = NULL;
	PTOKEN_USER pTokenUser = NULL;
	PTOKEN_OWNER pTokenOwner = NULL;
	PTOKEN_DEFAULT_DACL pTokenDacl = NULL;
	SID_IDENTIFIER_AUTHORITY SIDAuthority = SECURITY_NT_AUTHORITY;
	PSID AdminSid = NULL;
	PACL pAcl = NULL;
	PACL pNewAcl = NULL;
	TOKEN_DEFAULT_DACL NewTokenDacl = { 0 };
	BOOL EnableTokenVirtualization = TRUE;
	PACCESS_ALLOWED_ACE pTempAce = NULL;

	//������������
	status = NtFilterToken(
		ExistingTokenHandle, LUA_TOKEN, NULL, NULL, NULL, NewTokenHandle);
	if (!NT_SUCCESS(status)) goto FuncEnd;

	//���´�����ʺϹ���ԱȨ�����ƣ���ǿ����Ҫ��
	//****************************************************************************

	//��������������
	SuSetTokenIL(*NewTokenHandle, SECURITY_MANDATORY_MEDIUM_RID);

	//��ȡ���ƶ�Ӧ���û��˻�SID
	if (!NT_SUCCESS(SuGetTokenInfo(
		*NewTokenHandle, TokenUser, (void*&)pTokenUser, &Length)))
		goto FuncEnd;

	//��������OwnerΪ��ǰ�û��������ڴ棩
	pTokenOwner = (PTOKEN_OWNER)MemAlloc(sizeof(TOKEN_OWNER));
	if (!pTokenOwner) goto FuncEnd;

	//��������OwnerΪ��ǰ�û������ò�����
	pTokenOwner->Owner = pTokenUser->User.Sid;

	//��������OwnerΪ��ǰ�û�
	if (!NT_SUCCESS(NtSetInformationToken(*NewTokenHandle,
		TokenOwner, pTokenOwner, sizeof(TOKEN_OWNER))))
		goto FuncEnd;

	//��ȡ���Ƶ�DACL
	if (!NT_SUCCESS(SuGetTokenInfo(
		*NewTokenHandle, TokenDefaultDacl, (void*&)pTokenDacl, &Length)))
		goto FuncEnd;
	pAcl = pTokenDacl->DefaultDacl;

	//��ȡ����Ա��SID
	if (!NT_SUCCESS(RtlAllocateAndInitializeSid(
		&SIDAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
		NULL, NULL, NULL, NULL, NULL, NULL, &AdminSid)))
		goto FuncEnd;

	//������ACL��С
	Length += RtlLengthSid(pTokenUser->User.Sid);
	Length += sizeof(ACCESS_ALLOWED_ACE);

	//����ACL�ṹ�ڴ�
	pNewAcl = (PACL)MemAlloc(Length);
	if (!pNewAcl) goto FuncEnd;

	//����ACL
	if (!NT_SUCCESS(RtlCreateAcl(pNewAcl, Length, pAcl->AclRevision)))
		goto FuncEnd;

	//���ACE
	if (!NT_SUCCESS(RtlAddAccessAllowedAce(
		pNewAcl, pAcl->AclRevision, GENERIC_ALL, pTokenUser->User.Sid)))
		goto FuncEnd;

	//����ACE
	for (int i = 0; NT_SUCCESS(RtlGetAce(pAcl, i, (PVOID*)&pTempAce)); i++)
	{
		if (RtlEqualSid(AdminSid, &pTempAce->SidStart)) continue;

		RtlAddAce(pNewAcl, pAcl->AclRevision, 0,
			pTempAce, pTempAce->Header.AceSize);
	}

	//��������DACL
	Length += sizeof(TOKEN_DEFAULT_DACL);
	NewTokenDacl.DefaultDacl = pNewAcl;
	if (!NT_SUCCESS(NtSetInformationToken(*NewTokenHandle,
		TokenDefaultDacl, &NewTokenDacl, Length)))
		goto FuncEnd;

	//��LUA���⻯
	if (!NT_SUCCESS(NtSetInformationToken(*NewTokenHandle,
		TokenVirtualizationEnabled, &EnableTokenVirtualization, sizeof(BOOL))))
		goto FuncEnd;

	//****************************************************************************

	//ɨβ
FuncEnd:
	MemFree(pNewAcl);
	MemFree(AdminSid);
	MemFree(pTokenDacl);
	MemFree(pTokenOwner);
	MemFree(pTokenUser);
	return status;
}

//��������������Ȩ
NTSTATUS WINAPI SuEnableTokenPrivilege(
	_In_ HANDLE TokenHandle)
{
	//�������
	NTSTATUS status = NULL;
	DWORD Length = NULL;
	PTOKEN_PRIVILEGES pTPs = NULL;

	//��ȡ��Ȩ��Ϣ
	status = SuGetTokenInfo(
		TokenHandle, TokenPrivileges, (void*&)pTPs, &Length);
	if (!NT_SUCCESS(status)) goto FuncEnd;

	//������Ȩ��Ϣ
	for (DWORD i = 0; i < pTPs->PrivilegeCount; i++)
		pTPs->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;

	//����ȫ����Ȩ
	status = NtAdjustPrivilegesToken(
		TokenHandle, FALSE, pTPs, NULL, NULL, NULL);

FuncEnd:
	MemFree(pTPs);

	return status;
}

//Ϊ����������Ȩ
bool SuSetTokenPrivilege(
	_In_ HANDLE TokenHandle,
	_In_ TokenPrivilegesList Privilege,
	_In_ bool bEnable)
{
	NTSTATUS status;
	TOKEN_PRIVILEGES TP = { 0 };
	TP.PrivilegeCount = 1;
	TP.Privileges[0].Luid.LowPart = Privilege;
	TP.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : NULL;

	// Adjust a Privilege in a Token
	status = NtAdjustPrivilegesToken(
		TokenHandle, FALSE, &TP, NULL, NULL, NULL);

	// Return the Result
	return RtlNtStatusToDosError(status) == ERROR_SUCCESS;
}

//���õ�ǰ��������Ȩ��
bool SuSetCurrentPsPrivilege(
	_In_ TokenPrivilegesList Privilege,
	_In_ bool bEnable)
{
	bool bRet = false;
	HANDLE hCurrentProcessToken;
	if (NT_SUCCESS(SuGetCurrentPsToken(&hCurrentProcessToken)))
	{
		bRet = SuSetTokenPrivilege(hCurrentProcessToken, Privilege, bEnable);
		NtClose(hCurrentProcessToken);
	}
	return bRet;
}