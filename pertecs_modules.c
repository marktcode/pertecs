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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <CoreServices/CoreServices.h>
#include <Accelerate/Accelerate.h>


#include "pertecs.h"

#define TWOPI (6.283185307179586476925286766559)

#include "filters.h"
#include "pertecs_external.c"


FFTSetupD     setup[20];
double 		*hamming[20];



/****************************************************************************/
/*																			*/
/*  set upp fft						*/
/*																			*/
/****************************************************************************/
void setupFFTspace(struct xml_data  DataPtr) {
int	N, i;

	for (N=2; N < 20; N++) setup[N-1] = NULL;

	for (i=0; i < DataPtr.build.build_size; i++) {

		if (DataPtr.Modules[i]->type == FFT) {
			N =  (int) log2( DataPtr.Modules[i]->buff_sz / BUFFERMULTIPLIER) ;
			if (setup[N-1] == NULL) {
		 	 	setup[N-1] = vDSP_create_fftsetupD ( N, FFT_RADIX2 );
		  		if ( setup[N-1] == NULL ) {      
					fprintf (stderr, "\nFFT_Setup failed to allocate enough memory.\n" );
					exit ( 0 );
				}
				hamming[N-1] = ( double * ) malloc ( N * sizeof ( double ) );		
				vDSP_hamm_windowD (hamming[N-1], N,  0);
			}
//			fprintf (stderr, "%d %s %d %d %d %ld %ld\n", i, DataPtr.Modules[i]->name, DataPtr.Modules[i]->type,  FFT, N, setup[N-1], hamming[N-1]); fflush(stderr);
		}
	}
}

/****************************************************************************/
/*																			*/
/*  take down fft						*/
/*																			*/
/****************************************************************************/
void takedownFFTspace() {
int	N;

	for (N=1; N < 20; N++) {
		if (setup[N-1] != NULL) vDSP_destroy_fftsetupD ( setup[N-1] );
		if (hamming[N-1] != NULL) free ( hamming[N-1] );
	}
}

/****************************************************************************/
/*																			*/
/*  entropy thread runs to completion asynchronously						*/
/*																			*/
/****************************************************************************/
// added input checking selectively to make sure inputs with a NAN don't prevent the processes from recovery.
void *Compute_Entropy(void *threadarg)
{
	struct module *ModulePtr;

	double Partition = 0, PRange = 0, PIncr = 1;
	long HashTableSize;
    MATCHLISTENTRY *EntryList;
    CODEWORD *PrimeList;
	char *Dynamics, *revDynamics;
	int Window, i, j; // to be set from stored parameter
	double Complexity, revComplexity;
	double *DataPtr;
	
	ModulePtr = (struct module *) threadarg;
	
	Window = ModulePtr->ParamPtr[WINDOW]->intvalue ;
	if (Window == 0) { fprintf(stderr, "entropy window must be greater than zero\n"); fflush(stderr); exit (0); }
	DataPtr = &(ModulePtr->input_buffPtr[ModulePtr->In_Ptr - Window + 1]); 
	
//	fprintf(stderr," entropy thread started %d %d \n", Window, ModulePtr->In_Ptr - Window + 1); fflush(stderr);
	Dynamics = (char *) malloc(Window * sizeof(char) );
	revDynamics = (char *) malloc(Window * sizeof(char) );
	HashTableSize = Window + 255;  										// approximate the maximum number of different codewords that might occur during whole process
    // Allocate memory
    EntryList = (MATCHLISTENTRY*) malloc(sizeof (MATCHLISTENTRY) * HashTableSize);
	PrimeList = (CODEWORD *)malloc ((Window + 2) * sizeof (CODEWORD)); // two extra nodes, head, end nodes in string list.
	
	if ((EntryList == NULL) || (PrimeList == NULL) || (Dynamics == NULL)) { fprintf(stderr, "Error Allocating Memory for linked list\n"); exit ( -1 ) ;}

//	for (Partition = -PRange; Partition <= PRange; Partition += PIncr) { // assumes PRange
		j = Window-1;
		for (i=0; i < Window; i++)  { 
			revDynamics[j--] = Dynamics[i]  =  (DataPtr[i] >= Partition);
															// fprintf(stderr,"%c", '0'+ Dynamics[i]); //debug
		}
															// fprintf(stderr,"\n"); fflush(stderr); //debug
		Complexity = (double) ftdSelAugment(Dynamics, Window, EntryList, PrimeList);	
		revComplexity = (double) ftdSelAugment(revDynamics, Window, EntryList, PrimeList);	
////		fprintf(stderr," comp: %g %g\n", Complexity, revComplexity); fflush(stderr);
//	} // end of Partition 
		
	free (Dynamics); Dynamics = NULL;
	free (revDynamics); Dynamics = NULL;
	free (EntryList); EntryList = NULL;
	free (PrimeList);
	
	
	if (revComplexity > Complexity) 
		ModulePtr->Output[MAX_ENTROPY]->value = invlogint( Complexity)/Window;
	else ModulePtr->Output[MAX_ENTROPY]->value = invlogint( revComplexity)/Window;
	
	threadcount--;
   pthread_exit((void *) 0);	
}


#define NUM_THREADS 100
pthread_t entropy_thread[NUM_THREADS];
#define MAXRAND (2147483647.0)


/****************************************************************************/
/*																			*/
/*  Process_Module															*/
/*																			*/
/****************************************************************************/
// notes
// an optimisation that might be made is to premultiply the filter values by appropriate constants: 1, 2, 6 this means t and s are multiplied 
// need to make sure that no output can ever be a NAN

int Process_Module( struct module *Module[], struct pertecs_build *buildPtr, long time, int state) {
	int i, j, k, ii, master_trigger = FALSE;
	double a, b, c, d, temp, spec_temp, spec_temp2;
	double *z_pointer, *f_pointer, *s_pointer, *t_pointer;
	int offsetPtr;
	int buffer_offset;
	int	rc, t;
	void *status;
	double *xvals, *yvals;
	double *xcoeffs, *ycoeffs;
	double temporary;
	DSPDoubleSplitComplex A, result;
	pthread_attr_t attr;

	
	t = 0; 	int p = 0;
//	pthread_attr_init(&attr);
//	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

// fprintf(stderr, "got here %d\n", buildPtr->build_size); fflush(stderr);

	for (i=0; i < buildPtr->build_size; i++) { // runs through all of the modules and processes inputs to obtain outputs.

	// module calculations
		switch (Module[i]->type) {	

			case PRESETS:	// potentiometers
			
					// input checking
					if (Module[i]->ParamPtr[0]->value == NAN ) Module[i]->Output[POTINX]->value = LARGE_FINITE_NUMBER;
						else Module[i]->Output[POTINX]->value =  Module[i]->ParamPtr[0]->value;  // just moves the potentiomenter preset values on out
					// input checking
					if (Module[i]->ParamPtr[2]->value == NAN ) Module[i]->Output[POTINY]->value = LARGE_FINITE_NUMBER;
						else Module[i]->Output[POTINY]->value =  Module[i]->ParamPtr[2]->value;
					// input checking
					if (Module[i]->ParamPtr[4]->value == NAN ) Module[i]->Output[POTINZ]->value = LARGE_FINITE_NUMBER;
						else Module[i]->Output[POTINZ]->value =  Module[i]->ParamPtr[4]->value;
					
					// input checking
					if (!strncmp(Module[i]->ParamPtr[1]->label, "en", 2 )) Module[i]->Output[SELINX]->value = 1.0;
						else Module[i]->Output[SELINX]->value =  0.0;  // just moves the potentiomenter preset values on out
					// input checking
					if (!strncmp(Module[i]->ParamPtr[3]->label, "en", 2 )) Module[i]->Output[SELINY]->value = 1.0;
						else Module[i]->Output[SELINY]->value =  0.0;
					// input checking
					if (!strncmp(Module[i]->ParamPtr[5]->label, "en", 2 )) Module[i]->Output[SELINZ]->value = 1.0;
						else Module[i]->Output[SELINZ]->value =  0.0;
					  
					  
				break;

			case FILTER: // a general linear filter all working out of a single buffer space that is ( order ) * BUFFERMULTIPLIER  doubles where  extra temproary space is input/output buffers
				if (Module[i]->buff_sz > 0) {
					buffer_offset = ( Module[i]->buff_sz / BUFFERMULTIPLIER); // get true buffer length
					xvals =  &Module[i]->input_buffPtr[0];  yvals = &Module[i]->input_buffPtr[buffer_offset]; xcoeffs = &Module[i]->input_buffPtr[2 * buffer_offset]; ycoeffs = &Module[i]->input_buffPtr[3 * buffer_offset];
					
					offsetPtr = Module[i]->In_Ptr;  
					
					Module[i]->Output[DELAYED]->value = xvals[offsetPtr] / Module[i]->ParamPtr[GAIN]->value;
					temp = 0;
					for (k = 0; k < buffer_offset ; k++) {
						temp += ( xvals[offsetPtr] * xcoeffs[k] + yvals[offsetPtr] * ycoeffs[k]) ;
						offsetPtr = (offsetPtr + 1) % buffer_offset;
					}

					if (Module[i]->Input[STANDARD]->value == NAN ) xvals[Module[i]->In_Ptr] = LARGE_FINITE_NUMBER;
					else  xvals[Module[i]->In_Ptr] = Module[i]->Input[STANDARD]->value * Module[i]->ParamPtr[GAIN]->value; // gain is a hidden parameter
					
					Module[i]->Output[FILTEROUT]->value = yvals[Module[i]->In_Ptr] = temp + xvals[Module[i]->In_Ptr] * xcoeffs[0]; // adding in last one
					Module[i]->In_Ptr = (Module[i]->In_Ptr + 1) % buffer_offset;
				}
				break;

			case COMPAR: // the comparator
				// input checking
				if (Module[i]->Input[STANDARD]->value == NAN) temp = Module[i]->Output[DELAYED]->value = LARGE_FINITE_NUMBER;
					else temp = Module[i]->Output[DELAYED]->value = Module[i]->Input[STANDARD]->value;
					
				if (temp > 0 ) { 
					Module[i]->Output[ZERO]->value = Module[i]->Output[NEG]->value = Module[i]->Output[NONPOS]->value = 0.0;
					Module[i]->Output[NONNEG]->value = Module[i]->Output[POS]->value = 1.0;
				} 
				else if (temp == 0 ) { 
					Module[i]->Output[NEG]->value = Module[i]->Output[POS]->value = 0.0;					
					Module[i]->Output[NONPOS]->value = Module[i]->Output[NONNEG]->value = Module[i]->Output[ZERO]->value = 1.0;
				}
				else { // temp < 0
					Module[i]->Output[POS]->value = Module[i]->Output[ZERO]->value = Module[i]->Output[NONNEG]->value  = 0.0;
					 Module[i]->Output[NEG]->value = Module[i]->Output[NONPOS]->value = 1.0;					
				}
				break;

			case NOISE: // noise module
				Module[i]->Output[NOISEOUT]->value = (double) random()/ MAXRAND;
				break;

			case DELINE:	
				if (Module[i]->buff_sz <=0) Module[i]->Output[DELAYED]->value = Module[i]->Input[STANDARD]->value;
				else if ( Module[i]->ParamPtr[DELAYMODE]->label[0] == 'd') { // delay line true
					Module[i]->input_buffPtr[Module[i]->In_Ptr] = Module[i]->Input[STANDARD]->value;				// only first input is handled this way
					offsetPtr = Module[i]->In_Ptr = (Module[i]->In_Ptr + 1) % Module[i]->buff_sz;					// increment pointer
					Module[i]->Output[DELAYED]->value =  Module[i]->input_buffPtr[offsetPtr];  						// output the delayed position
				}
				else if (Module[i]->ParamPtr[DELAYMODE]->label[0] == 'f') { // function generator
					while (Module[i]->Input[STANDARD]->value < 0) Module[i]->Input[STANDARD]->value += Module[i]->buff_sz;
					Module[i]->In_Ptr = ((int) Module[i]->Input[STANDARD]->value ) % Module[i]->buff_sz;
						offsetPtr = (Module[i]->In_Ptr + 1) % Module[i]->buff_sz;					// increment pointer
					Module[i]->Output[DELAYED]->value =  Module[i]->input_buffPtr[Module[i]->In_Ptr] + ( Module[i]->Input[STANDARD]->value - (double) ((int) Module[i]->Input[STANDARD]->value) ) * ( Module[i]->input_buffPtr[offsetPtr] - Module[i]->input_buffPtr[Module[i]->In_Ptr] )  ;  						// output the delayed position
				}
				// in the FIR filter we have double the buffer size and use the first half for the accumulator filter processing and the second half for storing the weights
				// an example script for a raised cosine filter with 101 taps
				// echo "" | awk '{for (i=0; i < 101; i++) { print i, 0; sum+=-.5*cos(i*2*3.1415926/101)+.5;}  for (j=0; j < 101; j++) print i++, a=0.5*(-cos(j*2*3.1415926/101)+1)/sum }' > /tmp/func.sin

				else if (Module[i]->ParamPtr[DELAYMODE]->label[0] == 'F') { // FIR filter
					Module[i]->input_buffPtr[Module[i]->In_Ptr] = Module[i]->Input[STANDARD]->value;				// only first input is handled this way
					offsetPtr = Module[i]->In_Ptr = (Module[i]->In_Ptr + 1) % (Module[i]->buff_sz/2);					// increment pointer

					Module[i]->Output[DELAYED]->value = 0;
					k = (Module[i]->buff_sz/2);
					ii = offsetPtr;
					for (j = 0; j < Module[i]->buff_sz/2; j++ ) {
						Module[i]->Output[DELAYED]->value += Module[i]->input_buffPtr[ (ii++ % (Module[i]->buff_sz/2)) ] * Module[i]->input_buffPtr[k++];
					}
				}
				break;

			case DIFFINT:	
				if (Module[i]->Input[STANDARD]->value == NAN) Module[i]->input_buffPtr[Module[i]->In_Ptr] = LARGE_FINITE_NUMBER;
					else Module[i]->input_buffPtr[Module[i]->In_Ptr] = Module[i]->Input[STANDARD]->value;					// only first input is handled this way
				

				a = b = c = d = 0.0;		
				//fprintf(stderr, "%d %d \n", i, Module[i]->buff_sz); fflush(stderr);
				if (Module[i]->buff_sz >= 3) {
					if (time >= (long)  Module[i]->buff_sz) { // only proceed if buffer has been filled 
						offsetPtr = Module[i]->In_Ptr - Module[i]->buff_sz; if (offsetPtr < 0) offsetPtr += Module[i]->buff_sz;					// points to the left most value in the filter window
						// set up filter pointers
						if ((Module[i]->buff_sz/2 > 0) && (Module[i]->buff_sz/2 <= 65)) {
								z_pointer = zcoefptr[Module[i]->buff_sz/2]; 
								f_pointer = fcoefptr[Module[i]->buff_sz/2]; 
								s_pointer = scoefptr[Module[i]->buff_sz/2]; 
								t_pointer = tcoefptr[Module[i]->buff_sz/2];
	
	//							z_pointer = zcoefptr[20]; f_pointer = fcoef20; s_pointer = scoef20; t_pointer = tcoef20;
	//							fprintf (stderr, "%ld %ld \n", (unsigned long int)  zcoefptr[20], (unsigned long int)  zcoef20); fflush (stderr); exit (0);
	
						}
						else {
								fprintf (stderr, " buffer too large for smoothing filter\n"); fflush (stderr); exit (0);
						}
				
						for (k = 0; k < Module[i]->buff_sz ; k++) {								// compute the weighted filter sum
							temp = 	Module[i]->input_buffPtr[(offsetPtr + k) %  Module[i]->buff_sz];
							d += temp * z_pointer[k];														
							c += temp * f_pointer[k];													//  includes the filter length scaling  x L
							b += temp * s_pointer[k]; // * 2.0 now catered for in array constants;		  includes the filter length scaling 	x L^2
							a += temp * t_pointer[k]; // * 6.0 now catered for in array constants;		  includes the filter length scaling    x L^3	
						}
			
						temporary = b / 48.0; // half the adjustment 
						Module[i]->Output[INTEGRATED]->value +=  temporary;  // b * Module[i]->ParamPtr[1]->value / 48.0  ; // see manual for explanation ... adding half of the error now and half the next time
						if ( time  == (long) Module[i]->buff_sz) { // do a one off correction for missing values when frame_counter < filt_hz
	
						// to be replaced with a single calculation
						//	for (k = -Module[i]->buff_sz/2; k < 0; k++) {
						//		for (j = -Module[i]->buff_sz/2; j <= k; j++) 
						//			Module[i]->Output[INTEGRATED]->value += (( 2.0 * a * j  + b ) * j  + c) * j   + (2.0 * b + j * 6.0 * a)/12.0 ;
						//	}
	
						}
							
					}
					else temporary = 0; // need to make sure we have no rubbish coming into the integrator at startup time
	
					

				}
				offsetPtr = Module[i]->In_Ptr - Module[i]->buff_sz/2 - 1; if (offsetPtr < 0) offsetPtr += Module[i]->buff_sz;  // find the delayed position
				Module[i]->Output[DELAYED]->value =  Module[i]->input_buffPtr[offsetPtr];  
								
				Module[i]->Output[THRD_DERIV]->value = a ;											// smoothed third derivative
				Module[i]->Output[SCND_DERIV]->value = b ;											// smoothed second derivative
				Module[i]->Output[FST_DERIV]->value  = c ;											// smoothed first derivative
				Module[i]->Output[SMOOTHED]->value   = d ;											// smoothed zeroth order

				if (Module[i]->Input[SETHOLD]->value >= DIFFSET) {
					Module[i]->Output[INTEGRATED]->value = Module[i]->ParamPtr[INITIALISATION]->value; //resets 
					Module[i]->spare = 0;
				}
				else if (Module[i]->Input[SETHOLD]->value > DIFFHOLD) { //  in normal mode ... if -1 < input < 1
					//Module[i]->Output[DELAYED]->value = Module[i]->input_buffPtr[(offsetPtr + Module[i]->buff_sz/2 ) %  Module[i]->buff_sz]); 		// delayed raw, integrated
					// offsetPtr + Module[i]->buff_sz/2  is the inptr 
					Module[i]->Output[INTEGRATED]->value += (Module[i]->input_buffPtr[(Module[i]->In_Ptr) %  Module[i]->buff_sz] + Module[i]->spare )/2.0; // Module[i]->input_buffPtr[(Module[i]->In_Ptr -1) %  Module[i]->buff_sz] )/2.0 ; // trapezoidal
					Module[i]->spare = Module[i]->input_buffPtr[(Module[i]->In_Ptr) %  Module[i]->buff_sz] + 2 * temporary; // both the next sample and the correction term  divided by two when finally added in, so 2x temporary in anticipation
					// input checking on SPECIAL
					if (Module[i]->Output[SPECIAL]->value == NAN) Module[i]->Output[INTEGRATED]->value += LARGE_FINITE_NUMBER;
						else Module[i]->Output[INTEGRATED]->value += Module[i]->Input[SPECIAL]->value; 			// Special feature: allows ow delay feedback around integrator without affecting differentiation input
				}
				// else  HOLD = do nothing 
				Module[i]->In_Ptr = (Module[i]->In_Ptr + 1) % Module[i]->buff_sz;					// increment pointer
				break;
	
			case POLAR: // math functions
				if (Module[i]->Input[STANDARD]->value == NAN) temp = Module[i]->Output[DELAYED]->value = LARGE_FINITE_NUMBER;
					else temp = Module[i]->Output[DELAYED]->value = Module[i]->Input[STANDARD]->value;		// implicit delayed

				if (Module[i]->Input[SPECIAL]->value == NAN) spec_temp = Module[i]->Output[DELAYED2]->value = LARGE_FINITE_NUMBER;
					else spec_temp = Module[i]->Output[DELAYED2]->value = Module[i]->Input[SPECIAL]->value;	// may add delayed to window

				if (Module[i]->Input[SPECIAL2]->value == NAN) spec_temp2 = Module[i]->Output[DELAYED3]->value = LARGE_FINITE_NUMBER;
					else spec_temp2 = Module[i]->Output[DELAYED3]->value = Module[i]->Input[SPECIAL2]->value;	// may add delayed to window

				Module[i]->Output[NORM]->value = sqrt(temp * temp + spec_temp * spec_temp + spec_temp2 * spec_temp2) ;	// NORM
				Module[i]->Output[THETA]->value = atan2(spec_temp, temp) / TWOPI;					// theta angle is normalised -1 through 1
				temp = sqrt(temp * temp + spec_temp * spec_temp);
				Module[i]->Output[PHI]->value = atan2(temp, spec_temp2) / TWOPI;					// phi angle is normalised -1 through 1
				break;


			case MATHMOD: // math functions
				if (Module[i]->Input[STANDARD]->value == NAN) temp = Module[i]->Output[DELAYED]->value = LARGE_FINITE_NUMBER;
					else temp = Module[i]->Output[DELAYED]->value = Module[i]->Input[STANDARD]->value;
				if (temp > 0 ) 	{
					Module[i]->Output[SQRT]->value = sqrt(temp) ;									// sqrt
					Module[i]->Output[LOG]->value = log(temp) ;
					Module[i]->Output[RECIPROCAL]->value = 1/temp;
				}
				else if (temp == 0) {
					Module[i]->Output[SQRT]->value = 0 ;
					Module[i]->Output[LOG]->value = 0 ;												// NAN
					Module[i]->Output[RECIPROCAL]->value = 0;										// NAN
					
				}
				else {
					Module[i]->Output[SQRT]->value = 0 ;											// sqrt
					Module[i]->Output[LOG]->value = 0 ;
					Module[i]->Output[RECIPROCAL]->value = 1/temp;
				}

				if (temp) Module[i]->Output[RECIPROCAL]->value = 1/temp;							// reciprocal
				Module[i]->Output[EXP]->value = exp(temp) ;											// exp
				Module[i]->Output[SIN]->value  = sin(6.283185307179586476925286766559 * temp ) ;						//    3.14159265358979323846264338327950288419716939937 

				break;


			case QTC:	// the quick T-entropy calculation
			
				if ( Module[i]->Input[STANDARD]->value == NAN) temp = Module[i]->input_buffPtr[Module[i]->In_Ptr] = LARGE_FINITE_NUMBER;
					else temp = Module[i]->input_buffPtr[Module[i]->In_Ptr] = Module[i]->Input[STANDARD]->value;					// only first input is handled this way
				
				offsetPtr = Module[i]->In_Ptr - Module[i]->buff_sz/2; if (offsetPtr < 0) offsetPtr += Module[i]->buff_sz;  // find the delayed position
		
				Module[i]->Output[DELAYED]->value = Module[i]->input_buffPtr[offsetPtr];  // output the delayed position
				if (time >=  Module[i]->ParamPtr[WINDOW]->intvalue) {   // only proceed if the window worth has been filled 
					

					// if in to quartile in buffer, copy the input back into the bottom quartile
					if (Module[i]->In_Ptr >=   ( buffer_offset = ( Module[i]->buff_sz / BUFFERMULTIPLIER) * 3) ) {
						Module[i]->input_buffPtr[Module[i]->In_Ptr - buffer_offset ] = temp;
						if ( Module[i]->In_Ptr ==  Module[i]->buff_sz - 1 ) Module[i]->In_Ptr = Module[i]->buff_sz / BUFFERMULTIPLIER - 1; // anticipate the end of the buffer and reset to start end of first quartile
					}

					if ( Module[i]->ParamPtr[COUNTER]->intvalue == 0) {
//									fprintf(stderr, "COUNTER %d WINDOW_SHIFT %d  \n",Module[i]->ParamPtr[COUNTER]->intvalue, Module[i]->ParamPtr[WINDOW_SHIFT]->intvalue); fflush(stderr);
						threadcount++; 
						
						rc = pthread_create(&entropy_thread[t], NULL, Compute_Entropy, (void *) Module[i]);
						if (rc) {
							fprintf(stderr, ">>ERROR; failed to establish thread: %d\n", rc);
							exit(-1);
						}
						else pthread_detach(entropy_thread[t]);
						t++;
					}
				
					Module[i]->ParamPtr[COUNTER]->intvalue = ( Module[i]->ParamPtr[COUNTER]->intvalue + 1) % ( Module[i]->ParamPtr[WINDOW_SHIFT]->intvalue); // increment the respective counter 
				}
				else {
					Module[i]->Output[MAX_ENTROPY]->value = 0.0  ;										// 
					Module[i]->Output[RAST_ENTROPY]->value = 0.0  ;										// 
				}
				
				Module[i]->In_Ptr = (Module[i]->In_Ptr + 1) % Module[i]->buff_sz;					// increment pointer

				break;

			case FFT:	//  FFT calculation uses four buffers... 2 input 2 output  these are all catenated as a single buffer
				if (Module[i]->Input[REALFFTIN]->value == NAN) temp = LARGE_FINITE_NUMBER;
					else temp =  Module[i]->Input[REALFFTIN]->value;		
					
				// Module[i]->Output[DELAYREALFFT]->value =

				if (Module[i]->Input[IMAGFFTIN]->value == NAN) spec_temp  = LARGE_FINITE_NUMBER;
					else spec_temp =  Module[i]->Input[IMAGFFTIN]->value;	// need to set up the delay to reflect the whole of the FFT delay, so the copy will be done with the FFT

				// Module[i]->Output[DELAYIMAGFFT]->value =

				if (Module[i]->Input[SYNCIN]->value >= 1.0) Module[i]->In_Ptr = 0;						// synchronise the fft counter on a unitary input spike

				buffer_offset = Module[i]->buff_sz / BUFFERMULTIPLIER;

				A.realp = (double *) &Module[i]->input_buffPtr[0];
				A.imagp = (double *) &Module[i]->input_buffPtr[buffer_offset];
				result.realp = (double *) &Module[i]->input_buffPtr[2 * buffer_offset]; 
				result.imagp = (double *) &Module[i]->input_buffPtr[3 * buffer_offset];

				A.realp[Module[i]->In_Ptr] = temp;														// load the input buffers
				A.imagp[Module[i]->In_Ptr] = spec_temp;

				Module[i]->Output[REALFFT]->value = (double) result.realp[Module[i]->In_Ptr];
				Module[i]->Output[IMAGFFT]->value = (double) result.imagp[Module[i]->In_Ptr];


				Module[i]->In_Ptr = (Module[i]->In_Ptr + 1) ;											// increment pointer				
				if (Module[i]->In_Ptr >= buffer_offset) {
					Module[i]->In_Ptr = 0;																//reset the pointer				
					int N = (int) log2(buffer_offset);
					int fftsize = 1 << N;																// find nearest size of fft as  "offsetPtr"

//					fprintf(stderr, "\t\t\t\t\%d %ld %ld\n", N, Module[i]->input_buffPtr, hamming[N-1]);
					vDSP_vsmulD(  A.imagp, 1, hamming[N-1], A.imagp, 1, fftsize ); 							// window the imaginary part
					vDSP_vsmulD(  A.realp, 1, hamming[N-1], A.realp, 1, fftsize ); 							// window the real part
			vDSP_fft_zopD ( setup[N-1], &A, 1, &result, 1, N , FFT_FORWARD );

				}
				
				break;

			case EXTERN:	//  multiplex external modules from here
				external(Module[i], buildPtr, time);
				break;

			case TRACE:	//  scope viewing angle calculation 
				// no input checking on the trace since it never outputs to further computations
				curvePtr[p]->retrace = Module[i]->In_Ptr; // signal to display routine the position of the "retrace"
				ii = 3 * Module[i]->In_Ptr;
				
				snprintf(&curvePtr[p]->label[0],LABEL_SIZE, "%s", Module[i]->name ); 
				int which = curvePtr[p]->timeaxis = Module[i]->ParamPtr[TRACETIMER]->label[0]; // x y or z time scales ... then the corresponding input becomes the trigger level.
				int	trigger	= Module[i]->ParamPtr[TRACETIMER]->label[1]; // x y or z trigger or free running scope
				switch(which) { // is this to be copied through evry time... or just once each time the user interface updates.
					case 'x':
						if ( Module[i]->In_Ptr < Module[i]->buff_sz - 1 ) Module[i]->In_Ptr++; 	// scan in progress. Continue to end
						else {
							switch(trigger) { // compare selected trigger input with the time-base input
								case 'x':  // trig control is in x input, and trigger reference is 0
									break;
								case 'y': // trigger on y input... trig reference is in x input
									if ((Module[i]->Input[SPECIAL]->value > Module[i]->Input[STANDARD]->value ) && ( curvePtr[p]->polyline[ii+1] <= Module[i]->Input[STANDARD]->value )) {
										Module[i]->In_Ptr = 0; // reset
										master_trigger=TRUE;
									}
									break;
								case 'z': // trigger on z input... trig reference is in x input
									if ((Module[i]->Input[SPECIAL2]->value > Module[i]->Input[STANDARD]->value ) && ( curvePtr[p]->polyline[ii+2] <= Module[i]->Input[STANDARD]->value )) {
										Module[i]->In_Ptr = 0; // reset
										master_trigger=TRUE;
									}
									break;
								case 't': if (master_trigger==TRUE) Module[i]->In_Ptr = 0; // trigger 								
									break;
								default: // else free-run 								
									if (Module[i]->In_Ptr == Module[i]->buff_sz - 1) Module[i]->In_Ptr = 0; 
								break;
							}
						}

						if ( Module[i]->In_Ptr + 5 < Module[i]->buff_sz - 1 ) start_scan = Module[i]->input_buffPtr[Module[i]->In_Ptr + 5 ]; 	
						else start_scan = Module[i]->input_buffPtr[5];
						 

						curvePtr[p]->polyline[ii]  =  (float) xorig + xlimit * ( (float) Module[i]->input_buffPtr[Module[i]->In_Ptr]) ; //curvePtr[p]->xoffset + 
						curvePtr[p]->polyline[ii+1] =  Module[i]->Input[SPECIAL]->value ;  	// enter points in directly no scale/offset "curvePtr[p]->yoffset + curvePtr[p]->yscale *"
						curvePtr[p]->polyline[ii+2] = Module[i]->Input[SPECIAL2]->value ; 	// enter points in directly no scale/offset "curvePtr[p]->zoffset + curvePtr[p]->zscale *"


						break;

					case 'y':
						if ( Module[i]->In_Ptr < Module[i]->buff_sz - 1 ) Module[i]->In_Ptr++; 	// scan in progress. stops at end
						else {
							switch(trigger) { // compare selected trigger input with the time-base input
								case 'x': // trigger on x
									if ((Module[i]->Input[STANDARD]->value > Module[i]->Input[SPECIAL]->value ) && ( curvePtr[p]->polyline[ii] <= Module[i]->Input[SPECIAL]->value )) {
										Module[i]->In_Ptr = 0; // reset
										master_trigger=TRUE;
									}
									break;
								case 'y': // trig control is in y input, and trigger reference is 0
									break;
								case 'z': // trigger on z
									if ((Module[i]->Input[SPECIAL2]->value > Module[i]->Input[SPECIAL]->value ) && ( curvePtr[p]->polyline[ii+2] <= Module[i]->Input[SPECIAL]->value )) {
										Module[i]->In_Ptr = 0; // reset
										master_trigger=TRUE;
									}
									break;
								case 't': if (master_trigger==TRUE) Module[i]->In_Ptr = 0; // trigger slave								
									break;
								default: // else free-run 
									if (Module[i]->In_Ptr == Module[i]->buff_sz - 1) Module[i]->In_Ptr = 0; 
								break;
							}
						}

						if ( Module[i]->In_Ptr + 5 < Module[i]->buff_sz - 1 ) start_scan = Module[i]->input_buffPtr[Module[i]->In_Ptr + 5 ]; 	
						else start_scan = Module[i]->input_buffPtr[5];

						curvePtr[p]->polyline[ii+1]  = yorig + ylimit * (float) Module[i]->input_buffPtr[Module[i]->In_Ptr] ; //curvePtr[p]->yoffset + 
						curvePtr[p]->polyline[ii]  = Module[i]->Input[STANDARD]->value ; 	// enter points directly 
						curvePtr[p]->polyline[ii+2] = Module[i]->Input[SPECIAL2]->value ; 	// enter points directly

						break;

					case 'z':
						if ( Module[i]->In_Ptr < Module[i]->buff_sz - 1 ) Module[i]->In_Ptr++; 	// scan in progress. stops at end
						else {
							switch(trigger) { // compare selected trigger input with the time-base input
								case 'x': // trigger on x
									if ((Module[i]->Input[STANDARD]->value > Module[i]->Input[SPECIAL2]->value ) && ( curvePtr[p]->polyline[ii] <= Module[i]->Input[SPECIAL2]->value )) {
										Module[i]->In_Ptr = 0; // reset
										master_trigger=TRUE;
									}
									break;
								case 'y': // trigger on y
									if ((Module[i]->Input[SPECIAL]->value > Module[i]->Input[SPECIAL2]->value ) && ( curvePtr[p]->polyline[ii+1] <= Module[i]->Input[SPECIAL2]->value )) {
										Module[i]->In_Ptr = 0; // reset
										master_trigger=TRUE;
									}
									break;
								case 'z': // trig control is in z input, and trigger reference is 0
									break;
								case 't': if (master_trigger==TRUE) Module[i]->In_Ptr = 0; // trigger slave								
									break;
								default: // else free-run 
									if (Module[i]->In_Ptr == Module[i]->buff_sz - 1) Module[i]->In_Ptr = 0; 
								break;
							}
						}

						if ( Module[i]->In_Ptr + 5 < Module[i]->buff_sz - 1 ) start_scan = Module[i]->input_buffPtr[Module[i]->In_Ptr + 5 ]; 	
						else start_scan = Module[i]->input_buffPtr[5];

						curvePtr[p]->polyline[ii+2]  =  zorig + zlimit * (float) Module[i]->input_buffPtr[Module[i]->In_Ptr] ; //curvePtr[p]->zoffset +
						curvePtr[p]->polyline[ii]  = Module[i]->Input[STANDARD]->value ; 	// enter points directly
						curvePtr[p]->polyline[ii+1] =  Module[i]->Input[SPECIAL]->value ;  	// enter points directly"

						break;

					default: // xyz graphing
						curvePtr[p]->polyline[ii]  =  Module[i]->Input[STANDARD]->value ; 	// enter points directly
						curvePtr[p]->polyline[ii+1] =  Module[i]->Input[SPECIAL]->value ;  	// enter points directly
						curvePtr[p]->polyline[ii+2] = Module[i]->Input[SPECIAL2]->value ; 	// enter points directly
						Module[i]->In_Ptr = (Module[i]->In_Ptr + 1) % Module[i]->buff_sz;					// increment pointer
					break;
				}
						
				p++;
				
				break;


			case SCOPE:	//  scope viewing angle calculation
				break;

			case SYSTEM:	//  system control is a negative value -1.0 
					if (( Module[i]->Input[SPECIAL]->value  >= 1.0 )  && ( Module[i]->Input[STANDARD]->value  == 0.0  )) state = STOP; // special = stop   and standard = pause .... only stop if pause not set
					else if ( Module[i]->Input[SPECIAL2]->value  >= 1.0 )  state = RESET; // SPECIAL2 = reset
					else if ( Module[i]->Input[STANDARD]->value  >= 1.0  ) { //  standard  = pause  is set
							if ( Module[i]->Input[SPECIAL]->value  == 0.0 )  { state = PAUSE; buildPtr->multiplier_Rate = 1.0; } // if stop == 0 then pause  else
							else {state = RUN; buildPtr->multiplier_Rate = (double) Module[i]->Input[SPECIAL]->value; }	//  if stop gives the multiplier 																									// stop input interpretted as a clock multiplier
					}
					else buildPtr->multiplier_Rate = -1.0;
				break;

			default: break;
		}
	}
	
//   pthread_attr_destroy(&attr);
/*   for(t = 0; t < buildPtr->Ent_build_size; t++)
   {
//   		rc = pthread_detach(entropy_thread[t]);
      rc = pthread_join(entropy_thread[t], &status);
      if (rc)
      {
         printf("ERROR; return code from pthread_join() is %d\n", rc);
         exit(-1);
      }
 //     printf("Completed join with thread %d status= %ld\n",t, (long)status);
   }

//   pthread_exit(NULL);
	
*/	
	
//	while(threadcount) {usleep(20);} // wait until all threads complete  this just blocks for some reason... don't seem to need it.
// fprintf(stderr, "Threads: %d       \n",  threadcount); fflush(stderr);
	return (state);

}


