/**
 * @file       bsp_i2c.c
 * @copyright
 * @license
 * @version    1.0.0
 * @date       2024-07-20
 * @authors    Giang Phan Truong
 *             Khanh Nguyen Ngoc
 *             Viet Hoang Xuan
 *
 * @brief      Board Support Packages for I2C
 *
 * @note
 * @example    None
 */

/* Includes ----------------------------------------------------------- */
#include "bsp_i2c.h"
#include "common.h"

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */
bsp_i2c_status_t bsp_i2c_master_transmit(bsp_i2c_handle_t *hi2c, uint16_t device_address,
                                         uint8_t *data_pointer, uint16_t size, uint32_t time_out)
{
  // Check parameters
  __ASSERT((hi2c != NULL), BSP_I2C_ERROR);
  __ASSERT((data_pointer != NULL), BSP_I2C_ERROR);

  // Transmit
  HAL_StatusTypeDef ret;
  ret = HAL_I2C_Master_Transmit(hi2c, device_address, data_pointer, size, time_out);
  __ASSERT((ret == HAL_OK), BSP_I2C_FAILED);
  return BSP_I2C_OK;
}

bsp_i2c_status_t bsp_i2c_master_receive(bsp_i2c_handle_t *hi2c, uint16_t device_address,
                                        uint8_t *data_pointer, uint16_t size, uint32_t time_out)
{
  // Check parameters
  __ASSERT((hi2c != NULL), BSP_I2C_ERROR);
  __ASSERT((data_pointer != NULL), BSP_I2C_ERROR);

  // Receive
  HAL_StatusTypeDef ret;
  ret = HAL_I2C_Master_Receive(hi2c, device_address, data_pointer, size, time_out);
  __ASSERT((ret == HAL_OK), BSP_I2C_FAILED);
  return BSP_I2C_OK;
}

bsp_i2c_status_t bsp_i2c_master_transmit_it(bsp_i2c_handle_t *hi2c, uint16_t device_address,
                                            uint8_t *data_pointer, uint16_t size)
{
  // Check parameters
  __ASSERT((hi2c != NULL), BSP_I2C_ERROR);
  __ASSERT((data_pointer != NULL), BSP_I2C_ERROR);

  // Transmit
  HAL_StatusTypeDef ret;
  ret = HAL_I2C_Master_Transmit_IT(hi2c, device_address, data_pointer, size);
  __ASSERT((ret == HAL_OK), BSP_I2C_FAILED);
  return BSP_I2C_OK;
}

bsp_i2c_status_t bsp_i2c_master_receive_it(bsp_i2c_handle_t *hi2c, uint16_t device_address,
                                           uint8_t *data_buffer, uint16_t size)
{
  // Check parameters
  __ASSERT((hi2c != NULL), BSP_I2C_ERROR);
  __ASSERT((data_buffer != NULL), BSP_I2C_ERROR);

  // Receive
  HAL_StatusTypeDef ret;
  ret = HAL_I2C_Master_Receive_IT(hi2c, device_address, data_buffer, size);
  __ASSERT((ret == HAL_OK), BSP_I2C_FAILED);
  return BSP_I2C_OK;
}

bsp_i2c_status_t bsp_i2c_mem_write(bsp_i2c_handle_t *hi2c, uint16_t device_address, uint16_t mem_address,
                                   uint16_t mem_address_size, uint8_t *data_buffer, uint16_t size,
                                   uint32_t time_out)
{
  // Check parameters
  __ASSERT((hi2c != NULL), BSP_I2C_ERROR);
  __ASSERT((data_buffer != NULL), BSP_I2C_ERROR);

  // Write
  HAL_StatusTypeDef ret;
  ret = HAL_I2C_Mem_Write(hi2c, device_address, mem_address, mem_address_size, data_buffer, size, time_out);
  __ASSERT((ret == HAL_OK), BSP_I2C_FAILED);
  return BSP_I2C_OK;
}

bsp_i2c_status_t bsp_i2c_mem_read(bsp_i2c_handle_t *hi2c, uint16_t device_address, uint16_t mem_address,
                                  uint16_t mem_address_size, uint8_t *data_buffer, uint16_t size,
                                  uint32_t time_out)
{
  // Check parameters
  __ASSERT((hi2c != NULL), BSP_I2C_ERROR);
  __ASSERT((data_buffer != NULL), BSP_I2C_ERROR);

  // Read
  HAL_StatusTypeDef ret;
  ret = HAL_I2C_Mem_Read(hi2c, device_address, mem_address, mem_address_size, data_buffer, size, time_out);
  __ASSERT((ret == HAL_OK), BSP_I2C_FAILED);
  return BSP_I2C_OK;
}

bsp_i2c_status_t bsp_i2c_mem_write_it(bsp_i2c_handle_t *hi2c, uint16_t device_address, uint16_t mem_address,
                                      uint16_t mem_address_size, uint8_t *data_buffer, uint16_t size)
{
  // Check parameters
  __ASSERT((hi2c != NULL), BSP_I2C_ERROR);
  __ASSERT((data_buffer != NULL), BSP_I2C_ERROR);

  // Write
  HAL_StatusTypeDef ret;
  ret = HAL_I2C_Mem_Write_IT(hi2c, device_address, mem_address, mem_address_size, data_buffer, size);
  __ASSERT((ret == HAL_OK), BSP_I2C_FAILED);
  return BSP_I2C_OK;
}

bsp_i2c_status_t bsp_i2c_mem_read_it(bsp_i2c_handle_t *hi2c, uint16_t device_address, uint16_t mem_address,
                                     uint16_t mem_address_size, uint8_t *data_buffer, uint16_t size)
{
  // Check parameters
  __ASSERT((hi2c != NULL), BSP_I2C_ERROR);
  __ASSERT((data_buffer != NULL), BSP_I2C_ERROR);

  // Read
  HAL_StatusTypeDef ret;
  ret = HAL_I2C_Mem_Read_IT(hi2c, device_address, mem_address, mem_address_size, data_buffer, size);
  __ASSERT((ret == HAL_OK), BSP_I2C_FAILED);
  return BSP_I2C_OK;
}

/* Private definitions ------------------------------------------------ */

/* End of file -------------------------------------------------------- */
