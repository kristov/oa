#include <mini8fs.h>

#define BIT_TEST(a, f)   ((a >> f) & 1)
uint8_t m8_memory[1088];

uint8_t* m8_blk_addr(uint8_t blockid) {
    return &m8_memory[(blockid * M8_BLOCK_SIZE) + M8_FT_SIZE];
}

void m8_ent_setname(uint8_t* ent, uint8_t* name, uint8_t strlen) {
    uint8_t i = 0;
    for (i = 0; i < M8_FNAME_LEN; i++) {
        ent[i] = 0;
    }
    for (i = 0; i < strlen; i++) {
        ent[i] = name[i];
    }
}

uint8_t* m8_blkc_find(uint8_t blockid, uint8_t* name, uint8_t strlen) {
    do {
        uint8_t* blkaddr = m8_blk_addr(blockid);
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
        blockid = m8_memory[(blockid * 2) + 1];
    } while (blockid);
    return 0;
}

uint8_t m8_find_cons_blks(uint8_t nrblocks) {
    uint8_t bs = 0;
    uint8_t bc = 0;
    uint8_t nb = nrblocks;
    do {
        if (m8_memory[bc * 2]) {
            nb = nrblocks;
            bc++;
            bc = bc & M8_NR_BLOCKS_MASK;
            bs = bc;
            continue;
        }
        nb--;
        if (!nb) {
            return bs;
        }
        bc++;
        bc = bc & M8_NR_BLOCKS_MASK;
    } while (bc);
    return 0;
}

uint8_t* m8_link_cons_blks(uint8_t blockid, uint8_t nrblocks) {
    uint8_t* addr = m8_blk_addr(blockid);
    uint8_t i = blockid * 2;
    while (nrblocks) {
        blockid++;
        m8_memory[i] = 1; i++;
        nrblocks--;
        if (nrblocks) {
            m8_memory[i] = blockid; i++;
        }
    };
    return addr;
}

uint8_t* m8_blkc_extend(uint8_t blockid, uint8_t nrblocks) {
    uint8_t lblockid;
    do {
        lblockid = blockid;
        blockid = m8_memory[(blockid * 2) + 1];
    } while (blockid);
    blockid = m8_find_cons_blks(nrblocks);
    if (!blockid) {
        return 0;
    }
    m8_memory[(lblockid * 2) + 1] = blockid;
    return m8_link_cons_blks(blockid, nrblocks);
}

uint8_t* m8_blkc_dfree(uint8_t blockid) {
    uint8_t lblockid;
    do {
        uint8_t* blkaddr = m8_blk_addr(blockid);
        uint8_t b = M8_FILES_PER_BLOCK;
        while (b) {
            if (!blkaddr[M8_STATUS_BYTE]) {
                return blkaddr;
            }
            blkaddr += M8_FENTRY_LEN;
            b--;
        }
        lblockid = blockid;
        blockid = m8_memory[(blockid * 2) + 1];
    } while (blockid);
    return m8_blkc_extend(lblockid, 1);
}

uint8_t m8_unlink_cons_blks(uint8_t blockid) {
    uint8_t c = 0;
    uint8_t i = 0;
    while (blockid) {
        i = blockid * 2;
        m8_memory[i] = 0; i++;
        blockid = m8_memory[i];
        m8_memory[i] = 0;
        c++;
    };
    return c;
}

uint8_t* m8_path_find(uint8_t blockid, uint8_t* path) {
    uint8_t* part = path;
    uint8_t strlen = 0;
    uint8_t c = *part;
    while (c != 0) {
        if (c == '/') {
            uint8_t* ptr = m8_blkc_find(blockid, path, strlen);
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
    return m8_blkc_find(blockid, path, strlen);
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
    return m8_unlink_cons_blks(blockid);
}

uint8_t* m8_mkdir(uint8_t blockid, uint8_t* path) {
    uint8_t* part = path;
    uint8_t strlen = 0;
    uint8_t c = *part;
    while (c != 0) {
        if (c == '/') {
            uint8_t* ptr = m8_blkc_find(blockid, path, strlen);
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
    uint8_t* entry = m8_blkc_dfree(blockid);
    if (!entry) {
        return 0;
    }
    m8_ent_setname(entry, path, strlen);
    entry[M8_STATUS_BYTE] = 0xff;
    uint8_t nblockid = m8_find_cons_blks(1);
    if (!nblockid) {
        return 0;
    }
    entry[M8_BLOCKID_BYTE] = nblockid;
    uint8_t* nblock = m8_link_cons_blks(nblockid, 1);
    m8_ent_setname(nblock, (uint8_t*)"..", 2);
    nblock[M8_STATUS_BYTE] = 0xff;
    nblock[M8_BLOCKID_BYTE] = blockid;
    return nblock;
}

//    (0+31)/32
