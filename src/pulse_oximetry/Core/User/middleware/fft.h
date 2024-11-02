/**
 * @file       fft.h
 * @copyright
 * @license
 * @version    1.0.0
 * @date       14/10/2024
 * @author     Khanh Nguyen Ngoc
 *
 * @brief      FFT APIs
 *
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef USER_FFT_FFT_H_
#define USER_FFT_FFT_H_

/* Define to select arm math platform version ------------------------- */
#define ARM_MATH_CM4

/* Includes ----------------------------------------------------------- */
#include "arm_math.h"

/* Public defines ----------------------------------------------------- */
/**
 * @defgroup Defines related to FFT flag.
 * @brief    Flag to select Time domain to Frequency domain and vice versa.
 * @{
 */
#define FFT_FLAG_FORWARD (0) // Perform FFT (Time domain to Frequency domain)
#define FFT_FLAG_INVERSE (1) // Perform IFFT (Frequency domain to Time domain)
/**@} */

#define FFT_BUFFER_MAX_SIZE (256u)

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief FFT status enum definition.
 */
typedef enum
{
  FFT_STATUS_ERROR  = 0xFFFFFFFF,
  FFT_STATUS_FAILED = 0x7FFFFFFF,
  FFT_STATUS_OK     = 0x3FFFFFFF,
} fft_status_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief        Initialize FFT.
 *
 * @retval       FFT_STATUS_OK: if the function works correctly.
 */
fft_status_t fft_init();

/**
 * @brief        Calculate frequency of the peak value in input signal buffer.
 *
 * @param[in]    input_signal           Pointer to the input signal buffer.
 * @param[in]    sampling_frequency     Sampling frequency of the signal.
 *
 * @retval       The frequency of the peak value in input signal buffer.
 */
double fft_get_frequency_of_peak_value(double *input_signal, uint16_t sampling_frequency);

#endif /* USER_FFT_FFT_H_ */

/* End of file -------------------------------------------------------- */
