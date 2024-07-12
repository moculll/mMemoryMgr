/**
 * @author mocul
 * @link https://github.com/moculll
 * @copyright 2024, mocul, Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MMGR_INIT_PARTS_MODE_BIT(part, partIndex, mode) ((part) |= (part) & ~(partIndex) | ((mode) << (partIndex)))

/**
 * @brief mMemoryMgr Init
 * @param memoryPoolSize Alloc Pool size
 * @param spiltParts Spilt parts, >= 1
 * @param partsMode 8-bit val, bit[i]: parts mode
 * @note use MMGR_INIT_PARTS_MODE_BIT(part, partIndex, mode) to set partsMode
 * @note mode[0]: aligned, mode[1]: reversed
 */
void mMemoryMgrInit(size_t memoryPoolSize, size_t spiltParts, size_t partsMode);

/**
 * @brief alloc a space from memory pool
 * @param size alloc size
 * @retval pointer to space, could be NULL
 * @note must use mMemoryMgrInit before calling this function
 */
void *mMemoryMalloc(size_t size);

/**
 * @brief alloc a space from a specific memory pool part
 * @param size alloc size
 * @param partNum spiltParts: 1, partNum: 0
 * @param partNum spiltParts: 2, partNum: 0/1
 */
void *mMemoryMallocFrom(size_t size, uint8_t partNum);


/**
 * @brief free alloced space
 * @param free (void *) pointer
 * @note must use mMemoryMgrInit before call this function
 */
void mMemoryFree(void *free);

#ifdef __cplusplus
}
#endif
