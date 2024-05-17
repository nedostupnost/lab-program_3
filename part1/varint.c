#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define NUM_NUMBERS 1000000

uint32_t generate_number();
void write_varint(FILE *fp, uint32_t value);
void read_varint(FILE *fp, uint32_t *value);

int main() 
{
    FILE *uncompressed_fp = fopen("uncompressed.dat", "wb");
    FILE *compressed_fp = fopen("compressed.dat", "wb");

    if (uncompressed_fp == NULL || compressed_fp == NULL) 
    {
        perror("Unable to open file");
        return EXIT_FAILURE;
    }

    srand(time(NULL));

    for (int i = 0; i < NUM_NUMBERS; ++i) 
    {
        uint32_t num = generate_number();
        fwrite(&num, sizeof(uint32_t), 1, uncompressed_fp);
        write_varint(compressed_fp, num);
    }

    fclose(uncompressed_fp);
    fclose(compressed_fp);

    uncompressed_fp = fopen("uncompressed.dat", "rb");
    compressed_fp = fopen("compressed.dat", "rb");

    if (uncompressed_fp == NULL || compressed_fp == NULL) 
    {
        perror("Unable to open file");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < NUM_NUMBERS; ++i) 
    {
        uint32_t uncompressed_num, compressed_num;
        fread(&uncompressed_num, sizeof(uint32_t), 1, uncompressed_fp);
        read_varint(compressed_fp, &compressed_num);
        if (uncompressed_num != compressed_num) 
        {
            printf("Error: The sequence of numbers does not match!\n");
            return EXIT_FAILURE;
        }
    }

    fclose(uncompressed_fp);
    fclose(compressed_fp);

    printf("Verification completed successfully: number sequences match\n");

    return EXIT_SUCCESS;
}

uint32_t generate_number() 
{
    const int r = rand();
    const int p = r % 100;
    if (p < 90)
        return r % 128;

    if (p < 95) 
        return r % 16384;

    if (p < 99) 
        return r % 2097152;
    
    return r % 268435455;
}

void write_varint(FILE *fp, uint32_t value) 
{
    uint8_t buffer[5];
    int index = 0;
    while (value > 127) 
    {
        buffer[index++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    buffer[index++] = value & 0x7F;
    fwrite(buffer, 1, index, fp);
}

void read_varint(FILE *fp, uint32_t *value) 
{
    uint8_t byte;
    *value = 0;
    int shift = 0;
    do {
        fread(&byte, sizeof(uint8_t), 1, fp);
        *value |= (byte & 0x7F) << shift;
        shift += 7;
    } while (byte & 0x80);
}