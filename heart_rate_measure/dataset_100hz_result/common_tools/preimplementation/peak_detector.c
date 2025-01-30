/**
 * @file       peak_detector.c
 * @copyright  Copyright (C) 2019 ITRVN. All rights reserved.
 *
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

/* Includes ----------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* Private defines ---------------------------------------------------- */
#define MAX_SAMPLES     (5000)
#define MAX_LINE_LENGTH (100)

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */
const char *data_file = "golden_ppg_data_100hz.csv";
double sample[MAX_SAMPLES] = {0};

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
static void read_csv_file(const char *file_name, double *csv_buf, int samples);
static uint32_t peak_detector(double *input);

/* Function definitions ----------------------------------------------- */
int main(void)
{
  read_csv_file(data_file, sample, MAX_SAMPLES);

  // print total number of peaks
  printf("%d\n", peak_detector(sample));
  
  return 0;
}

/* Private definitions ------------------------------------------------ */
static void read_csv_file(const char *file_name, double *csv_buf, int samples) 
{
  FILE *sample_file = fopen(file_name, "r");
  if (!sample_file) 
  {
    printf("Could not open file\n");
    return;
  }

  char temp[MAX_LINE_LENGTH];
  int count = 0;

  while (count < samples && fgets(temp, MAX_LINE_LENGTH, sample_file)) 
  {
    // Assuming the CSV has no header and each line contains a single double value
    char *endptr;
    double value = strtod(temp, &endptr);

    // Check if the conversion was successful and if we reached a valid number
    if (endptr != temp) 
    {
      csv_buf[count++] = value;
    }
  }

  fclose(sample_file);
}

static uint32_t peak_detector(double *input)
{
  // Choose the Windows Size W1, W2 in TERMA framework
  uint32_t w_cycle = 55,
           w_evt = 9;

  float ma_cycle[MAX_SAMPLES] = {0},
        ma_evt[MAX_SAMPLES] = {0};

  float mean_of_signal = 0;
  int i, j, k;

  // Enhance the signal
  for (i = 0; i < MAX_SAMPLES; i++)
  {
    input[i] = pow(input[i], 2);
  }

  // Calculate the Event Duration Moving Average
  k = ((w_evt - 1) / 2);
  for (i = ((w_evt - 1) / 2); i < MAX_SAMPLES - ((w_evt - 1) / 2); i++)
  {
    for (j = -((w_evt - 1) / 2); j < k; j++)
    {
      ma_evt[i] += input[i + j];
    }
    ma_evt[i] /= w_evt;
  }

  // Calculate the Event Cycle Moving Average
  k = ((w_cycle - 1) / 2);
  for (i = ((w_cycle - 1) / 2); i < MAX_SAMPLES - ((w_cycle - 1) / 2); i++)
  {
    for (j = -((w_cycle - 1) / 2); j < k; j++)
    {
      ma_cycle[i] += input[i + j];
    }
    ma_cycle[i] /= w_cycle;
  }

  // Calculate the mean of signal
  for (i = 0; i < MAX_SAMPLES; i++)
  {
    mean_of_signal += input[i];
  }

  mean_of_signal /= MAX_SAMPLES;

  // Calculate the Threshold for generating Block of Interest
  float beta = 0.095;
  float threshold_1[MAX_SAMPLES] = {0};

  for (i = 0; i < MAX_SAMPLES; i++)
  {
    threshold_1[i] = ma_cycle[i] + beta * mean_of_signal;
  }

  // Generate the Bloock of Interest
  uint8_t block_of_interest[MAX_SAMPLES] = {0};
  for (i = 0; i < MAX_SAMPLES; i++)
  {
    if (ma_evt[i] > threshold_1[i])
    {
      block_of_interest[i] = 1;
    }
    else
    {
      block_of_interest[i] = 0;
    }
  }

  // Peak detector
  uint32_t pos_start_block = 0;
  uint32_t pos_stop_block = 0;
  uint32_t peak_num = 0;
  for (i = 0; i < MAX_SAMPLES - 1; i++)
  {
    if ((block_of_interest[i + 1] - block_of_interest[i]) == 1)
    {
      pos_start_block = i;
    }
    else if ((block_of_interest[i] - block_of_interest[i + 1]) == 1)
    {
      pos_stop_block = i;
      if (pos_stop_block - pos_start_block >= w_evt)
      {
        /// can calculate peak value to consider histogram
        peak_num++;
      }
    }
  }
  return peak_num;
}

/* End of file -------------------------------------------------------- */
