/**
 *  file    types.h
 *  date    2008/12/14
 *  author  kkamagui
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   커널에서 사용하는 각종 타입을 정의한 파일
 */

#ifndef MINTOS64_TYPES_H
#define MINTOS64_TYPES_H

#define kbyte_t unsigned char
#define kword_t unsigned short
#define kdword_t unsigned int
#define kqword_t unsigned long
#define kbool_t unsigned char

#define TRUE 1
#define FALSE 0
#define NULL ((void *) 0)

#pragma pack(push, 1)

// 비디오 모드 중 텍스트 모드 화면을 구성하는 자료구조
typedef struct kvideo_char {
    kbyte_t value;
    kbyte_t attribute;
} kvideo_char_t;

#pragma pack(pop)

#endif //MINTOS64_TYPES_H
