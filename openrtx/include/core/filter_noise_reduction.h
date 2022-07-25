/**
  ******************************************************************************
  * @file    filter_noise_reduction.h
  * @author  Giuseppe Callipo - IK8YFW - ik8yfw@libero.it
  * @version V1.0.0
  * @date    22-04-2018
  * @brief   Noise Reduction routines
  *
  *
  * NOTE: This file is part of RadioDSP project.
  *       See main.c file for additional project informations.
  ******************************************************************************/

#ifndef FILTER_NOISE_REDUCTION_H_INCLUDED
#define FILTER_NOISE_REDUCTION_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

#define BLOCK_SIZE		         256
#define FFT_SIZE                 256

uint8_t iCmdDnr = 1;

arm_lms_instance_f32  	Sl_LMS;

float32_t errOutput[BLOCK_SIZE];

/********************************************************************
 *
 *                   VARIABLES
 *
 ********************************************************************/

float32_t FFTBufferOut[BLOCK_SIZE*2];
float32_t FFTBufferTmp[BLOCK_SIZE*2];
float32_t FFTBufferMag[BLOCK_SIZE*2];
float32_t bufferPreOut[BLOCK_SIZE];

/********************************************************************
 *
 *                   DENOISE FNC
 *
 ********************************************************************/
void processing_noise_reduction(float* bufferIn, float* bufferOut2);


#ifdef __cplusplus
}
#endif

#endif /* FILTER_NOISE_REDUCTION_H_INCLUDED */

/**************************************END OF FILE****/
