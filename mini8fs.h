/**
 * @file mini8fs.h
 * @author Chris Eade
 * @date 2020-03-26
 * @brief Tiny in-memory file system
 *
 * This is a simple and small in-memory file system, designed primarily for
 * micro controllers and other small computers.
 */

#ifndef MINI8FS_H
#define MINI8FS_H

#include <stdint.h>

/**
 * @brief The total number of available blocks
 *
 * This must be a multiple of 8. This number multiplied by the block size will
 * be the total size of the file system excluding the file table (which is
 * M8_NR_BLOCKS multiplied by 2).
 */
#define M8_NR_BLOCKS 32

/**
 * @brief The size of each block in bytes
 *
 * Each block must be big enough to hold a reasonable number of file entries
 * for a directory block. The absolute minimum size of a block is 8 bytes, but
 * this would be pretty useless and inefficient.
 */
#define M8_BLOCK_SIZE 32

/**
 * @brief The maximum allowed file name length
 *
 * If this is changed from 6 weird behaviour might happen.
 */
#define M8_FNAME_LEN 6

#define M8_NR_BLOCKS_MASK (M8_NR_BLOCKS - 1)
#define M8_FT_SIZE (M8_NR_BLOCKS * 2)
#define M8_FENTRY_LEN (M8_FNAME_LEN + 2)
#define M8_FILES_PER_BLOCK (M8_BLOCK_SIZE / M8_FENTRY_LEN)
#define M8_STATUS_BYTE M8_FNAME_LEN
#define M8_BLOCKID_BYTE (M8_FNAME_LEN + 1)
#define M8_MEM_SIZE ((M8_NR_BLOCKS * M8_BLOCK_SIZE) + M8_FT_SIZE)
#define M8_DIR_ENT_MASK (M8_FILES_PER_BLOCK - 1)

extern uint8_t m8_memory[M8_MEM_SIZE];

uint8_t* m8_blk_addr(uint8_t blockid);

uint8_t* m8_blkc_find(uint8_t blockid, uint8_t* name, uint8_t strlen);

uint8_t m8_find_cons_blks(uint8_t nrblocks);

uint8_t* m8_link_cons_blks(uint8_t blockid, uint8_t nrblocks);

uint8_t* m8_blkc_extend(uint8_t blockid, uint8_t nrblocks);

uint8_t* m8_blkc_dfree(uint8_t blockid);

uint8_t m8_unlink_cons_blks(uint8_t blockid);

/** @brief Find a file or directory entry by path
 *
 * Finds a file or directory by the path name and returns the address of the
 * entry in the parent directory. If this is a file you want to read data from
 * you must convert the blockid in the file entry into an address.
 *
 * @return The memory address of the entry in the directory table
 * @param blockid Block id of the relative path start
 * @param path The path including the file to be created
*/
uint8_t* m8_path_find(uint8_t blockid, uint8_t* path);

uint8_t m8_path_rm(uint8_t blockid, uint8_t* path);

/** @brief Create a new directory entry
 *
 * This function creates a new directory using a path string. The path string
 * should end in the desired directory name to be created. All previous parts
 * in the path should be directories. The blockid is the starting point.
 *
 * @return The memory address of the entry in the parent directory table
 * @param blockid Block id of the relative path start
 * @param path The path including the directory to be created
*/
uint8_t* m8_mkdir(uint8_t blockid, uint8_t* path);

/** @brief Create a new file
 *
 * This function creates a new file using a path string. The path string
 * should end in the desired directory name to be created. All previous parts
 * in the path should be directories. The blockid is the starting point.
 *
 * @return The memory address of the first block of the data
 * @param blockid Block id of the relative path start
 * @param path The path including the file to be created
 * @param path The size in bytes of the file
*/
uint8_t* m8_newfile(uint8_t blockid, uint8_t* path, uint16_t size);

/** @brief Open a file
 *
 * Open a file by its path and return the memory address of the first block.
 *
 * @return The memory address of the first block of the data
 * @param blockid Block id of the relative path start
 * @param path The path including the file to be created
*/
uint8_t* m8_open(uint8_t blockid, uint8_t* path);

uint8_t m8_init();

#endif
