/**
 *  file    mode_switch.h
 *  date    2009/01/01
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   모드 전환에 관련된 함수들을 정의한 파일
 */
 
#ifndef MINTOS64_MODE_SWITCH_H
#define MINTOS64_MODE_SWITCH_H

#include "types.h"

void _KREAD_CPUID(kdword_t eax, kdword_t *peax, kdword_t *pebx, kdword_t *pecx, kdword_t *pedx);
#define kread_cpuid _KREAD_CPUID

void _KSWITCH_TO_IA32E();
#define kswitch_to_ia32e _KSWITCH_TO_IA32E


#endif //MINTOS64_MODE_SWITCH_H
