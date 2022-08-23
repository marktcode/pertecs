/*
# PERTECS (Programmable Environment for Real-Time emulation of Continuous Systems) is a suite of tools. At the core of this suite is an executable UNIX tool, configured using the language of analogue computer, to give flexible real-time data processing capabilities
# Copyright (C) 2008,2009  Author, Mark R. Titchener, email mark@tcode.auckland.ac.nz
#
# This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <pthread.h>
#include <fcntl.h>   /* File control definitions */     
#include <errno.h>   /* Error number definitions */     
#include <time.h>


static	int	rc;



// extern structures


#define true	1
#define false	0

#define FILTERMIX	(1.0)		//ratio gaussianweighted sinc to gaussian FILTERMIX:(1-FILTERMIX)
/****************************************************************************/
/*																			*/
/*  initialise  tables for gaussian sinc upsampling filters					*/
/*																			*/
/****************************************************************************/
// upsample is the upsample rate... no_zeros determines the truncation off point of the gaussian shaped sinc
// returns the pointer to the array of weights, created by routine
// assume that float is good enough for all interpolation/upsampling


float *init_gsinc( int upsample, int no_zeros) {
	int i, size;
	float *arrayPtr = NULL, fupsample;

	fupsample = (float) upsample;

	size = no_zeros*upsample  ; // eg no_zeros = 5 and upsample = 4  give 20 points per side. 
	arrayPtr = (float *) calloc((2 * size +1), sizeof(float)); 

	for (i=1; i <= size; i++) {
//		if ( (i % upsample) == 0) arrayPtr[size + i ] = arrayPtr[size - i ] = 0; // force small terms to exactly zero
//		else  arrayPtr[size + i ] = arrayPtr[size - i ] =  sin(3.14159267 *((float)i) / fupsample) /(3.1415926 * ((float)i)/ fupsample)  *  exp(-(i*i)/(3.1415926 * fupsample * fupsample)); // have initialised the array with the constants 

		arrayPtr[size + i ] = arrayPtr[size - i ] = (FILTERMIX * sin(3.14159267 *((float)i) / fupsample) /(3.1415926 * ((float)i)/ fupsample) + (1-FILTERMIX)/3.1415926) * exp(-(i*i)/(3.1415926 * fupsample * fupsample)); // have initialised the array with the constants 
// *

	}
	arrayPtr[size] = FILTERMIX + (1-FILTERMIX)/3.1415926; 		// force central term to exactly 1
	return( (float *) arrayPtr);
}


/****************************************************************************/
/*																			*/
/*  step one position upsampling filters									*/
/*																			*/
/****************************************************************************/
// assumes a filter ring buffer of size "size" and associated index
/*float step_one( int upsample, int no_zeros, int *farray_indexPtr, float *filterarray, float weights, float input_value) {

	int	j, size;
	
	if (upsample == 1) return (input_value);
	else {
		size = ( upsample * no_zeros * 2 )  + 1; // always an odd number 

		for (j=0; j < size; j++) {						// update the ring buffer by summing in the new weights
			filterarray[farray_index] += input_value * weights[j];
			farray_index  = (farray_index + 1) % size; // should end up where it was
		}
		for (j=0; j < upsample; j++) {					// output just the interpolated data 
			farray_index  = (farray_index + 1) % size;	// step along the ring buffer
			fprintf(fid , "%d %g\n", (*countptr)++, filterarray[farray_index]);	
			fflush(fid);
			filterarray[farray_index] = 0; // clear ready for new data
		}
	}
	return(farray_index);
}
*/


