#include <windows.h>
#include <tlhelp32.h>
#include <stdlib.h>
#include <string.h>

char *getpname(void)
{
    static char prog_name[1024] = {0};
    if (prog_name[0]){
        return prog_name;
    }
    char buf[1024] = {0};
    char *pname = NULL;

    DWORD pid = GetCurrentProcessId();
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(hProcessSnap == INVALID_HANDLE_VALUE){
        return NULL;
    }
    BOOL bMore = Process32First(hProcessSnap,&pe32);
    while(bMore){
        if (pe32.th32ProcessID == pid){
            strcpy(buf, pe32.szExeFile);
            break;
        }
        bMore = Process32Next(hProcessSnap, &pe32);
    }
    CloseHandle(hProcessSnap);

    pname = strrchr(buf, '/');
    if (!pname){
        pname = strrchr(buf, '\\');
    }
    pname = pname ? pname + 1 : buf;
    strcpy(prog_name, pname);

    return prog_name;
}
