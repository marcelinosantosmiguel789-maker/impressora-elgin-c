#ifndef TESTS_MOCK_WINDOWS_H
#define TESTS_MOCK_WINDOWS_H

/* API Win32 mínima usada por main.c; somente para testes automatizados sem hardware. */
typedef void *HMODULE;
typedef int (*FARPROC)(void);

HMODULE LoadLibraryA(const char *nome);
FARPROC GetProcAddress(HMODULE modulo, const char *nome);
int FreeLibrary(HMODULE modulo);
unsigned long GetLastError(void);

#endif
