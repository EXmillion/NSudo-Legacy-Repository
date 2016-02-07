// NSudo������ - �ڴ����(����Native API)

#pragma once

// PS: ��Ӧ�������� "ntdll.lib"

// NT�ѹ���API
extern "C"
{
	NTSYSAPI PVOID NTAPI RtlAllocateHeap(
		_In_ PVOID HeapHandle,
		_In_opt_ ULONG Flags,
		_In_ SIZE_T Size
		);

	NTSYSAPI BOOLEAN NTAPI RtlFreeHeap(
		_In_ PVOID HeapHandle,
		_In_opt_ ULONG Flags,
		_In_ _Post_invalid_ PVOID BaseAddress
		);
}

// �ڴ�����
#ifdef _AMD64_
#define ProcessHeap *(HANDLE *)(__readgsqword(96) + 48)
#else
#define ProcessHeap *(HANDLE *)(__readfsdword(48) + 24)
#endif	

// �����ڴ�
#define MemAlloc(Size) RtlAllocateHeap(ProcessHeap, NULL, Size)

// �ͷŷ����ڴ�
#define MemFree(BaseAddress) \
	if (BaseAddress) \
	{ \
		RtlFreeHeap(ProcessHeap, NULL, BaseAddress); \
		BaseAddress = NULL; \
	}
