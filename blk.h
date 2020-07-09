#ifndef BLK_H
#define BLK_H
/**
 * @file blk.h
 * @author Chris Eade
 * @date 2020-06-22
 * @brief In-memory block storage
 *
 * This presents an interface to a chunk of memory organised as blocks of a
 * fixed size. Blocks can be fetched and used to store data, and then "freed"
 * to release them back to availability. It's supposed to separate the file
 * system part of mini8fs from the block storage part.
 */
#include <stdint.h>

/**
 * @brief The total number of available blocks
 *
 * This must be a multiple of 8. This number multiplied by the block size will
 * be the total size of the file system excluding the file table (which is
 * BLK_NR_BLOCKS multiplied by 2).
 */
#define BLK_NR_BLOCKS 32

/**
 * @brief The size of each block in bytes
 *
 * Each block must be big enough to hold a reasonable number of data, but not
 * too much or space is wasted on small chunks of data.
 */
#define BLK_BLOCK_SIZE 32

/**
 * @brief A bitmask for wrapping number of blocks in block table
 *
 * So we can increment the block id and have it wrap around.
 */
#define BLK_NR_BLOCKS_MASK (BLK_NR_BLOCKS - 1)

/**
 * @brief Total size of the block table
 *
 * There are two bytes per block in the block table. One status byte, and one
 * "next" block id link byte.
 */
#define BLK_FT_SIZE (BLK_NR_BLOCKS * 2)

/**
 * @brief Total memory required for block table and all blocks
 */
#define BLK_MEM_SIZE ((BLK_NR_BLOCKS * BLK_BLOCK_SIZE) + BLK_FT_SIZE)

/**
 * @brief Macro for block id to address
 *
 * Calculate the memory address of a given block id. Since the block table is
 * at the beginning it is the size of this table plus the block id multiplied
 * by the block size.
 */
#define BLK_ADDR(blkid)  &blk_memory[(blkid * BLK_BLOCK_SIZE) + BLK_FT_SIZE];

/**
 * @brief A chunk of memory for storage of the blocks
 */
extern uint8_t blk_memory[BLK_MEM_SIZE];

/**
 * @brief Find nrblks consecutive free blocks
 *
 */
uint8_t blk_find_cons_blks(uint8_t nrblks);

/**
 * @brief Link nrblks together into a chain
 *
 */
uint8_t* blk_link_cons_blks(uint8_t blkid, uint8_t nrblks);

/**
 * @brief Unlink a chain of blocks
 *
 */
uint8_t blk_unlink_cons_blks(uint8_t blkid);

/**
 * @brief Extend a list of blocks
 *
 */
uint8_t* blk_extend(uint8_t blockid, uint8_t nrblocks);

/**
 * @brief Init block memory
 *
 */
uint8_t blk_init();
#endif
