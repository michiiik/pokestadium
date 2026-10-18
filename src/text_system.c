#include <stdio.h>
#include "text_system.h"
#include "src/jpeg_stream.h"
#include "src/memory.h"

#define D_800AF740_NUM_FILES 42

static BinArchive* gTextArchive;
static char** gTextTokenTable;

void Text_InitStringTables(void) {
    s32 i;

    gTextArchive = BinArchive_Open(textdata_ROM_START, NULL, 1, 0);
    gTextTokenTable = main_pool_alloc(D_800AF740_NUM_FILES * sizeof(char*), 0);

    for (i = 0; i < D_800AF740_NUM_FILES; i++) {
        gTextTokenTable[i] = NULL;
    }
}

// PORT: a text file starts with a big-endian count followed by that many big-endian u32 offsets (relative to the
// file start), swapped in place on first use. Files can be reloaded into a buffer at the same address, so whether a
// table is already swapped is decided from its contents: the first string follows the offset table, so in native
// order table[1] is a small value just past (count + 1) * 4, while an unswapped count reads as a huge number.
char** Text_GetStringTable(s32 file_number) {
    u32* table = (u32*)BinArchive_GetFile(gTextArchive, file_number);

    if (table == NULL) {
        return NULL;
    }
    if (!(table[0] < 0x10000 && table[1] >= (table[0] + 1) * 4 && table[1] < (table[0] + 1) * 4 + 0x10)) {
        u32 count = Port_Swap32(table[0]);
        u32 j;

        table[0] = count;
        for (j = 1; j <= count; j++) {
            table[j] = Port_Swap32(table[j]);
        }
        if (table[1] < (count + 1) * 4 || table[1] >= (count + 1) * 4 + 0x10) {
            fprintf(stderr, "[text] table %d has unexpected layout (count %u, first offset 0x%x)\n", file_number,
                    count, table[1]);
        }
    }
    return (char**)table;
}

void Text_SetStringToken(u32 arg0, uintptr_t arg1) {
    if ((arg0 >= 0x14) && (arg0 < 0x2A)) {
        gTextTokenTable[arg0] = (char*)arg1;
    }
}

void Text_SetNumberToken(u32 arg0, uintptr_t arg1) {
    if ((arg0 != 0) && (arg0 < 0xA)) {
        gTextTokenTable[arg0] = (char*)arg1;
    }
}

void Text_SubstituteTokens(char* text, u32 text_length, s8* token) {
    u32 i;
    s32 in_replace_mode;
    s32 current_char;
    char text_buffer[12];
    s8* replacement;
    char* sp54;
    u32 written_count;

    in_replace_mode = 0;
    written_count = 0;
    sp54 = replacement;
    replacement = sp54;
    i = 0;

    while (i < text_length - 1u) {
        switch (in_replace_mode) {
            case 0:
                current_char = *token++;
                if (current_char == '\x00') {
                    goto end;
                }

                if (current_char == '#') {
                    current_char = *token++;

                    current_char -= '0';
                    current_char = (*token++ + (current_char * 10)) - '0';
                    if ((current_char > 0) && (current_char < 10)) {
                        sprintf(text_buffer, "%d", gTextTokenTable[current_char]);
                        replacement = text_buffer;
                        in_replace_mode = 1;
                        continue;
                    }

                    if ((current_char >= 0x14) && (current_char < 0x2A)) {
                        if (gTextTokenTable[current_char] != NULL) {
                            replacement = gTextTokenTable[current_char];
                            in_replace_mode = 1;
                        }
                        continue;
                    }
                } else {
                    *text++ = current_char;
                    written_count++;
                    i++;
                }
                break;

            case 1:
                current_char = *replacement++;
                if (current_char == '\x00') {
                    in_replace_mode = 0;
                } else {
                    *text++ = current_char;
                    written_count++;
                    i++;
                }
                break;
        }
    }

end:
    *text++ = '\x00';
}

char* Text_GetString(char* text, s32 text_length, char** string_array, u32 file_number) {
    char* string;
    if (string_array == NULL) {
        return ""; // PORT: text archive not available yet (its tables are big-endian 32-bit offsets)
    }
    if (file_number >= ((u32*)string_array)[0]) {
        // PORT: out of range (e.g. Oak's gift Pokémon with an unset id 0xFFFF): the N64 read junk memory here and
        // printed garbage; on the host that is a wild pointer
        if (text != NULL && text_length > 0) {
            text[0] = '\0';
            return text;
        }
        return "";
    }
    char* string_array_lookup = (char*)string_array + ((u32*)string_array)[file_number + 1];

    if (text == NULL) {
        string = string_array_lookup;
    } else {
        string = text;
        Text_SubstituteTokens(text, text_length, string_array_lookup);
    }

    return string;
}

s32 Text_CountLines(s8* arg0) {
    s32 last_chr = 0;
    s32 line_count = 0;
    s32 chr = *arg0++;

    while (chr != 0) {
        last_chr = chr;
        if (chr == '\n') {
            line_count++;
        }
        chr = *arg0++;
    }

    if (last_chr != '\n') {
        line_count++;
    }

    return line_count;
}
