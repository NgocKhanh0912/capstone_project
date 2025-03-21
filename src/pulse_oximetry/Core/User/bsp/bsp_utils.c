/**
 * @file       bsp_utils.c
 * @copyright
 * @license
 * @version    1.0.0
 * @date       31/07/2024
 * @author     Giang Phan Truong
 *             Khanh Nguyen Ngoc
 *             Viet Hoang Xuan
 *
 * @brief
 *
 * @note       None
 * @example    None
 */

/* Includes ----------------------------------------------------------- */
#include "bsp_utils.h"
#include "common.h"

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */
uint32_t bsp_utils_get_tick()
{
  return HAL_GetTick();
}

void bsp_utils_blocking_delay(uint32_t delay_time)
{
  HAL_Delay(delay_time);
}

/* Private definitions ----------------------------------------------- */

/* End of file -------------------------------------------------------- */
