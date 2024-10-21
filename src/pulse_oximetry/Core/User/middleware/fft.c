/**
 * @file       fft.c
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

/* Includes ----------------------------------------------------------- */
#include "common.h"
#include "fft.h"
#include <math.h>

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
static arm_rfft_fast_instance_f32 fft_handler;

/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */
fft_status_t fft_init()
{
  arm_rfft_fast_init_f32(&fft_handler, FFT_BUFFER_MAX_SIZE);

  return FFT_STATUS_OK;
}

double fft_get_frequency_of_peak_value(double *input_signal, uint16_t sampling_frequency)
{
  float fft_input_buffer[FFT_BUFFER_MAX_SIZE]  = { 0 };
  float fft_output_buffer[FFT_BUFFER_MAX_SIZE] = { 0 };

  // Fill the fft input buffer
  for (uint16_t i = 0; i < FFT_BUFFER_MAX_SIZE; i++)
  {
    fft_input_buffer[i] = input_signal[i];
  }

  // Perform FFT
  arm_rfft_fast_f32(&fft_handler, fft_input_buffer, fft_output_buffer, FFT_FLAG_FORWARD);

  float peak_value               = 0;
  double frequency_of_peak_value = 0;
  uint16_t frequency_index       = 0;

  for (uint16_t i = 0; i < FFT_BUFFER_MAX_SIZE; i += 2)
  {
    float current_value = sqrtf((fft_output_buffer[i] * fft_output_buffer[i]) +
                                (fft_output_buffer[i + 1] * fft_output_buffer[i + 1]));

    if (current_value > peak_value)
    {
      peak_value = current_value;

      // Mapping frequency to bin
      frequency_of_peak_value = (frequency_index * sampling_frequency) / ((double)FFT_BUFFER_MAX_SIZE);
    }
    frequency_index++;
  }

  return frequency_of_peak_value;
}

/* End of file -------------------------------------------------------- */