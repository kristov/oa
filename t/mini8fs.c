#include <mini8fs.h>
#include <stdio.h>
#include <string.h>

void readfs(char *filename) {
    FILE* f = fopen(filename, "r");
    fread(&blk_memory, 1, 1088, f);
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

    return pass;
}

uint8_t test_ops_functions() {
    uint8_t pass = 0;
    uint8_t* addr = NULL;

    addr = m8_find(0, (uint8_t*)"dev", 3);
    pass += is_not_null(addr, "found address for 'dev'");

    addr = m8_find(2, (uint8_t*)"file7", 5);
    pass += is_not_null(addr, "found address for 'file7'");

    addr = m8_find(2, (uint8_t*)"file6", 5);
    pass += is_not_null(addr, "found address for 'file6'");

    addr = m8_dfree(1);
    pass += is_not_null(addr, "found address for dfree");
    addr = addr - 8;
    pass += is_u8(memcmp(addr, "stdout", 6), 0, "previous entry detected");

    addr = m8_path_find(0, (uint8_t*)"dev");
    pass += is_not_null(addr, "found address for dev");
    pass += is_u8(memcmp(addr, "dev", 3), 0, "name dev correct");

    addr = m8_path_find(0, (uint8_t*)"dev/stdin");
    pass += is_not_null(addr, "found address for dev/stdin");
    pass += is_u8(memcmp(addr, "stdin", 5), 0, "name stdin correct");

    addr = m8_path_find(2, (uint8_t*)"file1");
    pass += is_not_null(addr, "found address for file1 from prog");
    pass += is_u8(memcmp(addr, "file1", 5), 0, "name file1 correct");

    addr = m8_path_find(0, (uint8_t*)"prog/file6");
    pass += is_not_null(addr, "found address for prog/file6");
    pass += is_u8(memcmp(addr, "file6", 5), 0, "name file6 correct");

    addr = m8_path_find(2, (uint8_t*)"../prog/file6");
    pass += is_not_null(addr, "found address for relative ../prog/file6");
    pass += is_u8(memcmp(addr, "file6", 5), 0, "name file6 correct");

    uint8_t nrblocks = m8_path_rm(0, (uint8_t*)"prog/file4");
    pass += is_u8(nrblocks, 4, "unlinked 4 blocks");
    addr = m8_path_find(0, (uint8_t*)"prog/file4");
    pass += is_null(addr, "file not found anymore");

    addr = m8_mkdir(0, (uint8_t*)"dev/init");
    pass += is_not_null(addr, "new dir block returned");
    pass += is_u8(blk_memory[14], 1, "new block 7 allocated for directory");
    pass += is_u8(memcmp(addr, "..", 2), 0, "parent dir created correctly");

    addr = m8_mkdir(0, (uint8_t*)"dev/overf");
    pass += is_not_null(addr, "new dir block returned");
    pass += is_u8(blk_memory[16], 1, "new block 8 allocated for overflow");
    pass += is_u8(memcmp(addr, "..", 2), 0, "parent dir created correctly");

    addr = m8_newfile(0, (uint8_t*)"dev/file0", 9);
    pass += is_not_null(addr, "new file created");

    return pass;
}

int main() {
    readfs("fsimg.bin");
    test_ops_functions();
    return 0;
}
