// NSudo������ - �ַ�������(����Native API)

#pragma once

// PS: ��Ӧ�������� "ntdll.lib"

// NT�ַ����ṹ
typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

// NT�ַ�������API
extern "C"
{
	NTSYSAPI VOID NTAPI RtlInitUnicodeString(
		_Out_ PUNICODE_STRING DestinationString,
		_In_opt_ PWSTR SourceString
		);

	NTSYSAPI VOID NTAPI RtlFreeUnicodeString(
		_In_ PUNICODE_STRING UnicodeString
		);
}