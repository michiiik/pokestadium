#ifndef _2E110_H_
#define _2E110_H_

#include "global.h"

void Text_InitStringTables(void);
char** Text_GetStringTable(s32 file_number);
void Text_SetStringToken(u32 arg0, u32 arg1);
void Text_SetNumberToken(u32 arg0, u32 arg1);
void Text_SubstituteTokens(char* arg0, u32 arg1, s8* arg2);
char* Text_GetString(char* arg0, s32 arg1, char** arg2, u32 file_number);
s32 Text_CountLines(s8* arg0);

#endif // _2E110_H_
