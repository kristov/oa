#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdint.h>

struct rb {
    uint8_t mask;
    uint8_t write;
    uint8_t read;
    uint8_t buffer[16];
};

/** @brief Read a byte from the ring buffer
 *
 * Reads a byte from the ring buffer into a reference to the variable provided.
 *
 * @return Zero on success one on failure
 * @param Pointer to a buffer struct
 * @param Pointer to the destination variable
*/
uint8_t rb_read(struct rb* buff, uint8_t* c);

/** @brief Write a byte to the ring buffer
 *
 * Writes a byte to the ring buffer.
 *
 * @return Zero on success one on failure
 * @param Pointer to a buffer struct
 * @param The value to write
*/
uint8_t rb_write(struct rb* buff, uint8_t c);

/** @brief Get the remaining space in the buffer
 *
 * Get the remaining space in the buffer
 *
 * @return Number of bytes space on the buffer
 * @param Pointer to a buffer struct
*/
uint8_t rb_space(struct rb* buff);

/** @brief Initialize a new buffer
 *
 * Initialize a new buffer.
 *
 * @return Zero on success one on failure
 * @param Pointer to a buffer struct
 * @param The mask (buffer size)
*/
uint8_t rb_init(struct rb* buff, uint8_t mask);

#endif
