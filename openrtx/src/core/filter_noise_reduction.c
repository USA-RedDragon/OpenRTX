/**
  ******************************************************************************
  * @file    filter_noise_reduction.h
  * @author  Giuseppe Callipo - IK8YFW - ik8yfw@libero.it
  * @version V2.0.0
  * @date    25-09-2018
  * @brief   IO Routine
  *
  *
  * NOTE: This file is part of RadioDSP project.
  *       See main.c file for additional project informations.
  * Platform: CortexM4
  ******************************************************************************/

#include <arm_math.h>
#include <arm_const_structs.h>
#include <math.h>

#include <stdio.h>
#include <complex.h>

#include <filter_noise_reduction.h>

/*- working variables */
int     energyChkPeriod_idx = 0;
float   TH_VALUE            = 0.0;
float   POW_VALUE           = 0.0;
float   POW_VALUE_max       = 0.0;
float   level               = 8;
int     lasCmdDnr           = 0;
float   mediumValue         = 0;
int     indexVAD            = 200;


void processing_noise_reduction(float* bufferIn, float* bufferOut2) {

    int j =0;
    for (int i =0; i< BLOCK_SIZE; i++)
	{
        if (i >= 160) {
            FFTBufferOut[j] =  0;
        } else {
            FFTBufferOut[j] =  bufferIn[i];
        }
        j++;
        FFTBufferOut[j]=0;
        j++;
	}

    /* Process the data through the CFFT/CIFFT module */
    arm_cfft_f32(&arm_cfft_sR_f32_len256, FFTBufferOut, 0, 1);

    /* Process the data through the Complex Magniture Module for calculating the magnitude at each bin */
    arm_cmplx_mag_f32(FFTBufferOut, FFTBufferMag, FFT_SIZE);

    /*- Voice Activity Detector */
    /* Evaluate Noise  */
    energyChkPeriod_idx++;
    if (energyChkPeriod_idx>=indexVAD){
        POW_VALUE_max = 0;
        energyChkPeriod_idx=0;
    }
    POW_VALUE=0;
        float TH_VALUE_new = 0.0;
        int low = 2; int high =(FFT_SIZE/4);
        for (j =low; j< high; j++)
        {
           TH_VALUE_new += FFTBufferMag[j];
        }

        // Only for internal voice bins ...
        for (j =2; j< 30; j++)
        {     // Evaluate power spectrum ...
              POW_VALUE += FFTBufferMag[j]*FFTBufferMag[j];
        }
        // ... and update the current max values ...
        if (POW_VALUE>POW_VALUE_max){
               POW_VALUE_max = POW_VALUE;
        }

        //*******************************

    mediumValue = (TH_VALUE_new/(high-low)); // MEDIUM
    float TH_VALUE_low = (0.8+(level/10)) * mediumValue;
    TH_VALUE = level*mediumValue*(1.0-(POW_VALUE_max*10));
    if (TH_VALUE<TH_VALUE_low){
            TH_VALUE = TH_VALUE_low;
    }

     /*- DNR disabled if level is 0 */
     if (level == 0){
        TH_VALUE = 0;
	}

    /* Preserve values for plotting and apply subtraction */
    for (j =0; j< FFT_SIZE*2; j++)
	{
        /*- Subtract threshold noise */
        if (FFTBufferMag[j]<=TH_VALUE){
            FFTBufferMag[j] = 0;
	    }else{
            FFTBufferMag[j] =  FFTBufferMag[j]- TH_VALUE;
	    }
	}

	/* Rebuild & discard the fft complex signal from subctracted magnitude & original signal phase */
	int k =1;
	int s= 0;
	for (j =0; j< FFT_SIZE; j++)
	{

      float32_t r1 = FFTBufferOut[s]; float32_t i1 = FFTBufferOut[k];
      complex float z= r1 + i1*I;
      float32_t phi = cargf(z);
      float32_t r2 = FFTBufferMag[j]*arm_cos_f32(phi);
      float32_t i2 = FFTBufferMag[j]*arm_sin_f32(phi);

      FFTBufferOut[s] = r2;
      FFTBufferOut[k] = i2;
      s=s+2;
      k=s+1;
     }


    // Compute the inverse FFT ...
    arm_cfft_f32(&arm_cfft_sR_f32_len256, FFTBufferOut,1,1);

    // Rebuild and clean the signal if need
    j =0;
    for (int i =0; i< BLOCK_SIZE; i++)
	{
      bufferOut2[i] =  FFTBufferOut[j];

       j++; j++;
	}
    /*- DNR second stage LMS filter
    *   Over the Level 4 (on display) the noise derived by spectrum subctraction is strong
    *   (it is like an metallic effect on voice),
    *   then we can add a second stage, LMS filter, that works well when the difference beetwen the two
    *   input signals are wide. The overall effect is a fidelity on voice reproduction also when
    *   the noise reduction level is high.
    */
    // After level 4 activate the second stage.
    //if (level > 2)  arm_lms_f32(&Sl_LMS,&bufferPreOut[0],&bufferIn[0], &bufferOut2[0], &errOutput[0],BLOCK_SIZE);

}

/**************************************END OF FILE****/

