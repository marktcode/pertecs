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

//#define NDD1	"/dev/tty.SLAB_USBtoUART"
#define NDD1	"/dev/tty.usbserial-FTG8FMNN"
#define HONEY1	"/dev/tty.usbserial-FTFK3FKQ"
#define NDD_SAMPLE_RATE	(200) // 200Hz
#define	PPT_SAMPLE_RATE	(100) // 120Hz
static	pthread_t nddthread, pptthread;
static float zeropressure = 0.147645;		// zero calibratopn for ppt


static	int	rc;



// extern structures

struct Sensor_data
{
	struct termios options; 
	int port;
	int thread_id;
	int thread_status;
	
	int up_samplerate; 
	int	sample_cntr;
	int	size;
	int farray_index;				// index to array
	float  *weightsPtr;
	float  *filterarrayPtr;		// array

};

struct	Sensor_data	ndd_flow, ppt_pressure ;

struct frame
{
	FILE  *fid;
	float nddF; // ndd flow
	float ppt;  // ppt flow


};

struct	frame	dataframe;

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

/****************************************************************************/
/*																			*/
/*  ndd monitor thread														*/
/*																			*/
/****************************************************************************/

void *NDD_monitor(void *threadarg)
{
	struct Sensor_data *my_frame;

    char  character, character1, number;
    char  command[5];
	int n, i;


	int count = 0;
    int	ultraflow;
	sleep(1);
	my_frame = (struct Sensor_data *) threadarg;
	my_frame->port = open( NDD1 , O_RDWR | O_NOCTTY | O_NDELAY);
	

	
	my_frame->thread_status = true;
	
	
	if (my_frame->port == -1) perror("\n>> ERROR: Unable to open ndd port\n");       
	else {
		fcntl(my_frame->port, F_SETFL, 0);

	    tcgetattr(my_frame->port, &my_frame->options); 
	    cfsetispeed(&my_frame->options, B57600);    
	    cfsetospeed(&my_frame->options, B57600); 
	    my_frame->options.c_cflag |= (CLOCAL | CREAD); 
	    
	    tcsetattr(my_frame->port, TCSANOW, &my_frame->options); 

    	sleep (1);

		command[0]= (char) 1; command[1]= (char) 1; command[2]= (char) 18; command[3]= (char) 112; command[4]= (char) 151; 		
       	write (my_frame->port, &command, 5); // set base line of ndd... 01 01 12 70 97

    	sleep (1);
    	
		command[2]= (char) 16; command[3]= (char) 83; command[4]= (char) 133; 		
       	write (my_frame->port, &command, 5); // 01 01 10 53 85
       	    	
		command[2]= (char) 203; command[3]= (char) 59; command[4]= (char) 219; 		
       	write (my_frame->port, &command, 5); // 01 01 CB 3B DB  will expect a 7 byte code to return


		n = read( my_frame->port , &number, 1);
//		if ((n==1) && (number==7)) {fprintf (stderr,"\n>> reading ndd status\n"); fflush(stderr);}
		
		for (i=1; i< number; i++) n = read( my_frame->port , &character, 1);
		
		command[2]= (char) 17; command[3]= (char) 66; command[4]= (char) 12; 		
       	write (my_frame->port, &command, 5); // 01 01 11 42 0C   close out the preamble

    	sleep (1);
		command[2]= (char) 23; command[3]= (char) 39; command[4]= (char) 58; 		
       	write (my_frame->port, &command, 5); // start flow ... 01 01 17 27 3A
       	
//       	printf (">> reading ndd stream\n"); fflush(stdout);

    	while (my_frame->thread_status == true) { 

			n = read( my_frame->port , &character, 1); 
//			printf ("c%c",character); fflush(stdout);

			if ( (character & 240) == 176) { 
				count++;
				n = read( my_frame->port , &character1, 1); 
				ultraflow = (int) ((( character & 15) << 8) | ((unsigned) character1 & 255)); 
				if ( ultraflow > 2047) ultraflow = ultraflow - 4096;
				dataframe.nddF = 0.03 * (float) ultraflow;

				character = 0;
			}
			else if ( (character & 240) == 240) {
				n = read( my_frame->port , &character1, 1); // read next  to keep it in pair sync when it hits the check sum
				character = 0;
			}
		}
//		fprintf(stderr,"\n> closing ndd port\n"); fflush(stderr); 
		command[2]= (char) 24; command[3]= (char) 223; command[4]= (char) 205; 		
       	write (my_frame->port, &command, 5); // 01 01 18 DF CD  stop flow
		command[2]= (char) 17; command[3]= (char) 66; command[4]= (char) 12; 		       	
       	write (my_frame->port, &command, 5); // 01 01 11 42 0C   close

		if ( ndd_flow.weightsPtr ) free( ndd_flow.weightsPtr );
		if ( ndd_flow.filterarrayPtr ) free( ndd_flow.filterarrayPtr );
	}
	close( my_frame->port );


	pthread_exit(NULL);

}





/****************************************************************************/
/*																			*/
/*  ppt monitor thread														*/
/*																			*/
/****************************************************************************/

void *PPT_monitor(void *threadarg)
{
	struct Sensor_data *my_frame;

    char  character, inputarray[80];
	int n, inptr = 0, device = 0; 
	float pressure = 0.0;
	int	sync = false, synclock = false;
     

    sleep(1);
	my_frame = (struct Sensor_data *) threadarg;

	my_frame->thread_status = true; 
	my_frame->port = open( HONEY1 , O_RDWR | O_NOCTTY | O_NDELAY);

	if (my_frame->port == -1) perror(">> Unable to open honeywell port\n");       
	else {
      	fcntl(my_frame->port, F_SETFL, 0);

	    tcgetattr(my_frame->port, &my_frame->options); 
	    cfsetispeed(&my_frame->options, B9600);    
	    cfsetospeed(&my_frame->options, B9600); 
	    my_frame->options.c_cflag |= (CLOCAL | CREAD); 
	    tcsetattr(my_frame->port, TCSANOW, &my_frame->options); 

    	sleep (1);
       	write (my_frame->port, "*00WE\r\n", 7);
       	write (my_frame->port, "*00I=R120\r\n", 11);
       	
    	sleep (1);
       	write (my_frame->port, "*00P2\r\n", 7);
    	
    	while (my_frame->thread_status == true) {

			n = read( my_frame->port , &character, 1);
			if (character == '?') {
				inputarray[inptr] = 0; //terminate line
				inptr = 0; 
				inputarray[inptr++] = character;
				if (sync == true) {
					synclock = true;
					
					sscanf ( &inputarray[0], "?%dCP=%f", &device, &pressure);
					dataframe.ppt = pressure * 70.3069549744 ; //- zeropressure; // convert to cm of water at 4 degrees
				}
				else sync = true;
			}
			else if (inptr <= 32) { 
				inputarray[inptr++] = character ;
				inputarray[inptr] = 0;
			}
			else {
				inptr = 0;
				inputarray[inptr++] = character ;
				inputarray[inptr] = 0;
			}

		}
		fprintf (stderr, "> closing honeywell port\n"); fflush(stderr); 

		if ( ppt_pressure.weightsPtr ) free( ppt_pressure.weightsPtr );
		if ( ppt_pressure.filterarrayPtr ) free( ppt_pressure.filterarrayPtr );
	}
	close(my_frame->port);
	my_frame->thread_status = false; 

	pthread_exit(NULL);

}


/****************************************************************************/
/*																			*/
/*  Init external drivers for;												*/
/*		ndd																	*/
/*		ppt																	*/
/****************************************************************************/
// init external drivers

void Init_External(struct xml_data *dataPtr) {
	int j;
	
	fprintf(stderr,"\n> initiating ndd; output on proc 1...\n");
	rc = pthread_create(&nddthread, NULL, NDD_monitor, (void *) &ndd_flow);
	if (rc) {
		fprintf(stderr,"\n >> ERROR; return code from ndd pthread_create() is %d\n", rc);
		exit(-1);
	}
	else {
    	ndd_flow.up_samplerate = dataPtr->build.System_Sample_Rate/NDD_SAMPLE_RATE ; 				// work out the upsample rate
		dataframe.nddF = 0.0;
		ndd_flow.weightsPtr = NULL;
		ndd_flow.filterarrayPtr = NULL;
		ndd_flow.size = 0;
    	if ( ndd_flow.up_samplerate <= 1 ) ndd_flow.up_samplerate = 1;
		else { 							// only set up if up_samplerate > 1
			ndd_flow.size =  ndd_flow.up_samplerate * NO_ZEROS * 2 ; 							// always an odd number 
			ndd_flow.weightsPtr = init_gsinc( ndd_flow.up_samplerate, NO_ZEROS) ; 						// upsample & numpber of zeros
			ndd_flow.filterarrayPtr = (float  *) calloc (ndd_flow.size , sizeof(float));				// 

//	for (j=0; j < ndd_flow.size; j++) fprintf( stderr, "%g ", ndd_flow.weightsPtr[j]); 
//		fprintf( stderr, "\n"); fflush(stderr); 

		}
	}
	
	fprintf(stderr,"\n> initiating ppt; output on proc 2...\n");
	rc = pthread_create(&pptthread, NULL, PPT_monitor, (void *) &ppt_pressure);
	if (rc) {
		fprintf(stderr,"\n >> ERROR; return code from ppt pthread_create() is %d\n", rc);
		exit(-1);
	}
	else {
    	ppt_pressure.up_samplerate = dataPtr->build.System_Sample_Rate/PPT_SAMPLE_RATE ; 				// work out the upsample rate
		dataframe.ppt = 0.0;
		ppt_pressure.weightsPtr = NULL;
		ppt_pressure.filterarrayPtr = NULL;
		ppt_pressure.size = 0;
		if ( ppt_pressure.up_samplerate < 1 ) ppt_pressure.up_samplerate = 1;
		else {	
			ppt_pressure.size = ppt_pressure.up_samplerate * NO_ZEROS * 2 ; 					// always an odd number 
			ppt_pressure.weightsPtr = init_gsinc( ppt_pressure.up_samplerate, NO_ZEROS) ;				// upsample & numpber of zeros
			ppt_pressure.filterarrayPtr = (float  *) calloc (ppt_pressure.size , sizeof(float));		// 

//	for (j=0; j < ppt_pressure.size; j++) fprintf( stderr, "%g ", ppt_pressure.weightsPtr[j]); 
//		fprintf( stderr, "\n"); fflush(stderr); 

		}
	}
}



/****************************************************************************/
/*																			*/
/*  External module currently supports upsampling for:						*/
/*		ndd																	*/
/*		ppt																	*/
/****************************************************************************/


void external(struct module *Module, struct pertecs_build *buildPtr, long time)
{
	int j;

// available inputs
//	Module->Input[0]->value;
//	Module->Input[1]->value;
//	Module->Input[2]->value;

// available parameters
//	Module->Param[0]->value;
//	Module->Param[1]->value;
//	Module->Param[2]->value;
//	Module->Param[3]->value;
//	Module->Param[4]->value;
//	Module->Param[5]->value;

// outputs
// do the ndd
	if (ndd_flow.up_samplerate == 1) Module->Output[0]->value = dataframe.nddF;
	else {
		ndd_flow.sample_cntr = (ndd_flow.sample_cntr + 1) % ndd_flow.up_samplerate;

		ndd_flow.farray_index  = (ndd_flow.farray_index + 1) % ndd_flow.size;							// step along the ring buffer .. next position will be the very earliest entry
		Module->Output[0]->value = (double)  ndd_flow.filterarrayPtr[ndd_flow.farray_index] ;			// so output this and
		ndd_flow.filterarrayPtr[ndd_flow.farray_index] = 0; 											// clear ready for new data to be added
		if (ndd_flow.sample_cntr == 0) {
			for (j=0; j < ndd_flow.size - 1; j++) {						 									// can assume the last weight is 0 zero, so save effort
				ndd_flow.filterarrayPtr[ndd_flow.farray_index] += dataframe.nddF * ndd_flow.weightsPtr[j]; // update the ring buffer by summing in the new input value
				ndd_flow.farray_index  = (ndd_flow.farray_index + 1) % ndd_flow.size; 					
			}
			ndd_flow.farray_index  = (ndd_flow.farray_index + 1) % ndd_flow.size; 						// index should end up where it was so do one last time
		}
	}

// do Honeywell pressure sensor
	if (ppt_pressure.up_samplerate == 1) Module->Output[1]->value = dataframe.ppt;
	else {
		if (ppt_pressure.sample_cntr == 0) {
			for (j=0; j < ppt_pressure.size - 1; j++) {														// can assume the last weight is 0 zero, so save effort
				ppt_pressure.filterarrayPtr[ppt_pressure.farray_index] += dataframe.ppt * ppt_pressure.weightsPtr[j]; // update the ring buffer by summing in the new input value
				ppt_pressure.farray_index  = (ppt_pressure.farray_index + 1) % ppt_pressure.size;
			}
			ppt_pressure.farray_index  = (ppt_pressure.farray_index + 1) % ppt_pressure.size; 		// index should end up where it was so do one last time
		}
		ppt_pressure.sample_cntr = (ppt_pressure.sample_cntr + 1) % ppt_pressure.up_samplerate;

		ppt_pressure.farray_index  = (ppt_pressure.farray_index + 1) % ppt_pressure.size;				// step along the ring buffer
		Module->Output[1]->value = (double) ppt_pressure.filterarrayPtr[ppt_pressure.farray_index] ;	
		ppt_pressure.filterarrayPtr[ppt_pressure.farray_index] = 0; // clear ready for new data
	}

	Module->Output[2]->value = 0;
	Module->Output[3]->value = 0;
	Module->Output[4]->value = 0;
	Module->Output[5]->value = 0;
}

