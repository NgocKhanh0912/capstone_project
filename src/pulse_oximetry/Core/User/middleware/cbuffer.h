/**
 * @file       cbuffer.h
 * @copyright
 * @license
 * @version    1.0.0
 * @date       26/06/2024
 * @author     Giang Phan Truong
 *             Khanh Nguyen Ngoc
 *             Viet Hoang Xuan
 *
 * @brief      Circular Buffer
 *             This Circular Buffer is safe to use in IRQ with single reader,
 *             single writer. No need to disable any IRQ.
 *
 * @note       Capacity = <size> - 1
 *             Should use correct size of buffer when init.
 * @example    cbuffer_t cb;
 *             uint8_t cb_buff[6];
 *             uint8_t a[] = {0, 1, 2};
 *             void main(void)
 *             {
 *                 int ret = 0;
 *
 *                 ret = cb_init(&cb, cb_buff, sizeof(cb_buff));
 *                 if (ret == CB_ERROR)
 *                   printf("Error\n");
 *
 *                 ret = cb_write(&cb, a, sizeof(a));
 *                 printf("Number of written bytes: %d\n", ret);
 *
 *                 char b[5];
 *                 ret = cb_read(&cb, b, sizeof(b));
 *                 printf("Number of read bytes: %d\n", ret);
 *
 *                 ret = cb_clear(&cb);
 *                 if (ret == CB_ERROR)
 *                   printf("Error\n");
 *             }
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __CBUFFER_H
#define __CBUFFER_H

/* Includes ----------------------------------------------------------- */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Public defines ----------------------------------------------------- */
#define CB_MAX_SIZE ((uint32_t)(0x00800000u)) /* Max size of circular buffer */

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief Circular buffer struct definition.
 */
typedef struct
{
  uint8_t *data;              /* Pointer to buffer */
  uint32_t size;              /* Size of buffer */
  volatile uint32_t writer;   /* Position to write */
  volatile uint32_t reader;   /* Position to read */
  volatile uint32_t overflow; /* How many byte are overflow */
  volatile bool active;       /* Initialized or not */
} cbuffer_t;

/**
 * @brief Circular buffer status enum definition.
 */
typedef enum
{
  CB_STATUS_ERROR  = 0xFFFFFFFF,
  CB_STATUS_FAILED = 0x7FFFFFFF,
  CB_STATUS_OK     = 0x3FFFFFFF,
} cbuffer_status_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief        Initialize circular buffer.
 *
 * @param[in]    cb      Pointer to a cbuffer_t structure.
 * @param[in]    buf     Pointer to array.
 * @param[in]    size    Size of buffer.
 *
 * @retval       CB_STATUS_OK: if the function works correctly.
 * @retval       CB_STATUS_ERROR: if the function encounters an error.
 */
cbuffer_status_t cb_init(cbuffer_t *cb, void *buf, uint32_t size);

/**
 * @brief        Clear circular buffer.
 *
 * @param[in]    cb    Pointer to a cbuffer_t structure.
 *
 * @retval       CB_STATUS_OK: if the function works correctly.
 * @retval       CB_STATUS_ERROR: if the function encounters an error.
 */
cbuffer_status_t cb_clear(cbuffer_t *cb);

/**
 * @brief         Read data from circular buffer.
 *
 * @param[in]     cb      Pointer to a cbuffer_t structure.
 * @param[out]    buf     Pointer to data buffer.
 * @param[in]     size    Size of data that want to read.
 *
 * @retval        Number of successfully read byte: if the function works correctly.
 * @retval        CB_STATUS_ERROR: if the function encounters an error.
 */
uint32_t cb_read(cbuffer_t *cb, void *buf, uint32_t nbytes);

/**
 * @brief        Write data to circular buffer.
 *
 * @param[in]    cb      Pointer to a cbuffer_t structure.
 * @param[in]    buf     Pointer to data buffer.
 * @param[in]    size    Size of data that want to write.
 *
 * @retval       Number of successfully write byte: if the function works correctly.
 * @retval       CB_STATUS_ERROR: if the function encounters an error.
 */
uint32_t cb_write(cbuffer_t *cb, void *buf, uint32_t nbytes);

/**
 * @brief        Return a number of byte in circular buffer.
 *
 * @param[in]    cb    Pointer to a cbuffer_t structure.
 *
 * @retval       Number of byte in circular buffer: if the function works correctly.
 * @retval       CB_STATUS_ERROR: if the function encounters an error.
 */
uint32_t cb_data_count(cbuffer_t *cb);

/**
 * @brief        Return a number of space (in byte unit) in circular buffer.
 *
 * @param[in]    cb    Pointer to a cbuffer_t structure.
 *
 * @retval       Number of space (in byte unit) in circular buffer: if the function works correctly.
 * @retval       CB_STATUS_ERROR: if the function encounters an error.
 */
uint32_t cb_space_count(cbuffer_t *cb);

#endif // __CBUFFER_H

/* End of file -------------------------------------------------------- */