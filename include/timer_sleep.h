// .s5SSSs.                                         .s5SSs.  .s5SSSSs.                                   
//       SS. .s5SSSs.  .s5SSSs. .s    s.  .s5SSSs.       SS.    SSS    s.  .s5ssSs.  .s5SSSs.  .s5SSSs.  
// sS    S%S       SS.                SS.       SS. sS   S%S    S%S    SS.    SS SS.       SS.       SS. 
// SS    S%S sS    S%S sS       sS    S%S sS    `:; SS   S%S    S%S    S%S sS SS S%S sS    `:; sS    S%S 
// SS    S%S SS .sSSS  SSSs.    SS    S%S `:;;;;.   SS   S%S    S%S    S%S SS :; S%S SSSs.     SS .sS;:' 
// SS    S%S SS    S%S SS       SS    S%S       ;;. SS   S%S    S%S    S%S SS    S%S SS        SS    ;,  
// SS    `:; SS    `:; SS       SS    `:;       `:; SS   `:;    `:;    `:; SS    `:; SS        SS    `:; 
// SS    ;,. SS    ;,. SS       SS    ;,. .,;   ;,. SS  `;,.    ;,.    ;,. SS    ;,. SS    ;,. SS    ;,. 
// `:;;;;;:' `:;;;;;:' :;       `:;;;;;:' `:;;;;;:' `:;;;;;;    ;:'    ;:' :;    ;:' `:;;;;;:' `:    ;:' 
                    
#ifndef TIMER_SLEEP_H
#define TIMER_SLEEP_H

#include <windows.h>

typedef struct {
    DWORD   Length;
    DWORD   MaximumLength;
    PVOID   Buffer;
} USTRING;

VOID ObfuscatedSleep(DWORD dwMilliseconds);

#endif