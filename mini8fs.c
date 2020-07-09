#include <mini8fs.h>

#define BIT_TEST(a, f)        ((a >> f) & 1)

void m8_ent_setname(uint8_t* ent, uint8_t* name, uint8_t strlen) {
    uint8_t i = 0;
    for (i = 0; i < M8_FNAME_LEN; i++) {
        ent[i] = 0;
    }
    for (i = 0; i < strlen; i++) {
        ent[i] = name[i];
    }
}

uint8_t* m8_find(uint8_t blockid, uint8_t* name, uint8_t strlen) {
    do {
        uint8_t* blkaddr = BLK_ADDR(blockid);
        uint8_t b = M8_FILES_PER_BLOCK;
        while (b) {
            uint8_t* str1 = blkaddr;
            uint8_t* str2 = name;
            uint8_t eq = 1;
            uint8_t c = strlen;
            while (c) {
                if (*str1 != *str2) {
                    eq = 0;
                    break;
                }
                str1++;
                str2++;
                c--;
            }
            if (eq) {
                return blkaddr;
            }
            blkaddr += M8_FENTRY_LEN;
            b--;
        }
        blockid = blk_memory[(blockid * 2) + 1];
    } while (blockid);
    return 0;
}

uint8_t* m8_blkc_iter(uint8_t blockid, bci callback, void* usr) {
    do {
        uint8_t* blkaddr = BLK_ADDR(blockid);
        uint8_t b = M8_FILES_PER_BLOCK;
        while (b) {
            uint8_t* ret = callback(blkaddr, usr);
            if (ret) {
                return ret;
            }
            blkaddr += M8_FENTRY_LEN;
            b--;
        }
        blockid = blk_memory[(blockid * 2) + 1];
    } while (blockid);
    return 0;
}

uint8_t m8_find_cons_blks(uint8_t nrblocks) {
    uint8_t bs = 0;
    uint8_t bc = 0;
    uint8_t nb = nrblocks;
    do {
        if (blk_memory[bc * 2]) {
            nb = nrblocks;
            bc++;
            bc = bc & BLK_NR_BLOCKS_MASK;
            bs = bc;
            continue;
        }
        nb--;
        if (!nb) {
            return bs;
        }
        bc++;
        bc = bc & BLK_NR_BLOCKS_MASK;
    } while (bc);
    return 0;
}

uint8_t* m8_dfree(uint8_t blockid) {
    uint8_t lblockid;
    do {
        uint8_t* blkaddr = BLK_ADDR(blockid);
        uint8_t b = M8_FILES_PER_BLOCK;
        while (b) {
            if (!blkaddr[M8_STATUS_BYTE]) {
                return blkaddr;
            }
            blkaddr += M8_FENTRY_LEN;
            b--;
        }
        lblockid = blockid;
        blockid = blk_memory[(blockid * 2) + 1];
    } while (blockid);
    return blk_extend(lblockid, 1);
}

uint8_t* m8_path_find(uint8_t blockid, uint8_t* path) {
    uint8_t* part = path;
    uint8_t strlen = 0;
    uint8_t c = *part;
    while (c != 0) {
        if (c == '/') {
            uint8_t* ptr = m8_find(blockid, path, strlen);
            if (ptr == 0) {
                return 0;
            }
            if (!BIT_TEST(ptr[M8_STATUS_BYTE], 7)) {
                return 0;
            }
            blockid = ptr[M8_BLOCKID_BYTE];
            part++;
            path = part;
            c = *part;
            strlen = 0;
            continue;
        }
        part++;
        c = *part;
        strlen++;
    };
    return m8_find(blockid, path, strlen);
}

uint8_t m8_path_rm(uint8_t blockid, uint8_t* path) {
    uint8_t* ptr = m8_path_find(blockid, path);
    if (ptr == 0) {
        return 1;
    }
    uint8_t i = 0;
    for (i = 0; i < M8_FNAME_LEN; i++) {
        ptr[i] = 0;
    }
    ptr[i] = 0; i++;
    blockid = ptr[i];
    return blk_unlink_cons_blks(blockid);
}

uint8_t* m8_newentry(uint8_t blockid, uint8_t* path) {
    uint8_t* part = path;
    uint8_t strlen = 0;
    uint8_t c = *part;
    while (c != 0) {
        if (c == '/') {
            uint8_t* ptr = m8_find(blockid, path, strlen);
            if (ptr == 0) {
                return 0;
            }
            if (!BIT_TEST(ptr[M8_STATUS_BYTE], 7)) {
                return 0;
            }
            blockid = ptr[M8_BLOCKID_BYTE];
            part++;
            path = part;
            c = *part;
            strlen = 0;
            continue;
        }
        part++;
        c = *part;
        strlen++;
    };
    uint8_t* entry = m8_dfree(blockid);
    if (!entry) {
        return 0;
    }
    m8_ent_setname(entry, path, strlen);
    return entry;
}

uint8_t* m8_mkdir(uint8_t blockid, uint8_t* path) {
    uint8_t nblockid = blk_find_cons_blks(1);
    if (!nblockid) {
        return 0;
    }
    uint8_t* entry = m8_newentry(blockid, path);
    if (!entry) {
        return 0;
    }
    entry[M8_STATUS_BYTE] = 0xff;
    entry[M8_BLOCKID_BYTE] = nblockid;
    uint8_t* nblock = blk_link_cons_blks(nblockid, 1);
    m8_ent_setname(nblock, (uint8_t*)"..", 2);
    nblock[M8_STATUS_BYTE] = 0xff;
    nblock[M8_BLOCKID_BYTE] = blockid;
    return nblock;
}

uint8_t* m8_newfile(uint8_t blockid, uint8_t* path, uint16_t size) {
    uint8_t nrblocks = (size + (BLK_BLOCK_SIZE - 1)) / BLK_BLOCK_SIZE;
    uint8_t nblockid = blk_find_cons_blks(nrblocks);
    if (!nblockid) {
        return 0;
    }
    uint8_t* entry = m8_newentry(blockid, path);
    if (!entry) {
        return 0;
    }
    entry[M8_STATUS_BYTE] = 0x7f;
    entry[M8_BLOCKID_BYTE] = nblockid;
    return blk_link_cons_blks(nblockid, 1);
}

uint8_t* m8_open(uint8_t blockid, uint8_t* path) {
    uint8_t* entry = m8_path_find(blockid, path);
    if (!entry) {
        return 0;
    }
    return BLK_ADDR(entry[M8_BLOCKID_BYTE]);
}

uint8_t m8_init() {
    blk_init();
    uint8_t* nblock = BLK_ADDR(0);
    m8_ent_setname(nblock, (uint8_t*)"..", 2);
    nblock[M8_STATUS_BYTE] = 0xff;
    nblock[M8_BLOCKID_BYTE] = 0;
    return 0;
}
