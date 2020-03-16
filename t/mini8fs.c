#include <mini8fs.h>
#include <stdio.h>
#include <string.h>

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

uint8_t is_null(uint8_t* got, char* msg) {
    if (got == NULL) {
        printf("ok: \"%s\"\n", msg);
        return 1;
    }
    printf("bad: not NULL \"%s\"\n", msg);
    return 0;
}

uint8_t is_not_null(uint8_t* got, char* msg) {
    if (got != NULL) {
        printf("ok: \"%s\"\n", msg);
        return 1;
    }
    printf("bad: got NULL \"%s\"\n", msg);
    return 0;
}

uint8_t test_blk_functions() {
    uint8_t pass = 0;

    uint8_t* addr0 = m8_blkc_find(0, (uint8_t*)"dev", 3);
    pass += is_not_null(addr0, "found address for 'dev'");

    uint8_t* addr1 = m8_blkc_find(2, (uint8_t*)"file7", 5);
    pass += is_not_null(addr1, "found address for 'file7'");

    uint8_t* addr2 = m8_blkc_find(2, (uint8_t*)"file6", 5);
    pass += is_not_null(addr2, "found address for 'file6'");

    uint8_t blkid0 = m8_find_cons_blks(1);
    pass += is_u8(blkid0, 7, "found 1 free block at 7");

    uint8_t blkid1 = m8_find_cons_blks(2);
    pass += is_u8(blkid1, 7, "found 2 free blocks at 7");

    uint8_t blkid2 = m8_find_cons_blks(3);
    pass += is_u8(blkid2, 10, "found 3 free blocks at 10");

    uint8_t blkid3 = m8_find_cons_blks(31);
    pass += is_u8(blkid3, 0, "no free blocks found");

    uint8_t* addr3 = m8_link_cons_blks(7, 2);
    pass += is_not_null(addr3, "found address for linked blocks");
    pass += is_u8(m8_memory[14], 1, "chunk 0,0 written correctly");
    pass += is_u8(m8_memory[15], 8, "chunk 0,1 written correctly");
    pass += is_u8(m8_memory[16], 1, "chunk 1,0 written correctly");
    pass += is_u8(m8_memory[17], 0, "chunk 1,1 written correctly");

    uint8_t* addr4 = m8_blkc_extend(9, 6);
    pass += is_not_null(addr4, "found address for extended blocks");
    pass += is_u8(m8_memory[19], 10, "block linked to previous block");
    pass += is_u8(m8_memory[20], 1, "chunk 0,0 written correctly");
    pass += is_u8(m8_memory[21], 11, "chunk 0,1 written correctly");
    pass += is_u8(m8_memory[28], 1, "chunk 4,0 written correctly");
    pass += is_u8(m8_memory[29], 15, "chunk 4,0 written correctly");
    pass += is_u8(m8_memory[30], 1, "chunk 5,0 written correctly");
    pass += is_u8(m8_memory[31], 0, "chunk 5,0 written correctly");

    uint8_t* addr5 = m8_blkc_extend(9, 1);
    pass += is_not_null(addr5, "found address for extended blocks");
    pass += is_u8(m8_memory[31], 20, "block linked to previous block");
    pass += is_u8(m8_memory[40], 1, "chunk 0,0 written correctly");
    pass += is_u8(m8_memory[41], 0, "chunk 0,0 written correctly");

    uint8_t* addr6 = m8_blkc_dfree(1);
    pass += is_not_null(addr6, "found address for dfree");
    addr6 = addr6 - 8;
    pass += is_u8(memcmp(addr6, "stdout", 6), 0, "previous entry detected");

    uint8_t unlinked = m8_unlink_cons_blks(2);
    pass += is_u8(unlinked, 2, "correct number blocks unlinked");
    pass += is_u8(m8_memory[4], 0, "chunk 0,0 cleared correctly");
    pass += is_u8(m8_memory[5], 0, "chunk 0,1 cleared correctly");
    pass += is_u8(m8_memory[6], 0, "chunk 1,0 cleared correctly");
    pass += is_u8(m8_memory[7], 0, "chunk 1,1 cleared correctly");

    return pass;
}

uint8_t test_ops_functions() {
    uint8_t pass = 0;

    uint8_t* addr0 = m8_path_find(0, (uint8_t*)"dev");
    pass += is_not_null(addr0, "found address for dev");
    pass += is_u8(memcmp(addr0, "dev", 3), 0, "name dev correct");

    uint8_t* addr1 = m8_path_find(0, (uint8_t*)"dev/stdin");
    pass += is_not_null(addr1, "found address for dev/stdin");
    pass += is_u8(memcmp(addr1, "stdin", 5), 0, "name stdin correct");

    uint8_t* addr2 = m8_path_find(2, (uint8_t*)"file1");
    pass += is_not_null(addr2, "found address for file1 from prog");
    pass += is_u8(memcmp(addr2, "file1", 5), 0, "name file1 correct");

    uint8_t* addr3 = m8_path_find(0, (uint8_t*)"prog/file6");
    pass += is_not_null(addr3, "found address for prog/file6");
    pass += is_u8(memcmp(addr3, "file6", 5), 0, "name file6 correct");

    uint8_t* addr4 = m8_path_find(2, (uint8_t*)"../prog/file6");
    pass += is_not_null(addr4, "found address for relative ../prog/file6");
    pass += is_u8(memcmp(addr4, "file6", 5), 0, "name file6 correct");

    uint8_t nrblocks = m8_path_rm(0, (uint8_t*)"prog/file4");
    pass += is_u8(nrblocks, 4, "unlinked 4 blocks");
    uint8_t* addr5 = m8_path_find(0, (uint8_t*)"prog/file4");
    pass += is_null(addr5, "file not found anymore");

    uint8_t* addr6 = m8_mkdir(0, (uint8_t*)"dev/init");
    pass += is_not_null(addr6, "new dir block returned");
    pass += is_u8(m8_memory[14], 1, "new block 7 allocated for directory");
    pass += is_u8(memcmp(addr6, "..", 2), 0, "parent dir created correctly");

    uint8_t* addr7 = m8_mkdir(0, (uint8_t*)"dev/overf");
    pass += is_not_null(addr7, "new dir block returned");
    pass += is_u8(m8_memory[16], 1, "new block 8 allocated for overflow");
    pass += is_u8(memcmp(addr7, "..", 2), 0, "parent dir created correctly");

    return pass;
}

int main() {
    readfs("fsimg.bin");
    test_blk_functions();
    readfs("fsimg.bin");
    test_ops_functions();
    return 0;
}
