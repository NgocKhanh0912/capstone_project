/**
 * @file       sys_measure.h
 * @copyright
 * @license
 * @version    v1.0.0
 * @date       2024-07-29
 * @author     Giang Phan Truong
 *             Khanh Nguyen Ngoc
 *             Viet Hoang Xuan
 *
 * @brief      None
 *
 * @note
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __USER_SYS_MEASURE_H
#define __USER_SYS_MEASURE_H

/* Includes ----------------------------------------------------------- */
#include "drv_hr.h"
#include "cbuffer.h"

/* Public defines ----------------------------------------------------- */
#define SYS_MEASURE_MAX_SAMPLES_PROCESS (256)

/* Public enumerate/structure ----------------------------------------- */
enum sys_measure_status_t
{
  SYS_MEASURE_ERROR  = 0xFFFFFFFF,
  SYS_MEASURE_FAILED = 0x7FFFFFFF,
  SYS_MEASURE_OK     = 0x3FFFFFFF
};

typedef struct
{
  drv_hr_t dev;
  cbuffer_t filtered_data;
  uint32_t heart_rate;
} sys_measure_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief  Initialize the system measurement.
 *
 * @param[in]     signal       The type of signal.
 * @param[in]     adc          ADC to convert analog signal to digital.
 * @param[in]     tim          Timer to trigerr ADC Conversion.
 * @param[in]     prescaler    Prescale the clock timer source.
 * @param[in]     autoreload   Set the top of counter.
 * @param[in]     data_buf     Filtered PPG buffer for cbuffer.
 *
 * @return
 *  - (0xFFFFFFFF): Error.
 *  - (0x7FFFFFFF): Failed.
 *  - (0x3FFFFFFF) : Success.
 */
uint32_t sys_measure_init(sys_measure_t *signal, bsp_adc_typedef_t *adc, bsp_tim_typedef_t *tim,
                          uint32_t prescaler, uint32_t autoreload, double *data_buf);

/**
 * @brief Process PPG data to measure heart rate.
 *
 * @param[in]         signal                 The signal need to be processed.
 * @param[inout]      gui_raw_ppg_cb         Pointer to the raw PPG cbuffer to stream on GUI.
 * @param[inout]      gui_filtered_ppg_cb    Pointer to the filtered PPG cbuffer to stream on GUI.
 * @return
 *
 *  - (0xFFFFFFFF): Error.
 *  - (0x7FFFFFFF): Failed.
 *  - (0x3FFFFFFF) : Success.
 */
uint32_t sys_measure_process_data(sys_measure_t *signal, cbuffer_t *gui_raw_ppg_cb,
                                  cbuffer_t *gui_filtered_ppg_cb);

#endif // __USER_SYS_MEASURE_H

/* End of file -------------------------------------------------------- */
