#include <stdlib.h>
#include <string.h>

#ifdef __windows__
#include <windows.h>
#include <tlhelp32.h>
#else
#include <unistd.h>
#endif

#include "sak.h"

char *sak_progname(void)
{
    static char prog_name[1024] = {0};
    if (prog_name[0]){
        return prog_name;
    }

    char *pname = NULL;
    char buf[1024] = {0};

#ifdef __windows__
    DWORD pid = (DWORD)(sak_progid());
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
#else
    int ret = readlink("/proc/self/exe", buf, 1023);
    if (ret <= 0 || ret >=1024){
        return "";
    }

    pname = strrchr(buf, '/');
#endif
    pname = pname ? pname + 1 : buf;

    strcpy(prog_name, pname);

    return prog_name;
}

int sak_progid(void)
{
    int pid = -1;

#ifdef __windows__
    pid = (int)(GetCurrentProcessId());
#else
    pid = (int)(getpid());
#endif
 
    return pid;
}
