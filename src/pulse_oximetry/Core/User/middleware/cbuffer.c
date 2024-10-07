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
 *             Capacity = <size> - 1
 * @note       None
 */

/* Includes ----------------------------------------------------------- */
#include "cbuffer.h"
#include "common.h"

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
/**
 * @brief        Write 1 byte to circular buffer.
 *
 * @param[in]    cb      Pointer to a cbuffer_t structure.
 * @param[in]    byte    Data to write.
 *
 * @retval       CB_STATUS_OK: if the function works correctly.
 * @retval       CB_STATUS_ERROR: if the function encounters an error.
 */
static cbuffer_status_t cb_write_byte(cbuffer_t *cb, uint8_t byte);

/**
 * @brief        Read 1 byte from circular buffer.
 *
 * @param[in]    cb      Pointer to a cbuffer_t structure.
 * @param[in]    byte    Pointer to data buffer.
 *
 * @retval       CB_STATUS_OK: if the function works correctly.
 * @retval       CB_STATUS_ERROR: if the function encounters an error.
 */
static cbuffer_status_t cb_read_byte(cbuffer_t *cb, uint8_t *byte);

/* Function definitions ----------------------------------------------- */
cbuffer_status_t cb_init(cbuffer_t *cb, void *buf, uint32_t size)
{
  __ASSERT(cb != NULL, CB_STATUS_ERROR);
  __ASSERT(buf != NULL, CB_STATUS_ERROR);
  __ASSERT(size <= CB_MAX_SIZE, CB_STATUS_ERROR);

  cb->data     = buf;
  cb->size     = size;
  cb->writer   = 0;
  cb->reader   = 0;
  cb->overflow = 0;
  cb->active   = 1;

  return CB_STATUS_OK;
}

cbuffer_status_t cb_clear(cbuffer_t *cb)
{
  __ASSERT(cb != NULL, CB_STATUS_ERROR);

  cb->writer   = 0;
  cb->reader   = 0;
  cb->overflow = 0;

  return CB_STATUS_OK;
}

uint32_t cb_read(cbuffer_t *cb, void *buf, uint32_t nbytes)
{
  __ASSERT(cb != NULL, CB_STATUS_ERROR);
  __ASSERT(buf != NULL, CB_STATUS_ERROR);
  __ASSERT(cb->active == 1, CB_STATUS_ERROR);

  int data_count      = 0;
  int num_avail_bytes = 0;

  // Temporary deactive for processing
  cb->active = 0;

  data_count = cb_data_count(cb);

  if (data_count >= nbytes)
  {
    num_avail_bytes = nbytes;
  }
  else
  {
    num_avail_bytes = data_count;
  }

  for (int i = 0; i < num_avail_bytes; i++)
  {
    cb_read_byte(cb, (uint8_t *)buf + i);
  }

  cb->active = 1;

  return num_avail_bytes;
}

uint32_t cb_write(cbuffer_t *cb, void *buf, uint32_t nbytes)
{
  __ASSERT(cb != NULL, CB_STATUS_ERROR);
  __ASSERT(buf != NULL, CB_STATUS_ERROR);
  __ASSERT(cb->active == 1, CB_STATUS_ERROR);

  int space_count     = 0;
  int num_avail_bytes = 0;

  // Temporary deactive for processing
  cb->active = 0;

  space_count = cb_space_count(cb);

  if (space_count >= nbytes)
  {
    num_avail_bytes = nbytes;
    cb->overflow    = 0;
  }
  else
  {
    num_avail_bytes = space_count;
    cb->overflow    = nbytes - space_count;
  }

  for (int i = 0; i < num_avail_bytes; i++)
  {
    cb_write_byte(cb, *((uint8_t *)buf + i));
  }

  cb->active = 1;

  return num_avail_bytes;
}

uint32_t cb_data_count(cbuffer_t *cb)
{
  __ASSERT(cb != NULL, CB_STATUS_ERROR);

  int res = 0;

  if (cb->writer >= cb->reader)
  {
    res = cb->writer - cb->reader;
  }
  else
  {
    res = cb->size - cb->reader + cb->writer;
  }

  return res;
}

uint32_t cb_space_count(cbuffer_t *cb)
{
  __ASSERT(cb != NULL, CB_STATUS_ERROR);

  int res = 0;

  if (cb->reader > cb->writer)
  {
    res = cb->reader - cb->writer - 1;
  }
  else if (cb->reader < cb->writer)
  {
    res = cb->size - cb->writer + cb->reader - 1;
  }
  else
  {
    res = cb->size - 1;
  }

  return res;
}

static cbuffer_status_t cb_write_byte(cbuffer_t *cb, uint8_t byte)
{
  uint32_t next = cb->writer + 1;

  if (next == cb->size)
  {
    next = 0;
  }

  __ASSERT(next != cb->reader, CB_STATUS_ERROR);

  *(cb->data + cb->writer) = byte;
  cb->writer               = next;

  return CB_STATUS_OK;
}

static cbuffer_status_t cb_read_byte(cbuffer_t *cb, uint8_t *byte)
{
  __ASSERT(cb->reader != cb->writer, CB_STATUS_ERROR);

  uint32_t next = cb->reader + 1;

  if (next == cb->size)
  {
    next = 0;
  }

  *byte      = *(cb->data + cb->reader);
  cb->reader = next;

  return CB_STATUS_OK;
}

/* End of file -------------------------------------------------------- */