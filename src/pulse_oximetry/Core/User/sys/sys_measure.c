/**
 * @file       sys_measure.c
 * @copyright
 * @license
 * @version    v1.0.0
 * @date       2024-07-30
 * @author     Giang Phan Truong
 *             Khanh Nguyen Ngoc
 *             Viet Hoang Xuan
 *
 * @brief      None
 *
 * @note
 * @example    None
 */

/* Includes ----------------------------------------------------------- */
#include <math.h>
#include <float.h>
#include "sys_measure.h"
#include "common.h"
#include "fft.h"

#include "ai_datatypes_defines.h"
#include "ai_platform.h"
#include "peak_detection_model.h"
#include "peak_detection_model_data.h"

/* Private defines ---------------------------------------------------- */
/**
 * @defgroup Defines for configuring the peak detection method
 * @brief    Configuration options for selecting peak detection algorithm or model.
 * @{
 */
#define TERMA_ALGORITHM   (1)
#define DILATED_CNN_MODEL (2)

#define SYS_MEASURE_PEAK_DETECTOR (DILATED_CNN_MODEL)

#if SYS_MEASURE_PEAK_DETECTOR == TERMA_ALGORITHM
#define USE_ALGORITHM

/**
 * @defgroup Defines related to TERMA beta coefficient
 * @brief    Beta max, min value; calib beta step; beta init value
 * @{
 */
#define SYS_MEASURE_CALIB_BETA_STEP (0.05)
#define SYS_MEASURE_BETA_MAX        (1.2)
#define SYS_MEASURE_BETA_MIN        (0.005)
#define SYS_MEASURE_BETA_INIT_VALUE (0.095)
/**@} */

/**
 * @defgroup Defines related to TERMA window event
 * @brief    Window event and window cycle values
 * @{
 */
#define SYS_MEASURE_WINDOW_EVENT (9)
#define SYS_MEASURE_WINDOW_CYCLE ((SYS_MEASURE_WINDOW_EVENT * 6) + 1)
/**@} */

#elif SYS_MEASURE_PEAK_DETECTOR == DILATED_CNN_MODEL
#define USE_MODEL
#define SYS_MEASURE_PEAK_ACCEPT_THRESHOLD     (0.4)
#define SYS_MEASURE_PEAK_TOLERANCE            (0.3 * SYS_MEASURE_SAMPLING_RATE)

/**
 * @defgroup Defines related to PPG data normalization
 * @brief    Epsilon for data range, normalization max and min values.
 * @{
 */
#define SYS_MEASURE_NORMALIZE_PPG_EPSILON     (1e-6)
#define SYS_MEASURE_NORMALIZE_PPG_MAX         (1.0)
#define SYS_MEASURE_NORMALIZE_PPG_MIN         (-1.0)
/**@} */

/**
 * @defgroup Defines related to shape format
 * @brief    Batch, channels, width, height and size of input/output format.
 * @{
 */
#define SYS_MEASURE_MODEL_SHAPE_TYPE          (AI_SHAPE_BCWH)                      // Type BCWH
#define SYS_MEASURE_MODEL_SHAPE_SIZE          (4)                                  // B + C + W + H, size is 4
#define SYS_MEASURE_MODEL_SHAPE_BATCH         (1)                                  // Batch (B)
#define SYS_MEASURE_MODEL_SHAPE_CHANNELS      (1)                                  // Channels (C)
#define SYS_MEASURE_MODEL_SHAPE_WIDTH         (1)                                  // Width (W)
#define SYS_MEASURE_MODEL_SHAPE_INPUT_HEIGHT  (AI_PEAK_DETECTION_MODEL_IN_1_SIZE)  // Input shape height (H)
#define SYS_MEASURE_MODEL_SHAPE_OUTPUT_HEIGHT (AI_PEAK_DETECTION_MODEL_OUT_1_SIZE) // Output shape height (H)
/**@} */

#else
#error "Peak detector is not valid"
#endif
/**@} */

/**
 * @defgroup Defines related to filter
 * @brief    Filter number of coefficients
 * @{
 */
#define SYS_MEASURE_LPF_NUM_OF_COEFFS (5) // 4-order
#define SYS_MEASURE_HPF_NUM_OF_COEFFS (3) // 2-order
/**@} */

/**
 * @defgroup Defines related to numbers of peak in buffer
 * @brief    Numbers of maximum peaks and minimum peaks in the buffer
 * @{
 */
#define SYS_MEASURE_MAX_PEAK_IN_BUFFER (12) // 12 peak (300 bpm max)
#define SYS_MEASURE_MIN_PEAK_IN_BUFFER (2)
/**@} */

/**
 * @defgroup Defines related to stable index position threshold of peak in buffer
 * @brief    Peak stable position threshold at the begin and the end of the buffer
 * @{
 */
#define SYS_MEASURE_PEAK_STABLE_POSITION_THRESHOLD_AT_THE_BEGIN_OF_BUFFER (15)
#define SYS_MEASURE_PEAK_STABLE_POSITION_THRESHOLD_AT_THE_END_OF_BUFFER   (240)
/**@} */

#define SYS_MEASURE_SAMPLING_RATE                                       (100.0)
#define SYS_MEASURE_FILTERED_PPG_MIN_AMPLITUDE                          (100.0)
#define SYS_MEASURE_FILTERED_PPG_OFFSET                                 (1500.0)
#define SYS_MEASURE_CALIB_INTERVAL                                      (0.0065)
#define SYS_MEASURE_MAX_HEART_RATE_VARIABILITY_BETWEEN_TWO_MEASUREMENTS (20)
#define SYS_MEASURE_MAX_HEART_RATE_NUMBER_OF_INSTABILITY                (3)
#define SYS_MEASURE_FFT_HEART_RATE_RESOLUTION \
  (((double)SYS_MEASURE_SAMPLING_RATE / SYS_MEASURE_MAX_SAMPLES_PROCESS) * SECONDS_PER_MINUTE)

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */
static uint16_t s_adc_val_buf[SYS_MEASURE_MAX_SAMPLES_PROCESS + 1] = { 0 };

#if defined(USE_MODEL)
static ai_handle s_peak_detection_model = AI_HANDLE_NULL;

AI_ALIGNED(4) static ai_u8 s_activations[AI_PEAK_DETECTION_MODEL_DATA_ACTIVATIONS_SIZE];
AI_ALIGNED(4) static ai_float s_input_data[AI_PEAK_DETECTION_MODEL_IN_1_SIZE];
AI_ALIGNED(4) static ai_float s_output_data[AI_PEAK_DETECTION_MODEL_OUT_1_SIZE];

static ai_buffer ai_input =
  AI_BUFFER_INIT(AI_FLAG_NONE, AI_BUFFER_FORMAT_FLOAT,
                 AI_BUFFER_SHAPE_INIT(SYS_MEASURE_MODEL_SHAPE_TYPE, SYS_MEASURE_MODEL_SHAPE_SIZE,
                                      SYS_MEASURE_MODEL_SHAPE_BATCH, SYS_MEASURE_MODEL_SHAPE_CHANNELS,
                                      SYS_MEASURE_MODEL_SHAPE_WIDTH, SYS_MEASURE_MODEL_SHAPE_INPUT_HEIGHT),
                 AI_PEAK_DETECTION_MODEL_IN_1_SIZE, NULL, s_input_data);

static ai_buffer ai_output =
  AI_BUFFER_INIT(AI_FLAG_NONE, AI_BUFFER_FORMAT_FLOAT,
                 AI_BUFFER_SHAPE_INIT(SYS_MEASURE_MODEL_SHAPE_TYPE, SYS_MEASURE_MODEL_SHAPE_SIZE,
                                      SYS_MEASURE_MODEL_SHAPE_BATCH, SYS_MEASURE_MODEL_SHAPE_CHANNELS,
                                      SYS_MEASURE_MODEL_SHAPE_WIDTH, SYS_MEASURE_MODEL_SHAPE_OUTPUT_HEIGHT),
                 AI_PEAK_DETECTION_MODEL_OUT_1_SIZE, NULL, s_output_data);
#endif

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
#if defined(USE_MODEL)
/**
 * @brief  Init the AI Model.
 *
 * @return
 *
 *  - (0xFFFFFFFF): Error.
 *  - (0x7FFFFFFF): Failed.
 *  - (0x3FFFFFFF): Success.
 */
static uint32_t sys_measure_ai_model_init(void);

/**
 * @brief  Run the AI Model.
 *
 * @return
 *
 *  - (0xFFFFFFFF): Error.
 *  - (0x7FFFFFFF): Failed.
 *  - (0x3FFFFFFF): Success.
 */
static uint32_t sys_measure_ai_model_run(void);

/**
 * @brief  Normalize the filtered PPG signal.
 *
 * @param[inout]     ppg_data              The filtered PPG signal.
 * @param[in]        ppg_data_length       Length of the filtered PPG signal.
 * @param[in]        norm_max              Normalization max value.
 * @param[in]        norm_min              Normalization min value.
 *
 * @return
 *
 *  - (0xFFFFFFFF): Error.
 *  - (0x7FFFFFFF): Failed.
 *  - (0x3FFFFFFF): Success.
 */
static uint32_t sys_measure_normalize_ppg_data(double *ppg_data, uint16_t ppg_data_length, double norm_max,
                                               double norm_min);

/**
 * @brief  Filters out close peaks, retaining only the highest peak in each group.
 *
 * This function processes an array of detected peak indices and removes peaks that are
 * too close to each other based on a specified tolerance. In each group of close peaks,
 * only the peak with the highest signal value is retained.
 *
 * @param[inout]     peak_indices       Array of detected peak indices (modified after filtering).
 * @param[inout]     num_peaks          Pointer to the number of peaks (updated after filtering).
 * @param[in]        signal             Array containing PPG signal data.
 * @param[in]        tolerance          Minimum distance between peaks to consider them separate.
 *
 * @return
 *
 *  - (0xFFFFFFFF): Error.
 *  - (0x7FFFFFFF): Failed.
 *  - (0x3FFFFFFF): Success.
 */
static uint32_t sys_measure_filter_close_peaks(uint32_t *peak_indices, uint32_t *num_peaks, double *signal,
                                               uint32_t tolerance);
#endif

/**
 * @brief  Filter the interferances of the signal.
 *
 * @param[inout]     signal                The signal object.
 * @param[inout]     gui_raw_ppg_cb        Pointer to the raw PPG cbuffer to stream on GUI.
 * @param[inout]     gui_filtered_ppg_cb   Pointer to the filtered PPG cbuffer to stream on GUI.
 *
 * @return
 *
 *  - (0xFFFFFFFF): Error
 *  - (0x7FFFFFFF): Failed
 *  - (0x3FFFFFFF): Success
 */
static uint32_t sys_measure_filter_data(sys_measure_t *signal, cbuffer_t *gui_raw_ppg_cb,
                                        cbuffer_t *gui_filtered_ppg_cb);

/**
 * @brief  Detect the peak in dataset of signal.
 *
 * @param[in]     signal  The signal object.
 *
 * @return
 *
 *  - (0xFFFFFFFF): Error
 *  - (0x7FFFFFFF): Failed
 *  - (0x3FFFFFFF): Success
 */
static uint32_t sys_measure_peak_detector(sys_measure_t *signal);

/* Function definitions ----------------------------------------------- */
#if defined(USE_MODEL)
static uint32_t sys_measure_ai_model_init(void)
{
  // Create model
  ai_error model_create_ret =
    ai_peak_detection_model_create(&s_peak_detection_model, AI_PEAK_DETECTION_MODEL_DATA_CONFIG);
  __ASSERT(model_create_ret.type == AI_ERROR_NONE, SYS_MEASURE_ERROR);

  const ai_network_params params = { .params = AI_PEAK_DETECTION_MODEL_DATA_WEIGHTS(
                                       ai_peak_detection_model_data_weights_get()),
                                     .activations = AI_PEAK_DETECTION_MODEL_DATA_ACTIVATIONS(s_activations) };

  // Init model
  ai_bool model_init_ret = ai_peak_detection_model_init(s_peak_detection_model, &params);
  __ASSERT(model_init_ret == true, SYS_MEASURE_FAILED);

  return SYS_MEASURE_OK;
}

static uint32_t sys_measure_ai_model_run(void)
{
  __ASSERT(s_peak_detection_model != AI_HANDLE_NULL, SYS_MEASURE_ERROR);

  ai_i32 batch = ai_peak_detection_model_run(s_peak_detection_model, &ai_input, &ai_output);
  __ASSERT(batch == 1, SYS_MEASURE_FAILED);

  return SYS_MEASURE_OK;
}

static uint32_t sys_measure_normalize_ppg_data(double *ppg_data, uint16_t ppg_data_length, double norm_max,
                                               double norm_min)
{
  __ASSERT(ppg_data != NULL, SYS_MEASURE_ERROR);
  __ASSERT(ppg_data_length != 0, SYS_MEASURE_ERROR);
  __ASSERT(norm_max >= norm_min, SYS_MEASURE_ERROR);

  double ppg_min = DBL_MAX;
  double ppg_max = -DBL_MAX;

  // Find max and min of PPG data
  for (uint_fast16_t i = 0; i < ppg_data_length; i++)
  {
    if (ppg_data[i] < ppg_min)
    {
      ppg_min = ppg_data[i];
    }

    if (ppg_data[i] > ppg_max)
    {
      ppg_max = ppg_data[i];
    }
  }

  // No actual PPG signal, return
  if (ppg_max <= SYS_MEASURE_FILTERED_PPG_MIN_AMPLITUDE)
  {
    return SYS_MEASURE_FAILED;
  }

  double range = ppg_max - ppg_min;
  __ASSERT(range >= SYS_MEASURE_NORMALIZE_PPG_EPSILON, SYS_MEASURE_FAILED);

  double scale = (norm_max - norm_min) / range;

  // Normalize PPG data
  for (uint_fast16_t i = 0; i < ppg_data_length; i++)
  {
    ppg_data[i] = norm_min + (ppg_data[i] - ppg_min) * scale;
  }

  return SYS_MEASURE_OK;
}

static uint32_t sys_measure_filter_close_peaks(uint32_t *peak_indices, uint32_t *num_peaks, double *signal,
                                               uint32_t tolerance)
{
  __ASSERT(peak_indices != NULL, SYS_MEASURE_ERROR);
  __ASSERT(num_peaks != NULL, SYS_MEASURE_ERROR);
  __ASSERT(*num_peaks != 0, SYS_MEASURE_ERROR);
  __ASSERT(signal != NULL, SYS_MEASURE_ERROR);

  uint32_t filtered_peaks[AI_PEAK_DETECTION_MODEL_OUT_1_SIZE] = { 0 };
  uint32_t filtered_count                                     = 0;

  // Temporary group for close peaks
  uint32_t current_group[AI_PEAK_DETECTION_MODEL_OUT_1_SIZE] = { 0 };
  uint32_t group_size                                        = 0;

  // Initialize the first group with the first peak
  current_group[group_size++] = peak_indices[0];

  for (uint32_t i = 1; i < *num_peaks; i++)
  {
    // If the peak is within the tolerance range, add it to the current group
    if (peak_indices[i] - current_group[group_size - 1] <= tolerance)
    {
      current_group[group_size++] = peak_indices[i];
    }
    else
    {
      // Select the highest peak in the current group
      uint32_t best_peak = current_group[0];
      double max_value   = signal[best_peak];

      for (uint32_t j = 1; j < group_size; j++)
      {
        if (signal[current_group[j]] > max_value)
        {
          best_peak = current_group[j];
          max_value = signal[best_peak];
        }
      }

      filtered_peaks[filtered_count++] = best_peak;

      // Start a new group with the current peak
      group_size                  = 0;
      current_group[group_size++] = peak_indices[i];
    }
  }

  // Process the last group
  if (group_size > 0)
  {
    uint32_t best_peak = current_group[0];
    double max_value   = signal[best_peak];

    for (uint32_t j = 1; j < group_size; j++)
    {
      if (signal[current_group[j]] > max_value)
      {
        best_peak = current_group[j];
        max_value = signal[best_peak];
      }
    }

    filtered_peaks[filtered_count++] = best_peak;
  }

  // Copy the filtered peaks back to the input array, update the number of peaks after filtering
  for (uint32_t i = 0; i < filtered_count; i++)
  {
    peak_indices[i] = filtered_peaks[i];
  }

  *num_peaks = filtered_count;

  return SYS_MEASURE_OK;
}
#endif

uint32_t sys_measure_init(sys_measure_t *signal, bsp_adc_typedef_t *adc, bsp_tim_typedef_t *tim,
                          uint32_t prescaler, uint32_t autoreload, double *data_buf)
{
  __ASSERT(signal != NULL, SYS_MEASURE_ERROR);
  __ASSERT(adc != NULL, SYS_MEASURE_ERROR);
  __ASSERT(tim != NULL, SYS_MEASURE_ERROR);
  __ASSERT(data_buf != NULL, SYS_MEASURE_ERROR);

  uint32_t ret;

  ret = cb_init(&(signal->dev.adc_conv), s_adc_val_buf, sizeof(s_adc_val_buf));
  __ASSERT(ret == CB_STATUS_OK, SYS_MEASURE_FAILED);

  ret = cb_init(&(signal->filtered_data), data_buf, (SYS_MEASURE_MAX_SAMPLES_PROCESS + 1) * sizeof(double));
  __ASSERT(ret == CB_STATUS_OK, SYS_MEASURE_FAILED);

  signal->heart_rate = 0;
  ret                = drv_hr_init(&(signal->dev), adc, tim, prescaler, autoreload);
  __ASSERT(ret == DRV_HR_OK, SYS_MEASURE_FAILED);

#if defined(USE_ALGORITHM)
  ret = fft_init();
  __ASSERT(ret == FFT_STATUS_OK, SYS_MEASURE_FAILED);
#elif defined(USE_MODEL)
  ret = sys_measure_ai_model_init();
  __ASSERT(ret == SYS_MEASURE_OK, SYS_MEASURE_FAILED);
#endif

  return SYS_MEASURE_OK;
}

uint32_t sys_measure_process_data(sys_measure_t *signal, cbuffer_t *gui_raw_ppg_cb,
                                  cbuffer_t *gui_filtered_ppg_cb)
{
  __ASSERT(signal != NULL, SYS_MEASURE_ERROR);
  __ASSERT(signal->dev.active == true, SYS_MEASURE_ERROR);

  sys_measure_filter_data(signal, gui_raw_ppg_cb, gui_filtered_ppg_cb);

  if (cb_space_count(&signal->filtered_data) == sizeof(double) - 1)
  {
    sys_measure_peak_detector(signal);
  }

  return SYS_MEASURE_OK;
}

/* Private definitions ------------------------------------------------ */
static uint32_t sys_measure_filter_data(sys_measure_t *signal, cbuffer_t *gui_raw_ppg_cb,
                                        cbuffer_t *gui_filtered_ppg_cb)
{
  __ASSERT(signal != NULL, SYS_MEASURE_ERROR);
  __ASSERT(gui_raw_ppg_cb != NULL, SYS_MEASURE_ERROR);
  __ASSERT(gui_filtered_ppg_cb != NULL, SYS_MEASURE_ERROR);

  // Coeffi in z-domain

  // LPF params:
  // fs = 100.0
  // ws = 12
  // wp = 3
  // wc = 4
  // order = 4
  const double lpf_numerator_z[SYS_MEASURE_LPF_NUM_OF_COEFFS] = { 0.000177296607979, 0.000709186431917,
                                                                  0.001063779647875, 0.000709186431917,
                                                                  0.000177296607979 };

  const double lpf_denominator_z[SYS_MEASURE_LPF_NUM_OF_COEFFS] = { 1, -3.349831562667920, 4.252610698953553,
                                                                    -2.420450670140820, 0.520508279582855 };

  // HPF params:
  // fs = 100.0
  // ws = 0.0005
  // wp = 0.01
  // wc = 0.005
  // order = 2
  const double hpf_numerator_z[SYS_MEASURE_HPF_NUM_OF_COEFFS] = { 0.999777886079662, -1.999555772159325,
                                                                  0.999777886079662 };

  const double hpf_denominator_z[SYS_MEASURE_HPF_NUM_OF_COEFFS] = { 1, -1.999555722824731,
                                                                    0.999555821493919 };

  while (cb_data_count(&(signal->dev.adc_conv)) != 0)
  {
    // Apply LPF
    static double lpf_recent_input[SYS_MEASURE_LPF_NUM_OF_COEFFS]  = { 0 };
    static double lpf_recent_output[SYS_MEASURE_LPF_NUM_OF_COEFFS] = { 0 };

    // Shift lpf recent value to the right
    for (int i = SYS_MEASURE_LPF_NUM_OF_COEFFS - 1; i > 0; --i)
    {
      lpf_recent_input[i]  = lpf_recent_input[i - 1];
      lpf_recent_output[i] = lpf_recent_output[i - 1];
    }

    // Put the current value of the input signal into the first position of the array
    uint16_t adc_temp;
    cb_read(&(signal->dev.adc_conv), &adc_temp, sizeof(adc_temp));
    cb_write(gui_raw_ppg_cb, &adc_temp, sizeof(adc_temp));
    lpf_recent_input[0] = (double)adc_temp;

    // Calculate the current output value
    lpf_recent_output[0] = lpf_numerator_z[0] * lpf_recent_input[0];

    for (int j = 1; j < SYS_MEASURE_LPF_NUM_OF_COEFFS; ++j)
    {
      lpf_recent_output[0] += lpf_numerator_z[j] * lpf_recent_input[j];
    }

    for (int j = 1; j < SYS_MEASURE_LPF_NUM_OF_COEFFS; ++j)
    {
      lpf_recent_output[0] -= lpf_denominator_z[j] * lpf_recent_output[j];
    }

    // Apply HPF
    static double hpf_recent_output[SYS_MEASURE_HPF_NUM_OF_COEFFS] = { 0 };
    // lpf output is the hpf input, dont need to shift it because it's shifted in lpf
    // Shift hpf recent output to the right
    for (int i = SYS_MEASURE_HPF_NUM_OF_COEFFS - 1; i > 0; --i)
    {
      hpf_recent_output[i] = hpf_recent_output[i - 1];
    }

    // Calculate the current output value
    hpf_recent_output[0] = hpf_numerator_z[0] * lpf_recent_output[0];

    for (int j = 1; j < SYS_MEASURE_HPF_NUM_OF_COEFFS; ++j)
    {
      hpf_recent_output[0] += hpf_numerator_z[j] * lpf_recent_output[j];
    }

    for (int j = 1; j < SYS_MEASURE_HPF_NUM_OF_COEFFS; ++j)
    {
      hpf_recent_output[0] -= hpf_denominator_z[j] * hpf_recent_output[j];
    }

    // Place the current output value at the first position of the array
    if (cb_space_count(&(signal->filtered_data)) >= sizeof(double))
    {
      cb_write(&(signal->filtered_data), &hpf_recent_output[0], sizeof(hpf_recent_output[0]));
    }

    if (cb_space_count(gui_filtered_ppg_cb) >= sizeof(double))
    {
      cb_write(gui_filtered_ppg_cb, &hpf_recent_output[0], sizeof(hpf_recent_output[0]));
    }
  }
  return SYS_MEASURE_OK;
}

static uint32_t sys_measure_peak_detector(sys_measure_t *signal)
{
  __ASSERT(signal != NULL, SYS_MEASURE_ERROR);

  double handle_data[SYS_MEASURE_MAX_SAMPLES_PROCESS] = { 0 };
  cbuffer_t peak_detector_cbuf                        = signal->filtered_data;
  cb_read(&peak_detector_cbuf, handle_data, sizeof(handle_data));

#if defined(USE_ALGORITHM)
  // Choose the beta and Windows Size W1, W2 in TERMA framework
  static int w_cycle = SYS_MEASURE_WINDOW_CYCLE;
  static int w_evt   = SYS_MEASURE_WINDOW_EVENT;
  static double beta = SYS_MEASURE_BETA_INIT_VALUE;

  double ma_cycle[SYS_MEASURE_MAX_SAMPLES_PROCESS] = { 0 }, ma_evt[SYS_MEASURE_MAX_SAMPLES_PROCESS] = { 0 };

  double mean_of_signal = 0;
  int i, j;

  double fft_input_data[SYS_MEASURE_MAX_SAMPLES_PROCESS] = { 0 };
  memcpy(fft_input_data, handle_data, sizeof(fft_input_data));
  double fft_heart_rate =
    (SECONDS_PER_MINUTE * fft_get_frequency_of_peak_value(fft_input_data, SYS_MEASURE_SAMPLING_RATE));

  double ppg_max = -DBL_MAX;

  // Enhance the signal
  for (i = 0; i < SYS_MEASURE_MAX_SAMPLES_PROCESS; i++)
  {
    if (handle_data[i] > ppg_max)
    {
      ppg_max = handle_data[i];
    }

    handle_data[i] = pow(handle_data[i] + SYS_MEASURE_FILTERED_PPG_OFFSET, 2);
  }

  // No actual PPG signal, return
  if (ppg_max <= SYS_MEASURE_FILTERED_PPG_MIN_AMPLITUDE)
  {
    signal->heart_rate = 0;
    return SYS_MEASURE_FAILED;
  }

  // Calculate the Event Duration Moving Average
  for (i = ((w_evt - 1) / 2); i < __SIZE_OF(handle_data) - ((w_evt - 1) / 2); i++)
  {
    for (j = -((w_evt - 1) / 2); j < ((w_evt - 1) / 2); j++)
    {
      ma_evt[i] += handle_data[i + j];
    }
    ma_evt[i] /= w_evt;
  }

  // Calculate the Event Cycle Moving Average
  for (i = ((w_cycle - 1) / 2); i < __SIZE_OF(handle_data) - ((w_cycle - 1) / 2); i++)
  {
    for (j = -((w_cycle - 1) / 2); j < ((w_evt - 1) / 2); j++)
    {
      ma_cycle[i] += handle_data[i + j];
    }
    ma_cycle[i] /= w_cycle;
  }

  // Calculate the mean of signal
  for (i = 0; i < SYS_MEASURE_MAX_SAMPLES_PROCESS; i++)
  {
    mean_of_signal += handle_data[i];
  }
  mean_of_signal /= SYS_MEASURE_MAX_SAMPLES_PROCESS;

  // Calculate the Threshold for generating Block of Interest
  double threshold[SYS_MEASURE_MAX_SAMPLES_PROCESS] = { 0 };

  for (i = 0; i < SYS_MEASURE_MAX_SAMPLES_PROCESS; i++)
  {
    threshold[i] = ma_cycle[i] + beta * mean_of_signal;
  }

  // Generate the Block of Interest
  uint8_t block_of_interest[SYS_MEASURE_MAX_SAMPLES_PROCESS] = { 0 };
  for (i = 0; i < SYS_MEASURE_MAX_SAMPLES_PROCESS; i++)
  {
    if (ma_evt[i] > threshold[i])
    {
      block_of_interest[i] = 1;
    }
    else
    {
      block_of_interest[i] = 0;
    }
  }

  // Peak detector
  uint32_t pos_start_block      = 0;
  uint32_t pos_stop_block       = 0;
  uint32_t is_block_of_interest = 0;

  double peak_amplitude                                   = 0;
  uint32_t peak_index                                     = 0;
  uint32_t peak_nums                                      = 0;
  uint32_t peak_index_buf[SYS_MEASURE_MAX_PEAK_IN_BUFFER] = { 0 };

  double heart_rate                        = 0;
  static uint8_t unstable_heart_rate_count = 0;
  static double previous_heart_rate        = 0;

  for (i = 0; i < SYS_MEASURE_MAX_SAMPLES_PROCESS - 1; i++)
  {
    if ((block_of_interest[i + 1] - block_of_interest[i]) == 1)
    {
      pos_start_block      = i;
      is_block_of_interest = 1;
    }
    if (((block_of_interest[i] - block_of_interest[i + 1]) == 1) && (is_block_of_interest == 1))
    {
      pos_stop_block = i;
      if (pos_stop_block - pos_start_block >= w_evt)
      {
        peak_amplitude = handle_data[pos_start_block];
        for (i = pos_start_block; i <= pos_stop_block; i++)
        {
          if (handle_data[i] > peak_amplitude)
          {
            peak_amplitude = handle_data[i];
            peak_index     = i;
          }
        }
        peak_index_buf[peak_nums] = peak_index;
        peak_nums++;
      }
      is_block_of_interest = 0;
    }
  }

  // Dynamic beta calibration
  if (peak_nums < SYS_MEASURE_MIN_PEAK_IN_BUFFER)
  {
    double decrease_beta = beta - SYS_MEASURE_CALIB_BETA_STEP;
    if (decrease_beta >= SYS_MEASURE_BETA_MIN)
    {
      beta = decrease_beta;
      return SYS_MEASURE_FAILED;
    }
  }
  else
  {
    if (peak_nums > SYS_MEASURE_MAX_PEAK_IN_BUFFER)
    {
      double increase_beta = beta + SYS_MEASURE_CALIB_BETA_STEP;
      if (increase_beta <= SYS_MEASURE_BETA_MAX)
      {
        beta = increase_beta;
        return SYS_MEASURE_FAILED;
      }
    }

    // Choose stable peak and calculate the peak interval (in second unit)
    if (peak_nums >= 4)
    {
      heart_rate = peak_index_buf[2] - peak_index_buf[1];
      __ASSERT(heart_rate > 0, SYS_MEASURE_FAILED);
    }
    else if (peak_nums == 3)
    {
      if (peak_index_buf[0] >= SYS_MEASURE_PEAK_STABLE_POSITION_THRESHOLD_AT_THE_BEGIN_OF_BUFFER)
      {
        heart_rate = peak_index_buf[1] - peak_index_buf[0];
        __ASSERT(heart_rate > 0, SYS_MEASURE_FAILED);
      }
      else if (peak_index_buf[2] <= SYS_MEASURE_PEAK_STABLE_POSITION_THRESHOLD_AT_THE_END_OF_BUFFER)
      {
        heart_rate = peak_index_buf[2] - peak_index_buf[1];
        __ASSERT(heart_rate > 0, SYS_MEASURE_FAILED);
      }
      else
      {
        return SYS_MEASURE_FAILED;
      }
    }
    else
    {
      if ((peak_index_buf[0] >= SYS_MEASURE_PEAK_STABLE_POSITION_THRESHOLD_AT_THE_BEGIN_OF_BUFFER) &&
          (peak_index_buf[1] <= SYS_MEASURE_PEAK_STABLE_POSITION_THRESHOLD_AT_THE_END_OF_BUFFER))
      {
        heart_rate = peak_index_buf[1] - peak_index_buf[0];
        __ASSERT(heart_rate > 0, SYS_MEASURE_FAILED);
      }
      else
      {
        return SYS_MEASURE_FAILED;
      }
    }

    heart_rate *= (1 / SYS_MEASURE_SAMPLING_RATE);

    // Calibration
    heart_rate -= SYS_MEASURE_CALIB_INTERVAL;

    // Estimate the heart rate (beats per minute unit)
    heart_rate = SECONDS_PER_MINUTE / heart_rate;
    __ASSERT(((heart_rate >= HEART_RATE_MIN) && (heart_rate <= HEART_RATE_MAX)), SYS_MEASURE_FAILED);

    // Remove noisy heart rate signals caused by unstable sensor contact using FFT
    if (((uint32_t)fft_heart_rate >= HEART_RATE_MIN) && ((uint32_t)fft_heart_rate <= HEART_RATE_MAX))
    {
      if (fabs(heart_rate - fft_heart_rate) > SYS_MEASURE_FFT_HEART_RATE_RESOLUTION)
      {
        return SYS_MEASURE_FAILED;
      }
    }

    // Retain heart rate results when the user removes the sensor or changes the wearer of the device
    if (previous_heart_rate != 0)
    {
      if (fabs(heart_rate - previous_heart_rate) >=
          SYS_MEASURE_MAX_HEART_RATE_VARIABILITY_BETWEEN_TWO_MEASUREMENTS)
      {
        unstable_heart_rate_count += 1;
        if (unstable_heart_rate_count < SYS_MEASURE_MAX_HEART_RATE_NUMBER_OF_INSTABILITY)
        {
          return SYS_MEASURE_FAILED;
        }
      }
    }

    unstable_heart_rate_count = 0;
    previous_heart_rate       = heart_rate;
    signal->heart_rate        = (uint32_t)heart_rate;
  }
#elif defined(USE_MODEL)
  uint32_t ret;

  ret = sys_measure_normalize_ppg_data(handle_data, SYS_MEASURE_MAX_SAMPLES_PROCESS,
                                       SYS_MEASURE_NORMALIZE_PPG_MAX, SYS_MEASURE_NORMALIZE_PPG_MIN);
  if (ret == SYS_MEASURE_FAILED)
  {
    signal->heart_rate = 0;
    return SYS_MEASURE_FAILED;
  }

  // Move normalized data to AI input buffer
  for (uint_fast16_t i = 0; i < AI_PEAK_DETECTION_MODEL_IN_1_SIZE; i++)
  {
    s_input_data[i] = (ai_float)handle_data[i];
  }

  ret = sys_measure_ai_model_run();
  __ASSERT(ret == SYS_MEASURE_OK, SYS_MEASURE_FAILED);

  uint32_t peak_nums                                          = 0;
  uint32_t peak_index_buf[AI_PEAK_DETECTION_MODEL_OUT_1_SIZE] = { 0 };

  // Find peaks
  for (uint_fast16_t i = 0; i < AI_PEAK_DETECTION_MODEL_OUT_1_SIZE; i++)
  {
    if (s_output_data[i] > SYS_MEASURE_PEAK_ACCEPT_THRESHOLD)
    {
      peak_index_buf[peak_nums] = i;
      peak_nums++;
    }
  }

  // Filter close peaks
  ret = sys_measure_filter_close_peaks(peak_index_buf, &peak_nums, handle_data, SYS_MEASURE_PEAK_TOLERANCE);
  __ASSERT(ret == SYS_MEASURE_OK, SYS_MEASURE_FAILED);

  double heart_rate                        = 0;
  static uint8_t unstable_heart_rate_count = 0;
  static double previous_heart_rate        = 0;

  // Choose stable peak and calculate the peak interval (in second unit)
  if (peak_nums >= 4)
  {
    heart_rate = peak_index_buf[2] - peak_index_buf[1];
    __ASSERT(heart_rate > 0, SYS_MEASURE_FAILED);
  }
  else if (peak_nums == 3)
  {
    if (peak_index_buf[0] >= SYS_MEASURE_PEAK_STABLE_POSITION_THRESHOLD_AT_THE_BEGIN_OF_BUFFER)
    {
      heart_rate = peak_index_buf[1] - peak_index_buf[0];
      __ASSERT(heart_rate > 0, SYS_MEASURE_FAILED);
    }
    else if (peak_index_buf[2] <= SYS_MEASURE_PEAK_STABLE_POSITION_THRESHOLD_AT_THE_END_OF_BUFFER)
    {
      heart_rate = peak_index_buf[2] - peak_index_buf[1];
      __ASSERT(heart_rate > 0, SYS_MEASURE_FAILED);
    }
    else
    {
      return SYS_MEASURE_FAILED;
    }
  }
  else
  {
    if ((peak_index_buf[0] >= SYS_MEASURE_PEAK_STABLE_POSITION_THRESHOLD_AT_THE_BEGIN_OF_BUFFER) &&
        (peak_index_buf[1] <= SYS_MEASURE_PEAK_STABLE_POSITION_THRESHOLD_AT_THE_END_OF_BUFFER))
    {
      heart_rate = peak_index_buf[1] - peak_index_buf[0];
      __ASSERT(heart_rate > 0, SYS_MEASURE_FAILED);
    }
    else
    {
      return SYS_MEASURE_FAILED;
    }
  }

  heart_rate *= (1 / SYS_MEASURE_SAMPLING_RATE);

  // Calibration
  heart_rate -= SYS_MEASURE_CALIB_INTERVAL;

  // Estimate the heart rate (beats per minute unit)
  heart_rate = SECONDS_PER_MINUTE / heart_rate;
  __ASSERT(((heart_rate >= HEART_RATE_MIN) && (heart_rate <= HEART_RATE_MAX)), SYS_MEASURE_FAILED);

  // Retain heart rate results when the user removes the sensor or changes the wearer of the device
  if (previous_heart_rate != 0)
  {
    if (fabs(heart_rate - previous_heart_rate) >=
        SYS_MEASURE_MAX_HEART_RATE_VARIABILITY_BETWEEN_TWO_MEASUREMENTS)
    {
      unstable_heart_rate_count += 1;
      if (unstable_heart_rate_count < SYS_MEASURE_MAX_HEART_RATE_NUMBER_OF_INSTABILITY)
      {
        return SYS_MEASURE_FAILED;
      }
    }
  }

  unstable_heart_rate_count = 0;
  previous_heart_rate       = heart_rate;
  signal->heart_rate        = (uint32_t)heart_rate;
#endif

  return SYS_MEASURE_OK;
}

/* End of file -------------------------------------------------------- */
