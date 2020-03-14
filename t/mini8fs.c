#include <mini8fs.h>
#include <stdio.h>

void readfs(char *filename) {
    FILE* f = fopen(filename, "r");
    fread(&m8_memory, 1, 1088, f);
    fclose(f);
}

uint8_t is_u8(uint8_t got, uint8_t exp, char* msg) {
    if (got == exp) {
        printf("ok: \"%s\"\n", msg);
        return 1;
    }
    printf("bad: got %d, exp %d \"%s\"\n", got, exp, msg);
    return 0;
}

uint8_t is_u32(uint32_t got, uint32_t exp, char* msg) {
    if (got == exp) {
        printf("ok: \"%s\"\n", msg);
        return 1;
    }
    printf("bad: got %ld, exp %ld \"%s\"\n", got, exp, msg);
    return 0;
}

/*
uint8_t is_mem() {
    uint8_t v;
    scanf("%X", &v);
}
*/

uint8_t is_not_null(uint8_t* got, char* msg) {
    if (got != NULL) {
        printf("ok: \"%s\"\n", msg);
        return 1;
    }
    printf("bad: got NULL \"%s\"\n", msg);
    return 0;
}

int test_blk_functions() {
    uint8_t pass = 0;

    uint8_t* addr0 = m8_blkc_find(0, (uint8_t*)"dev", 3);
    pass += is_not_null(addr0, "found address for 'dev'");

    uint8_t* addr1 = m8_blkc_find(2, (uint8_t*)"file7", 5);
    pass += is_not_null(addr1, "found address for 'file7'");

    uint8_t blkid0 = m8_find_cons_blks(1);
    pass += is_u8(blkid0, 7, "found 1 free block at 7");

    uint8_t blkid1 = m8_find_cons_blks(2);
    pass += is_u8(blkid1, 7, "found 2 free blocks at 7");

    uint8_t blkid2 = m8_find_cons_blks(3);
    pass += is_u8(blkid2, 10, "found 3 free blocks at 10");

    uint8_t blkid3 = m8_find_cons_blks(31);
    pass += is_u8(blkid3, 0, "no free blocks found");

    //uint8_t* addr2 = m8_link_cons_blocks(2);
    //pass += is_mem(&m8_memory[7], "01 08 01 00", "blocks written correctly");

    return pass;
}

int main() {
    readfs("fsimg.bin");
    test_blk_functions();
    return 0;
}
