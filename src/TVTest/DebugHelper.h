#ifndef DEBUG_HELPER_H
#define DEBUG_HELPER_H


#if !defined(_DEBUG) && !defined(_WIN64)
#define ENABLE_DEBUG_HELPER
#endif


#ifdef ENABLE_DEBUG_HELPER
#include <dbghelp.h>
#endif


class CDebugHelper {
public:
	enum ExceptionFilterMode {
		EXCEPTION_FILTER_DEFAULT,
		EXCEPTION_FILTER_NONE,
		EXCEPTION_FILTER_DIALOG
	};

private:
#ifdef ENABLE_DEBUG_HELPER
	typedef BOOL (WINAPI *SymInitializeFunc)(HANDLE hProcess,PCTSTR UserSearchPath,BOOL fInvadeProcess);
	typedef BOOL (WINAPI *SymCleanupFunc)(HANDLE hProcess);
	typedef BOOL (WINAPI *SymFromAddrFunc)(HANDLE hProcess,DWORD64 Address,PDWORD64 Displacement,PSYMBOL_INFO Symbol);
	typedef DWORD (WINAPI *SymSetOptionsFunc)(DWORD SymOptions);
	typedef DWORD (WINAPI *SymLoadModuleFunc)(HANDLE hProcess,HANDLE hFile,
		PSTR ImageName,PSTR ModuleName,DWORD BaseOfDll,DWORD SizeOfDll);
	typedef BOOL (WINAPI *StackWalk64Func)(DWORD MachineType,HANDLE hProcess,
		HANDLE hThread,LPSTACKFRAME64 StackFrame,PVOID ContextRecord,
		PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
		PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
		PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
		PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress);

	static HMODULE m_hDbgHelp;
	static SymInitializeFunc m_pSymInitialize;
	static SymCleanupFunc m_pSymCleanup;
	static SymFromAddrFunc m_pSymFromAddr;
	static SymSetOptionsFunc m_pSymSetOptions;
	static SymLoadModuleFunc m_pSymLoadModule;
	static StackWalk64Func m_pStackWalk64;
#endif
	static ExceptionFilterMode m_ExceptionFilterMode;
	static LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo);

public:
	CDebugHelper();
	~CDebugHelper();
	static bool SetExceptionFilterMode(ExceptionFilterMode Mode);
};


#endif