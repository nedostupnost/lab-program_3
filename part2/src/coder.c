#include "coder.h"
#include <stdio.h>

int encode(uint32_t code_point, CodeUnits *code_units) 
{
    if (code_point <= 0x7F) // <= 1 byte
    {
        code_units->code[0] = (uint8_t)code_point;
        code_units->length = 1;
    } 
    else if (code_point <= 0x3FFF) // <= 2 byte
    {
        code_units->code[0] = (uint8_t)((code_point >> 7) | 0x80);
        code_units->code[1] = (uint8_t)(code_point & 0x7F);
        code_units->length = 2;
    } 
    else if (code_point <= 0x1FFFFF) // <= 3 byte
    {
        code_units->code[0] = (uint8_t)((code_point >> 14) | 0xC0);
        code_units->code[1] = (uint8_t)((code_point >> 7) | 0x80);
        code_units->code[2] = (uint8_t)(code_point & 0x7F);
        code_units->length = 3;
    } 
    else if (code_point <= 0x1FFFFFFF) // <= 4 byte
    {
        code_units->code[0] = (uint8_t)((code_point >> 21) | 0xE0);
        code_units->code[1] = (uint8_t)((code_point >> 14) | 0x80);
        code_units->code[2] = (uint8_t)((code_point >> 7) | 0x80);
        code_units->code[3] = (uint8_t)(code_point & 0x7F);
        code_units->length = 4;
    } 
    else // > 4 byte
    {
        return -1;
    }
    return 0;
}

uint32_t decode(const CodeUnits *code_unit) 
{
    uint32_t code_point = 0;
    for (size_t i = 0; i < code_unit->length; ++i) 
    {
        code_point <<= 7;
        code_point |= code_unit->code[i] & 0x7F;
    }
    return code_point;
}

int read_next_code_unit(FILE *in, CodeUnits *code_units) 
{
    code_units->length = 0;
    uint8_t byte;
    while (fread(&byte, sizeof(uint8_t), 1, in) == 1) 
    {
        code_units->code[code_units->length++] = byte;
        if (!(byte & 0x80)) 
        {
            break;
        }
    }
    if (feof(in)) 
    {
        return EOF;
    }
    return 0;
}

int write_code_unit(FILE *out, const CodeUnits *code_unit) 
{
    if (fwrite(code_unit->code, sizeof(uint8_t), code_unit->length, out) != code_unit->length) 
    {
        return -1;
    }
    return 0;
}