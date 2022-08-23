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

#include "pertecs.h"

/****************************************************************************/
/*																			*/
/*  initialise the standard Input											*/
/*																			*/
/****************************************************************************/
FILE * Init_Input_Telemetry () {
	FILE *	input_fid = NULL;
	
	if (( input_fid = fopen ( "/dev/stdin", "r" )) == 0 ) {
		fprintf ( stderr, "> Can't open input stream:\n");
		exit (-1) ;
	}

	return (input_fid) ;
}

/****************************************************************************/
/*																			*/
/*  initialise the standard Output											*/
/*																			*/
/****************************************************************************/
FILE * Init_Output_Telemetry () {
	FILE *	output_fid = NULL;
	
	if (( output_fid = fopen ( "/dev/stdout", "w" )) == 0 ) {
		fprintf ( stderr, "> Can't open output stream:\n");
		exit (-1) ;
	}

	return (output_fid) ;
}


/****************************************************************************/
/*																			*/
/*  initialise Input Output Names											*/
/*																			*/
/****************************************************************************/
void InitInputOutputNames(struct xml_data *DataPtr) {

	int i, j;
	
	j = DataPtr->build.build_size; // start the input/output numbering from end of modules  ... might like to change this so that its not dependent on the build

// serial input 
	if (DataPtr->build.Input_channel_no) { // allow for a standalone pertecs... no inputs

		for (i=0; i <= DataPtr->build.Input_channel_no; i++) {  // adding 1 for the frame counter
			DataPtr->IO_Modules[i]->put.In->module_no = j++;
			snprintf( DataPtr->IO_Modules[i]->name , DEFAULT_NAME_SIZE, "%s%c%d",DataPtr->build.Input_label, DELIM,  i); // module label
			snprintf( DataPtr->IO_Modules[i]->put.In->label , DEFAULT_NAME_SIZE, "stdin%c%d", DELIM, i); // signal label
		}
		snprintf( DataPtr->IO_Modules[0]->put.In->label , DEFAULT_NAME_SIZE, "framecounter");
	}
			
	
// serial output 
	if (DataPtr->build.Output_channel_no) {
		for (i=0; i <= DataPtr->build.Output_channel_no; i++) { 
			DataPtr->Output_Modules[i]->put.Out->module_no = j++; 
			snprintf( DataPtr->Output_Modules[i]->name , DEFAULT_NAME_SIZE, "%s%c%d",DataPtr->build.Output_label, DELIM, i); // module label
			snprintf( DataPtr->Output_Modules[i]->put.Out->label , DEFAULT_NAME_SIZE, "stdout%c%d", DELIM, i); // signal label
		}
		snprintf( DataPtr->Output_Modules[0]->put.Out->label , DEFAULT_NAME_SIZE, "framecounter");
	}	

// virtual i/o lugs 
	if (DataPtr->build.Connection_build_size) {
		for (i=0; i < DataPtr->build.Connection_build_size; i++) { 
			DataPtr->lug_Modules[i]->put.In->module_no = j++; 
			snprintf( DataPtr->lug_Modules[i]->name , DEFAULT_NAME_SIZE, "%s%c%d",DataPtr->build.Connection_label, DELIM, i); // module label
			snprintf( DataPtr->lug_Modules[i]->put.In->label , DEFAULT_NAME_SIZE, "termlug%c%d", DELIM, i); // signal label
		}
	}	

}

/****************************************************************************/
/*																			*/
/*  initialise Input Output memory											*/
/*																			*/
/****************************************************************************/
void InitInputOutput(struct xml_data *DataPtr) {

	int i, j;
	
	j = DataPtr->build.build_size; // start the input/output numbering from end of modules  ... might like to change this so that its not dependent on the build

// serial input 
	if (DataPtr->build.Input_channel_no) { // allow for a standalone pertecs... no inputs
		if (DataPtr->build.Input_channel_no > MAX_CHANNELS) {
			fprintf(stderr, "Input telemetry frame exceeds allowance\n"); fflush(stderr);
			exit (1);
		}
		for (i=0; i <= DataPtr->build.Input_channel_no; i++) {  // adding 1 for the frame counter
			DataPtr->IO_Modules[i] = (struct io_module *) calloc (1, sizeof (struct io_module));
			DataPtr->IO_Modules[i]->name = (char *) calloc (DEFAULT_NAME_SIZE, sizeof(char)) ;
			DataPtr->IO_Modules[i]->put.In = (struct field*) calloc (1, sizeof ( struct field ) ) ;
			DataPtr->IO_Modules[i]->put.In->name = (char *) calloc (DEFAULT_NAME_SIZE, sizeof(char)) ;
			DataPtr->IO_Modules[i]->put.In->label = (char *) calloc (DEFAULT_NAME_SIZE, sizeof(char)) ; // allocation of label space
			DataPtr->IO_Modules[i]->put.In->metaref  = (char *) calloc(DEFAULT_NAME_SIZE, sizeof(char));
		}

		// INIT INPUT telemetry, //	
		DataPtr->input_fid = Init_Input_Telemetry() ;

		// set up the input interpolation .... defaults
		DataPtr->interp_size = 0;
		for (i=0; i <= DataPtr->build.Input_channel_no; i++)  DataPtr->interpfilterPtr[i] = NULL;
		DataPtr->weightsPtr = NULL;
		DataPtr->farray_index = 0;
		// then calculate the weights
		if ((DataPtr->in_sampleflg == UP_INSAMPLE_INTERPOLATE)  && (DataPtr->in_resample > 1)){ // set up interpolation weights so long as sample rate > 1
			DataPtr->interp_size = DataPtr->in_resample * NO_ZEROS * 2 ;
			DataPtr->weightsPtr = init_gsinc( DataPtr->in_resample, NO_ZEROS) ; // initialise filter weights
			for (i=1; i <= DataPtr->build.Input_channel_no; i++)  DataPtr->interpfilterPtr[i] = (float  *) calloc (DataPtr->interp_size , sizeof(float));		// 
		}
	}
			
// serial output 

	if (DataPtr->build.Input_channel_no + DataPtr->build.Output_channel_no > 2 * MAX_CHANNELS) {
		fprintf(stderr, "Output frame exceeds allowance\n"); fflush(stderr);
		exit (1);
	}
	if (DataPtr->build.Output_channel_no) {
		for (i=0; i <= DataPtr->build.Output_channel_no; i++) { 
			DataPtr->Output_Modules[i] = (struct io_module *) calloc (1, sizeof (struct io_module));
			DataPtr->Output_Modules[i]->name = (char *) calloc (DEFAULT_NAME_SIZE, sizeof(char)) ;
			DataPtr->Output_Modules[i]->put.Out = (struct field*) calloc (1, sizeof ( struct field )) ;
			DataPtr->Output_Modules[i]->put.Out->name = (char *) calloc (DEFAULT_NAME_SIZE, sizeof(char)) ;
			DataPtr->Output_Modules[i]->put.Out->label = (char *) calloc (DEFAULT_NAME_SIZE, sizeof(char)) ; // allocation of label space
			DataPtr->Output_Modules[i]->put.Out->metaref  = (char *) calloc(DEFAULT_NAME_SIZE, sizeof(char));
		}

		// INIT  OUTPUT telemetry, //	
		DataPtr->output_fid = Init_Output_Telemetry() ;

		DataPtr->frameoutPtr = (float *) malloc (DataPtr->build.Output_channel_no * sizeof(double) );
	}	

	if (DataPtr->build.Input_channel_no + DataPtr->build.Output_channel_no + DataPtr->build.Connection_build_size > 2 * MAX_CHANNELS) {
		fprintf(stderr, "Terminal lugs frame exceeds allowance\n"); fflush(stderr);
		exit (1);
	}
	if (DataPtr->build.Connection_build_size) {
		for (i=0; i < DataPtr->build.Connection_build_size; i++) { 
			DataPtr->lug_Modules[i] = (struct io_module *) calloc (1, sizeof (struct io_module));
			DataPtr->lug_Modules[i]->name = (char *) calloc (DEFAULT_NAME_SIZE, sizeof(char)) ;
			DataPtr->lug_Modules[i]->put.Out = (struct field*) calloc (1, sizeof ( struct field )) ;
			DataPtr->lug_Modules[i]->put.In->name = (char *) calloc (DEFAULT_NAME_SIZE, sizeof(char)) ;
			DataPtr->lug_Modules[i]->put.In->label = (char *) calloc (DEFAULT_NAME_SIZE, sizeof(char)) ; // allocation of label space
			DataPtr->lug_Modules[i]->put.In->metaref  = (char *) calloc(DEFAULT_NAME_SIZE, sizeof(char));
		}
	}	

	InitInputOutputNames(DataPtr);

}

/****************************************************************************/
/*																			*/
/*  initialise Module default Names	(not including parameter names)			*/
/*																			*/
/****************************************************************************/
void InitModuleNames(struct xml_data *DataPtr) {

	int	j, i=0;

// pots
	for (j=0; j < DataPtr->build.Pot_channel_no; j++) {
		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "%s%c%d",  DataPtr->build.Pot_label, DELIM, j); // moved from i to j

		snprintf( DataPtr->Modules[i]->Output[POTINX]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Pot_out_namex);
		snprintf( DataPtr->Modules[i]->Output[POTINY]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Pot_out_namey);
		snprintf( DataPtr->Modules[i]->Output[POTINZ]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Pot_out_namez);
		snprintf( DataPtr->Modules[i]->Output[SELINX]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Pot_sel_namex);
		snprintf( DataPtr->Modules[i]->Output[SELINY]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Pot_sel_namey);
		snprintf( DataPtr->Modules[i]->Output[SELINZ]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Pot_sel_namez);
			DataPtr->Modules[i]->Output[POTINX]->name = DataPtr->build.Pot_out_namex;
			DataPtr->Modules[i]->Output[POTINY]->name = DataPtr->build.Pot_out_namey;
			DataPtr->Modules[i]->Output[POTINZ]->name = DataPtr->build.Pot_out_namez;
			DataPtr->Modules[i]->Output[SELINX]->name = DataPtr->build.Pot_sel_namex;
			DataPtr->Modules[i]->Output[SELINY]->name = DataPtr->build.Pot_sel_namey;
			DataPtr->Modules[i]->Output[SELINZ]->name = DataPtr->build.Pot_sel_namez;

		i++;
	}

// comparators
	for (j=0; j < DataPtr->build.Comp_build_size; j++) {
		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "%s%c%d",  DataPtr->build.Comp_mod_label, DELIM, j); // moved from i to j
		snprintf( DataPtr->Modules[i]->Input[STANDARD]->label, DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Comp_in);
			DataPtr->Modules[i]->Input[STANDARD]->name = DataPtr->build.Comp_in;

		snprintf( DataPtr->Modules[i]->Output[DELAYED]->label, DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Comp_delayed); 
		snprintf( DataPtr->Modules[i]->Output[POS]->label, DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Comp_out); 
		snprintf( DataPtr->Modules[i]->Output[ZERO]->label, DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Comp_zero); 
		snprintf( DataPtr->Modules[i]->Output[NONPOS]->label, DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Comp_nonpos); 
		snprintf( DataPtr->Modules[i]->Output[NEG]->label, DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Comp_neg); 
		snprintf( DataPtr->Modules[i]->Output[NONNEG]->label, DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Comp_nonneg); 
			DataPtr->Modules[i]->Output[DELAYED]->name = DataPtr->build.Comp_delayed; 
			DataPtr->Modules[i]->Output[POS]->name = DataPtr->build.Comp_out; 
			DataPtr->Modules[i]->Output[ZERO]->name = DataPtr->build.Comp_zero; 
			DataPtr->Modules[i]->Output[NONPOS]->name = DataPtr->build.Comp_nonpos; 
			DataPtr->Modules[i]->Output[NEG]->name = DataPtr->build.Comp_neg; 
			DataPtr->Modules[i]->Output[NONNEG]->name = DataPtr->build.Comp_nonneg; 
			

		i++;
	}


//intdiff
	for (j=0; j < DataPtr->build.ID_build_size; j++) {
		
	// module name default is constructed from the class and module number
		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "%s%c%d", DataPtr->build.ID_mod_label, DELIM, j); // default module name  moved from i to j
		
	// input default names constructed by catenating the input name to the module 
		snprintf( DataPtr->Modules[i]->Input[STANDARD]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.ID_in_i);
		snprintf( DataPtr->Modules[i]->Input[SPECIAL]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.ID_in_j);
		snprintf( DataPtr->Modules[i]->Input[SETHOLD]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.ID_in_sethold);
			DataPtr->Modules[i]->Input[STANDARD]->name = DataPtr->build.ID_in_i;
			DataPtr->Modules[i]->Input[SPECIAL]->name = DataPtr->build.ID_in_j;
			DataPtr->Modules[i]->Input[SETHOLD]->name = DataPtr->build.ID_in_sethold;
			
	// same with the outputs
		snprintf( DataPtr->Modules[i]->Output[THRD_DERIV]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.ID_out_a);
		snprintf( DataPtr->Modules[i]->Output[SCND_DERIV]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.ID_out_b);
		snprintf( DataPtr->Modules[i]->Output[FST_DERIV]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.ID_out_c);
		snprintf( DataPtr->Modules[i]->Output[SMOOTHED]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.ID_out_d);
		snprintf( DataPtr->Modules[i]->Output[DELAYED]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.ID_out_e);
		snprintf( DataPtr->Modules[i]->Output[INTEGRATED]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.ID_out_f);	
			DataPtr->Modules[i]->Output[THRD_DERIV]->name = DataPtr->build.ID_out_a;
			DataPtr->Modules[i]->Output[SCND_DERIV]->name = DataPtr->build.ID_out_b;
			DataPtr->Modules[i]->Output[FST_DERIV]->name = DataPtr->build.ID_out_c;
			DataPtr->Modules[i]->Output[SMOOTHED]->name = DataPtr->build.ID_out_d;
			DataPtr->Modules[i]->Output[DELAYED]->name = DataPtr->build.ID_out_e;
			DataPtr->Modules[i]->Output[INTEGRATED]->name = DataPtr->build.ID_out_f;
		
					
		i++;
	}

// math
	for (j=0; j < DataPtr->build.Math_build_size; j++) {

		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "%s%c%d",  DataPtr->build.Math_mod_label, DELIM,j);  // moved from i to j
		snprintf( DataPtr->Modules[i]->Input[STANDARD]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Math_in);
			DataPtr->Modules[i]->Input[STANDARD]->name = DataPtr->build.Math_in;

		snprintf( DataPtr->Modules[i]->Output[DELAYED]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Math_delayed);
		snprintf( DataPtr->Modules[i]->Output[SQRT]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Math_sqrt);
		snprintf( DataPtr->Modules[i]->Output[LOG]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Math_log);
		snprintf( DataPtr->Modules[i]->Output[EXP]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Math_exp);
		snprintf( DataPtr->Modules[i]->Output[SIN]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Math_sin);
		snprintf( DataPtr->Modules[i]->Output[RECIPROCAL]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Math_recip);
			DataPtr->Modules[i]->Output[DELAYED]->name = DataPtr->build.Math_delayed;
			DataPtr->Modules[i]->Output[SQRT]->name = DataPtr->build.Math_sqrt;
			DataPtr->Modules[i]->Output[LOG]->name = DataPtr->build.Math_log;
			DataPtr->Modules[i]->Output[EXP]->name = DataPtr->build.Math_exp;
			DataPtr->Modules[i]->Output[SIN]->name = DataPtr->build.Math_sin;
			DataPtr->Modules[i]->Output[RECIPROCAL]->name = DataPtr->build.Math_recip;

		i++;
	}


//entropy
	for (j=0; j < DataPtr->build.Ent_build_size; j++) {
		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "%s%c%d",  DataPtr->build.Ent_mod_label, DELIM, j); // moved from i to j
		snprintf( DataPtr->Modules[i]->Input[STANDARD]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Ent_in);
			DataPtr->Modules[i]->Input[STANDARD]->name = DataPtr->build.Ent_in;

		snprintf( DataPtr->Modules[i]->Output[DELAYED]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Ent_delayed);
		snprintf( DataPtr->Modules[i]->Output[RAST_ENTROPY]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Ent_out);
			DataPtr->Modules[i]->Output[DELAYED]->name = DataPtr->build.Ent_delayed;
			DataPtr->Modules[i]->Output[RAST_ENTROPY]->name = DataPtr->build.Ent_out;
			
		i++;
	}

// fft
	for (j=0; j < DataPtr->build.FFT_build_size; j++) {

		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "%s%c%d",  DataPtr->build.FFT_mod_label, DELIM, j);	 // moved from i to j

		snprintf( DataPtr->Modules[i]->Input[STANDARD]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.FFT_inx);
		snprintf( DataPtr->Modules[i]->Input[SPECIAL]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.FFT_iny);
		snprintf( DataPtr->Modules[i]->Input[SPECIAL2]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.FFT_sync);
			DataPtr->Modules[i]->Input[STANDARD]->name = DataPtr->build.FFT_inx;
			DataPtr->Modules[i]->Input[SPECIAL]->name = DataPtr->build.FFT_iny;
			DataPtr->Modules[i]->Input[SPECIAL2]->name = DataPtr->build.FFT_sync;
		
		snprintf( DataPtr->Modules[i]->Output[DELAYED]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.FFT_delayedx);
		snprintf( DataPtr->Modules[i]->Output[DELAYED2]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.FFT_delayedy);
		snprintf( DataPtr->Modules[i]->Output[REALFFT]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.FFT_realout);
		snprintf( DataPtr->Modules[i]->Output[IMAGFFT]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.FFT_imagout);
			DataPtr->Modules[i]->Output[DELAYED]->name = DataPtr->build.FFT_delayedx;
			DataPtr->Modules[i]->Output[DELAYED2]->name = DataPtr->build.FFT_delayedy;
			DataPtr->Modules[i]->Output[REALFFT]->name = DataPtr->build.FFT_realout;
			DataPtr->Modules[i]->Output[IMAGFFT]->name = DataPtr->build.FFT_imagout;
			
		i++;
	}


// filter
	for (j=0; j < DataPtr->build.Filter_build_size; j++) {
	
		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "%s%c%d",  DataPtr->build.Filter_mod_label, DELIM, j); // moved from i to j

		snprintf( DataPtr->Modules[i]->Input[STANDARD]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Filter_in);
			DataPtr->Modules[i]->Input[STANDARD]->name = DataPtr->build.Filter_in;
		snprintf( DataPtr->Modules[i]->Output[DELAYED]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Filter_delayed);
		snprintf( DataPtr->Modules[i]->Output[FILTEROUT]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Filter_out);
			DataPtr->Modules[i]->Output[DELAYED]->name = DataPtr->build.Filter_delayed;
			DataPtr->Modules[i]->Output[FILTEROUT]->name = DataPtr->build.Filter_out;
		
		i++;
	}


// polar
	for (j=0; j < DataPtr->build.Plr_build_size; j++) {
	
		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "%s%c%d",  DataPtr->build.Plr_mod_label, DELIM, j); // moved from i to j

		snprintf( DataPtr->Modules[i]->Input[STANDARD]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Plr_xin);
		snprintf( DataPtr->Modules[i]->Input[SPECIAL]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Plr_yin);
		snprintf( DataPtr->Modules[i]->Input[SPECIAL2]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Plr_zin);
			DataPtr->Modules[i]->Input[STANDARD]->name = DataPtr->build.Plr_xin;
			DataPtr->Modules[i]->Input[SPECIAL]->name = DataPtr->build.Plr_yin;
			DataPtr->Modules[i]->Input[SPECIAL2]->name = DataPtr->build.Plr_zin;
		
		snprintf( DataPtr->Modules[i]->Output[DELAYED]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Plr_xdelayed);
		snprintf( DataPtr->Modules[i]->Output[DELAYED2]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Plr_ydelayed);
		snprintf( DataPtr->Modules[i]->Output[DELAYED3]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Plr_zdelayed);
		snprintf( DataPtr->Modules[i]->Output[NORM]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Plr_norm);
		snprintf( DataPtr->Modules[i]->Output[THETA]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Plr_theta);
		snprintf( DataPtr->Modules[i]->Output[PHI]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Plr_phi);
			DataPtr->Modules[i]->Output[DELAYED]->name = DataPtr->build.Plr_xdelayed;
			DataPtr->Modules[i]->Output[DELAYED2]->name = DataPtr->build.Plr_ydelayed;
			DataPtr->Modules[i]->Output[DELAYED3]->name = DataPtr->build.Plr_zdelayed;
			DataPtr->Modules[i]->Output[NORM]->name = DataPtr->build.Plr_norm;
			DataPtr->Modules[i]->Output[THETA]->name = DataPtr->build.Plr_theta;
			DataPtr->Modules[i]->Output[PHI]->name = DataPtr->build.Plr_phi;

		i++;
	}



// delay
	for (j=0; j < DataPtr->build.Delay_build_size; j++) {

		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "%s%c%d",  DataPtr->build.Delay_mod_label, DELIM, j); // moved from i to j

		snprintf( DataPtr->Modules[i]->Input[STANDARD]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Delay_in_name);
			DataPtr->Modules[i]->Input[STANDARD]->name = DataPtr->build.Delay_in_name;
		snprintf( DataPtr->Modules[i]->Output[DELAYED]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Delay_out_name);
			DataPtr->Modules[i]->Output[DELAYED]->name = DataPtr->build.Delay_out_name;

		i++;
	}

// noise
	for (j=0; j < DataPtr->build.Noise_channel_size; j++) {
		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "%s%c%d",  DataPtr->build.Noise_mod_label, DELIM, j); // moved from i to j

		snprintf( DataPtr->Modules[i]->Output[NOISEOUT]->label, DEFAULT_NAME_SIZE, "%s%c%s", DataPtr->Modules[i]->name, DELIM, DataPtr->build.Noise_out_name);
			DataPtr->Modules[i]->Output[NOISEOUT]->name = DataPtr->build.Noise_out_name;

		i++;
	}


// external

	for (j=0; j < DataPtr->build.Extern_build_size; j++) {
		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "%s%c%d",  DataPtr->build.Extern_mod_label, DELIM, j); // moved from i to j

		snprintf( DataPtr->Modules[i]->Input[STANDARD]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Extern_argx_label);
		snprintf( DataPtr->Modules[i]->Input[SPECIAL]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Extern_argy_label);
		snprintf( DataPtr->Modules[i]->Input[SPECIAL2]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Extern_pushz_label);
			DataPtr->Modules[i]->Input[STANDARD]->name = DataPtr->build.Extern_argx_label;
			DataPtr->Modules[i]->Input[SPECIAL]->name = DataPtr->build.Extern_argy_label;
			DataPtr->Modules[i]->Input[SPECIAL2]->name = DataPtr->build.Extern_pushz_label;


		snprintf( DataPtr->Modules[i]->Output[ARG1]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Extern_arg1_label);
		snprintf( DataPtr->Modules[i]->Output[ARG2]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Extern_arg2_label);
		snprintf( DataPtr->Modules[i]->Output[ARG3]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Extern_arg3_label);
		snprintf( DataPtr->Modules[i]->Output[ARG4]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Extern_arg3_label);
		snprintf( DataPtr->Modules[i]->Output[ARG5]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Extern_arg5_label);
		snprintf( DataPtr->Modules[i]->Output[ARG6]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Extern_arg6_label);

		i++;
	}

// trace
	for (j=0; j < DataPtr->build.Trace_build_size; j++) {

		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "%s%c%d",  DataPtr->build.Trace_mod_label, DELIM, j); // moved from i to j


		snprintf( DataPtr->Modules[i]->Input[STANDARD]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Trace_xin_name);
		snprintf( DataPtr->Modules[i]->Input[SPECIAL]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Trace_yin_name);
		snprintf( DataPtr->Modules[i]->Input[SPECIAL2]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Trace_zin_name);
			DataPtr->Modules[i]->Input[STANDARD]->name = DataPtr->build.Trace_xin_name;
			DataPtr->Modules[i]->Input[SPECIAL]->name = DataPtr->build.Trace_yin_name;
			DataPtr->Modules[i]->Input[SPECIAL2]->name = DataPtr->build.Trace_zin_name;

		i++;
	}

// scope
		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "SCOPE"); 	// hard coded name
		snprintf( DataPtr->build.Scope_mod_label, DEFAULT_NAME_SIZE, "SCOPE"); 	// into the build structure too

		snprintf( DataPtr->Modules[i]->Input[STANDARD]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Scope_orienta);
		snprintf( DataPtr->Modules[i]->Input[SPECIAL]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.Scope_orientb);
			DataPtr->Modules[i]->Input[STANDARD]->name = DataPtr->build.Scope_orienta;
			DataPtr->Modules[i]->Input[SPECIAL]->name = DataPtr->build.Scope_orientb;

		i++;
// system
		snprintf( DataPtr->Modules[i]->name, DEFAULT_NAME_SIZE, "SYSTEM"); 	// hard coded name
		snprintf( DataPtr->build.System_mod_label, DEFAULT_NAME_SIZE, "SYSTEM"); 	// into the build structure too

		snprintf( DataPtr->Modules[i]->Input[STANDARD]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.System_pause);
		snprintf( DataPtr->Modules[i]->Input[SPECIAL]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.System_stop);
		snprintf( DataPtr->Modules[i]->Input[SPECIAL2]->label , DEFAULT_NAME_SIZE, "%s%c%s",DataPtr->Modules[i]->name, DELIM, DataPtr->build.System_reset);
			DataPtr->Modules[i]->Input[STANDARD]->name = DataPtr->build.System_pause;
			DataPtr->Modules[i]->Input[SPECIAL]->name = DataPtr->build.System_stop;
			DataPtr->Modules[i]->Input[SPECIAL2]->name = DataPtr->build.System_reset;


}



/****************************************************************************/
/*																			*/
/*  initialise memory allocation for modules								*/
/*																			*/
/****************************************************************************/
void	Create_Module_Space(struct xml_data *DataPtr) {
	int i,j;
	
	if (DataPtr->build.build_size + 2 > MAX_BUILD_SIZE ) { // allow for two extra pseudo modules
		fprintf (stderr, "number of modules exceeds maximum build size\n"); fflush(stderr);
		exit (-1); 
	
	}
	
/* allocate space for modules input/outputs all modules utilise a standardised interface structure whether they are used or not*/	
	for (i=0; i < DataPtr->build.build_size; i++) {
	
		DataPtr->Modules[i] = (struct module*) malloc ( sizeof ( struct module )) ; 
		
		// add input field to module
		for (j=0; j < MAX_MODULE_INPUTS_SIZE; j++)  {
			DataPtr->Modules[i]->no_inputs = MAX_MODULE_INPUTS_SIZE;
			DataPtr->Modules[i]->Input[j] = (struct field*) malloc (sizeof ( struct field )) ; 
			DataPtr->Modules[i]->Input[j]->value = 0.0; DataPtr->Modules[i]->Input[j]->intvalue = 0;			
			DataPtr->Modules[i]->Input[j]->module_no = i; 
			DataPtr->Modules[i]->Input[j]->module_node = j; 
		}

		// outputs
		for (j=0; j < MAX_MODULE_OUTPUTS_SIZE; j++) {
			DataPtr->Modules[i]->no_outputs = MAX_MODULE_OUTPUTS_SIZE;
			DataPtr->Modules[i]->Output[j] = (struct field*) malloc (sizeof ( struct field )) ;		
			DataPtr->Modules[i]->Output[j]->value = 0.0; DataPtr->Modules[i]->Output[j]->intvalue = 0;
			DataPtr->Modules[i]->Output[j]->module_no = i; 
			DataPtr->Modules[i]->Output[j]->module_node = j; 
		}
		
		// buffer
			DataPtr->Modules[i]->input_buffPtr = NULL; // always check before use

		// parameters
		for (j=0; j < MAX_MODULE_PARAMETERS_SIZE; j++) {
			DataPtr->Modules[i]->no_parameters = MAX_MODULE_PARAMETERS_SIZE;
			DataPtr->Modules[i]->ParamPtr[j] = (struct field*) malloc (sizeof ( struct field )) ;	
			DataPtr->Modules[i]->ParamPtr[j]->name = (char *) malloc (DEFAULT_NAME_SIZE * sizeof ( char )) ;
			DataPtr->Modules[i]->ParamPtr[j]->name[0] = 0; // terminate	
			DataPtr->Modules[i]->ParamPtr[j]->label = (char *) malloc (DEFAULT_NAME_SIZE * sizeof ( char )) ;
			DataPtr->Modules[i]->ParamPtr[j]->label[0] = 0; // terminate	
			DataPtr->Modules[i]->ParamPtr[j]->value = 0.0; DataPtr->Modules[i]->ParamPtr[j]->intvalue = 0;
			DataPtr->Modules[i]->ParamPtr[j]->module_no = i; 
			DataPtr->Modules[i]->ParamPtr[j]->module_node = j; 
		}	

		DataPtr->Modules[i]->module_no = i;

		DataPtr->Modules[i]->In_Ptr = 0;

		DataPtr->Modules[i]->name = (char *) malloc (DEFAULT_NAME_SIZE * sizeof(char)) ;
		DataPtr->Modules[i]->name[0] = 0 ;
		for (j=0; j < MAX_MODULE_INPUTS_SIZE; j++) { 
			DataPtr->Modules[i]->Input[j]->label = (char *) malloc (DEFAULT_NAME_SIZE * sizeof(char)) ; DataPtr->Modules[i]->Input[j]->label[0] = 0; 
		}
		for (j=0; j < MAX_MODULE_OUTPUTS_SIZE; j++) { 
			DataPtr->Modules[i]->Output[j]->label = (char *) malloc (DEFAULT_NAME_SIZE * sizeof(char)) ; DataPtr->Modules[i]->Output[j]->label[0] = 0; 
		}
	}



/****************************** module class names, settings, def  module label, input names, output names *********************/
// now we need to preserve the order of the modules.... carefully.... to allow for legacy builds? Not sure this constrant applies any more

	i=0;

// pots
	for (j=0; j < DataPtr->build.Pot_channel_no; j++) {
		DataPtr->Modules[i]->type = PRESETS;
		DataPtr->Modules[i]->buff_sz =  DEFAULT_PRESETS_SZ;	 


		snprintf( DataPtr->Modules[i]->ParamPtr[POTSXINIT]->label, DEFAULT_NAME_SIZE,  "0.0"); 
		snprintf( DataPtr->Modules[i]->ParamPtr[POTSYINIT]->label, DEFAULT_NAME_SIZE,  "0.0"); 
		snprintf( DataPtr->Modules[i]->ParamPtr[POTSZINIT]->label, DEFAULT_NAME_SIZE,  "0.0"); 
		snprintf( DataPtr->Modules[i]->ParamPtr[POTSXENABLE]->label, DEFAULT_NAME_SIZE,  "disable"); 
		snprintf( DataPtr->Modules[i]->ParamPtr[POTSYENABLE]->label, DEFAULT_NAME_SIZE,  "disable"); 
		snprintf( DataPtr->Modules[i]->ParamPtr[POTSZENABLE]->label, DEFAULT_NAME_SIZE,  "disable"); 
			DataPtr->Modules[i]->ParamPtr[POTSXINIT]->value = 0.0 ; 
			DataPtr->Modules[i]->ParamPtr[POTSYINIT]->value = 0.0 ; 
			DataPtr->Modules[i]->ParamPtr[POTSZINIT]->value = 0.0 ;
			DataPtr->Modules[i]->ParamPtr[POTSXENABLE]->value = FALSE ; 
			DataPtr->Modules[i]->ParamPtr[POTSYENABLE]->value = FALSE ; // enable inputs
			DataPtr->Modules[i]->ParamPtr[POTSZENABLE]->value = FALSE ; // 


		DataPtr->Pot_Frame[j] = (struct field *) malloc (sizeof (struct field));
		DataPtr->Pot_Frame[j]->module_no = i; 
		DataPtr->Pot_Frame[j]->name = (char *) malloc (DEFAULT_NAME_SIZE * sizeof(char)) ;
		DataPtr->Pot_Frame[j]->label = (char *) malloc (DEFAULT_NAME_SIZE * sizeof(char)) ; // assign a default label

		snprintf( DataPtr->Modules[i]->ParamPtr[POTSXINIT]->name, DEFAULT_NAME_SIZE,  POTSXINIT_LBL); 
		snprintf( DataPtr->Modules[i]->ParamPtr[POTSYINIT]->name, DEFAULT_NAME_SIZE,  POTSYINIT_LBL); 
		snprintf( DataPtr->Modules[i]->ParamPtr[POTSZINIT]->name, DEFAULT_NAME_SIZE,  POTSZINIT_LBL); 
		snprintf( DataPtr->Modules[i]->ParamPtr[POTSXENABLE]->name, DEFAULT_NAME_SIZE,  POTSXENABLE_LBL); 
		snprintf( DataPtr->Modules[i]->ParamPtr[POTSYENABLE]->name, DEFAULT_NAME_SIZE,  POTSYENABLE_LBL); 
		snprintf( DataPtr->Modules[i]->ParamPtr[POTSZENABLE]->name, DEFAULT_NAME_SIZE,  POTSZENABLE_LBL); 

		snprintf( DataPtr->Pot_Frame[j]->name , DEFAULT_NAME_SIZE, "%s%c%d",DataPtr->build.Pot_label, DELIM, i+1); // module label
		snprintf( DataPtr->Pot_Frame[j]->label , DEFAULT_NAME_SIZE, "%s%c%d",DataPtr->build.Pot_out_namex, DELIM, i+1); // signal label

		i++;
	}

// comparators
	for (j=0; j < DataPtr->build.Comp_build_size; j++) {
		DataPtr->Modules[i]->type = COMPAR;			
		DataPtr->Modules[i]->buff_sz = DEFAULT_COMP_BUFF_SZ;

		snprintf( DataPtr->Modules[i]->ParamPtr[INITIALISATION]->name, DEFAULT_NAME_SIZE,  CMP_INITIALISATION);
		snprintf( DataPtr->Modules[i]->ParamPtr[INITIALISATION2]->name, DEFAULT_NAME_SIZE,  CMP_INITIALISATION2);


		i++;
	}


//intdiff
	for (j=0; j < DataPtr->build.ID_build_size; j++) {
		DataPtr->Modules[i]->type = DIFFINT; // class id
		
		DataPtr->Modules[i]->buff_sz = DEFAULT_DIFINT_BUFF_SZ * 2 + 1; // odd size split buffer
		DataPtr->Modules[i]->spare = 0;

		snprintf( DataPtr->Modules[i]->ParamPtr[INITIALISATION]->name, DEFAULT_NAME_SIZE,  PRM_INITIALISATION);
					
		i++;
	}

// math
	for (j=0; j < DataPtr->build.Math_build_size; j++) {
		DataPtr->Modules[i]->type = MATHMOD;

		DataPtr->Modules[i]->buff_sz = DEFAULT_MATH_BUFF_SZ;
		i++;
	}


//entropy
	for (j=0; j < DataPtr->build.Ent_build_size; j++) {
		DataPtr->Modules[i]->type = QTC;
			
		DataPtr->Modules[i]->buff_sz = DEFAULT_ENT_BUFF_SZ * BUFFERMULTIPLIER; // allow for sliding wndows

		snprintf( DataPtr->Modules[i]->ParamPtr[WINDOW]->name, DEFAULT_NAME_SIZE, PRM_WINDOW);
		snprintf( DataPtr->Modules[i]->ParamPtr[WINDOW_SHIFT]->name, DEFAULT_NAME_SIZE, PRM_WINDOW_SHIFT);
		snprintf( DataPtr->Modules[i]->ParamPtr[START_PART]->name, DEFAULT_NAME_SIZE, PRM_START_PART);
		snprintf( DataPtr->Modules[i]->ParamPtr[STOP_PART]->name, DEFAULT_NAME_SIZE, PRM_STOP_PART);
		snprintf( DataPtr->Modules[i]->ParamPtr[COUNTER]->name, DEFAULT_NAME_SIZE, PRM_COUNTER);

		i++;
	}

// fft
	for (j=0; j < DataPtr->build.FFT_build_size; j++) {
		DataPtr->Modules[i]->type = FFT;
			DataPtr->Modules[i]->buff_sz = DEFAULT_FFT_BUFF_SZ * BUFFERMULTIPLIER ; // fixed window that is sunc/cycled... four buffers required.  computes using floats instead of doubles...
		i++;
	}


// filter
	for (j=0; j < DataPtr->build.Filter_build_size; j++) {
		DataPtr->Modules[i]->type = FILTER;

			DataPtr->Modules[i]->buff_sz = DEFAULT_FIR_BUFF_SZ * BUFFERMULTIPLIER; // has to be changed again if a Ch 

		snprintf( DataPtr->Modules[i]->ParamPtr[FILTERTYPE]->name, DEFAULT_NAME_SIZE,  FILTERTYPE_LBL);
		snprintf( DataPtr->Modules[i]->ParamPtr[PASSBAND]->name, DEFAULT_NAME_SIZE,  PASSBAND_LBL);
		snprintf( DataPtr->Modules[i]->ParamPtr[RIPPLE]->name, DEFAULT_NAME_SIZE,  RIPPLE_LBL);
		snprintf( DataPtr->Modules[i]->ParamPtr[CORNERFREQ1]->name, DEFAULT_NAME_SIZE,  CORNERFREQ1_LBL);
		snprintf( DataPtr->Modules[i]->ParamPtr[CORNERFREQ2]->name, DEFAULT_NAME_SIZE,  CORNERFREQ2_LBL);
		snprintf( DataPtr->Modules[i]->ParamPtr[GAIN]->name, DEFAULT_NAME_SIZE,  GAIN_LBL);
		
		i++;
	}


// polar
	for (j=0; j < DataPtr->build.Plr_build_size; j++) {
		DataPtr->Modules[i]->type = POLAR;

			DataPtr->Modules[i]->buff_sz = DEFAULT_PLR_BUFF_SZ;

		i++;
	}



// delay
	for (j=0; j < DataPtr->build.Delay_build_size; j++) {
		DataPtr->Modules[i]->type = DELINE;

			DataPtr->Modules[i]->buff_sz = DEFAULT_DELY_BUFF_SZ;

		snprintf( DataPtr->Modules[i]->ParamPtr[DELAYMODE]->name, DEFAULT_NAME_SIZE, DELAYMODE_LBL);
		snprintf( DataPtr->Modules[i]->ParamPtr[DELFUNCFILE]->name, DEFAULT_NAME_SIZE, DELFUNCFILE_LBL);
		snprintf( DataPtr->Modules[i]->ParamPtr[DELAYMODE]->label, DEFAULT_NAME_SIZE,  "delay"); 
		snprintf( DataPtr->Modules[i]->ParamPtr[DELFUNCFILE]->label, DEFAULT_NAME_SIZE,  "-"); 
			DataPtr->Modules[i]->ParamPtr[TRACETIMER]->value = 0 ; // delay, 1 function generator
			DataPtr->Modules[i]->ParamPtr[TRACECOLOUR]->value = 0 ; // not defined

		i++;
	}

// noise
	for (j=0; j < DataPtr->build.Noise_channel_size; j++) {
		DataPtr->Modules[i]->type = NOISE;

			DataPtr->Modules[i]->buff_sz = DEFAULT_NOISE_BUFF_SZ;

		i++;
	}

// analogue

	if (DataPtr->build.Audio_build_size == 1) { // only have one ananlog interface module
		DataPtr->Modules[i]->type = AUDIO;
		
		DataPtr->Modules[i]->buff_sz =  DEFAULT_AUDIO_BUFF_SZ;

			DataPtr->Modules[i]->Output[LEFTOUT]->name = DataPtr->build.Audio_leftout;
			DataPtr->Modules[i]->Output[RIGHTOUT]->name = DataPtr->build.Audio_rightout;
			DataPtr->Modules[i]->Output[AOUT]->name = DataPtr->build.Audio_aout;
			DataPtr->Modules[i]->Output[BOUT]->name = DataPtr->build.Audio_bout;
			DataPtr->Modules[i]->Output[COUT]->name = DataPtr->build.Audio_cout;
			DataPtr->Modules[i]->Output[DOUT]->name = DataPtr->build.Audio_dout;

		snprintf( DataPtr->Modules[i]->ParamPtr[0]->name, DEFAULT_NAME_SIZE,  "");
		snprintf( DataPtr->Modules[i]->ParamPtr[1]->name, DEFAULT_NAME_SIZE,  "");
		snprintf( DataPtr->Modules[i]->ParamPtr[2]->name, DEFAULT_NAME_SIZE,  "");
		snprintf( DataPtr->Modules[i]->ParamPtr[3]->name, DEFAULT_NAME_SIZE,  "");
		snprintf( DataPtr->Modules[i]->ParamPtr[4]->name, DEFAULT_NAME_SIZE,  "");
		snprintf( DataPtr->Modules[i]->ParamPtr[5]->name, DEFAULT_NAME_SIZE,  "");

		snprintf( DataPtr->Modules[i]->ParamPtr[0]->label, DEFAULT_NAME_SIZE,  ""); 
		snprintf( DataPtr->Modules[i]->ParamPtr[1]->label, DEFAULT_NAME_SIZE,  ""); 
		snprintf( DataPtr->Modules[i]->ParamPtr[2]->label, DEFAULT_NAME_SIZE,  "");
		snprintf( DataPtr->Modules[i]->ParamPtr[3]->label, DEFAULT_NAME_SIZE,  ""); 
		snprintf( DataPtr->Modules[i]->ParamPtr[4]->label, DEFAULT_NAME_SIZE,  ""); 
		snprintf( DataPtr->Modules[i]->ParamPtr[5]->label, DEFAULT_NAME_SIZE,  ""); 

			DataPtr->Modules[i]->ParamPtr[0]->value = 0 ;
			DataPtr->Modules[i]->ParamPtr[1]->value = 0 ;
			DataPtr->Modules[i]->ParamPtr[2]->value = 0 ;
			DataPtr->Modules[i]->ParamPtr[3]->value = 0 ;
			DataPtr->Modules[i]->ParamPtr[4]->value = 0 ;
			DataPtr->Modules[i]->ParamPtr[5]->value = 0 ;

		i++;
	}

// external

	for (j=0; j < DataPtr->build.Extern_build_size; j++) {
		DataPtr->Modules[i]->type = EXTERN;
		
		DataPtr->Modules[i]->buff_sz =  DEFAULT_EXTRN_BUFF_SZ;

			DataPtr->Modules[i]->Output[ARG1]->name = DataPtr->build.Extern_arg1_label;
			DataPtr->Modules[i]->Output[ARG2]->name = DataPtr->build.Extern_arg2_label;
			DataPtr->Modules[i]->Output[ARG3]->name = DataPtr->build.Extern_arg3_label;
			DataPtr->Modules[i]->Output[ARG4]->name = DataPtr->build.Extern_arg4_label;
			DataPtr->Modules[i]->Output[ARG5]->name = DataPtr->build.Extern_arg5_label;
			DataPtr->Modules[i]->Output[ARG6]->name = DataPtr->build.Extern_arg6_label;

		snprintf( DataPtr->Modules[i]->ParamPtr[EXTERNINADDR]->name, DEFAULT_NAME_SIZE,  EXTERNINADDR_LBL);
		snprintf( DataPtr->Modules[i]->ParamPtr[EXTERNINPORT]->name, DEFAULT_NAME_SIZE,  EXTERNINPORT_LBL);
		snprintf( DataPtr->Modules[i]->ParamPtr[EXTERNOUTADDR]->name, DEFAULT_NAME_SIZE,  EXTERNOUTADDR_LBL);
		snprintf( DataPtr->Modules[i]->ParamPtr[EXTERNOUTIP]->name, DEFAULT_NAME_SIZE,  EXTERNOUTIP_LBL);
		snprintf( DataPtr->Modules[i]->ParamPtr[EXTERNOUTPORT]->name, DEFAULT_NAME_SIZE,  EXTERNOUTPORT_LBL);
		snprintf( DataPtr->Modules[i]->ParamPtr[EXTERNTEXTARG]->name, DEFAULT_NAME_SIZE,  EXTERNTEXTARG_LBL);

		snprintf( DataPtr->Modules[i]->ParamPtr[EXTERNINADDR]->label, DEFAULT_NAME_SIZE,  "192.168.1.45"); 
		snprintf( DataPtr->Modules[i]->ParamPtr[EXTERNINPORT]->label, DEFAULT_NAME_SIZE,  "12700"); 
		snprintf( DataPtr->Modules[i]->ParamPtr[EXTERNOUTADDR]->label, DEFAULT_NAME_SIZE,  "/pushdata"); //push data includes two floats and three optional strings 
		snprintf( DataPtr->Modules[i]->ParamPtr[EXTERNOUTIP]->label, DEFAULT_NAME_SIZE,  ""); 
		snprintf( DataPtr->Modules[i]->ParamPtr[EXTERNOUTPORT]->label, DEFAULT_NAME_SIZE,  ""); 
		snprintf( DataPtr->Modules[i]->ParamPtr[EXTERNTEXTARG]->label, DEFAULT_NAME_SIZE,  ""); 

			DataPtr->Modules[i]->ParamPtr[EXTERNINADDR]->value = 0 ;
			DataPtr->Modules[i]->ParamPtr[EXTERNINPORT]->value = 0 ;
			DataPtr->Modules[i]->ParamPtr[EXTERNOUTADDR]->value = 0 ;
			DataPtr->Modules[i]->ParamPtr[EXTERNOUTIP]->value = 0 ;
			DataPtr->Modules[i]->ParamPtr[EXTERNOUTPORT]->value = 0 ;
			DataPtr->Modules[i]->ParamPtr[EXTERNTEXTARG]->value = 0 ;

		i++;
	}

// trace
	for (j=0; j < DataPtr->build.Trace_build_size; j++) {
		DataPtr->Modules[i]->type = TRACE;
		
		DataPtr->Modules[i]->buff_sz =  DEFAULT_TRACE_BUFF_SZ;	// set this up so it gets the buffer from the xml display input 

		snprintf( DataPtr->Modules[i]->ParamPtr[TRACETIMER]->name, DEFAULT_NAME_SIZE,  TRACETIMER_LBL); 
		snprintf( DataPtr->Modules[i]->ParamPtr[TRACECOLOUR]->name, DEFAULT_NAME_SIZE,  TRACECOLOUR_LBL); 
		snprintf( DataPtr->Modules[i]->ParamPtr[TRACEVISIBLE]->name, DEFAULT_NAME_SIZE,  TRACEVISIBLE_LBL); 
		snprintf( DataPtr->Modules[i]->ParamPtr[TRACESTATIC]->name, DEFAULT_NAME_SIZE,  TRACESTATIC_LBL); 
		snprintf( DataPtr->Modules[i]->ParamPtr[TRACELINETYP]->name, DEFAULT_NAME_SIZE,  TRACELINETYP_LBL); 
		snprintf( DataPtr->Modules[i]->ParamPtr[TRACELINEWGHT]->name, DEFAULT_NAME_SIZE,  TRACELINEWGHT_LBL); 

		snprintf( DataPtr->Modules[i]->ParamPtr[TRACETIMER]->label, DEFAULT_NAME_SIZE,  "-"); 
		snprintf( DataPtr->Modules[i]->ParamPtr[TRACECOLOUR]->label, DEFAULT_NAME_SIZE,  "green"); 
		snprintf( DataPtr->Modules[i]->ParamPtr[TRACEVISIBLE]->label, DEFAULT_NAME_SIZE,  "enable"); 
		snprintf( DataPtr->Modules[i]->ParamPtr[TRACESTATIC]->label, DEFAULT_NAME_SIZE,  "static"); 
		snprintf( DataPtr->Modules[i]->ParamPtr[TRACELINETYP]->label, DEFAULT_NAME_SIZE,  "."); 
		snprintf( DataPtr->Modules[i]->ParamPtr[TRACELINEWGHT]->label, DEFAULT_NAME_SIZE,  "1.0"); 

			DataPtr->Modules[i]->ParamPtr[TRACETIMER]->value = 0 ; 
			DataPtr->Modules[i]->ParamPtr[TRACECOLOUR]->value = 1 ; // green ?
			DataPtr->Modules[i]->ParamPtr[TRACEVISIBLE]->value = TRUE ;
			DataPtr->Modules[i]->ParamPtr[TRACESTATIC]->value = FALSE ; 
			DataPtr->Modules[i]->ParamPtr[TRACELINETYP]->value = 0 ; // point
			DataPtr->Modules[i]->ParamPtr[TRACELINEWGHT]->value = 1.0 ; // 

		i++;
	}

// scope
		DataPtr->Modules[i]->type = SCOPE;

		DataPtr->Modules[i]->buff_sz =  0;

		i++;
// system
		DataPtr->Modules[i]->type = SYSTEM;

		DataPtr->Modules[i]->buff_sz =  0;



		InitModuleNames ( DataPtr );
}









void Create_Module_Buffers(struct xml_data *dataPtr) { 
	int i, j;
	

	for (i=0; i < dataPtr->build.build_size; i++) {
		if (dataPtr->Modules[i]->buff_sz) { // if non zero .... 
			dataPtr->Modules[i]->input_buffPtr = (double*) calloc ( dataPtr->Modules[i]->buff_sz , sizeof ( double )) ; // default size
		}
		else dataPtr->Modules[i]->input_buffPtr = NULL;
	}
}



void CopyModulePresets(struct xml_data *dataPtr) {  // copy current module over to modules space..
// need a strategy to cater for changes... 
	int  i;

	dataPtr->Modules[dataPtr->currentmodule.module_no]->module_no = dataPtr->currentmodule.module_no; // properly numbered !
	dataPtr->Modules[dataPtr->currentmodule.module_no]->type = dataPtr->currentmodule.type;
	strncpy (dataPtr->Modules[dataPtr->currentmodule.module_no]->name, dataPtr->currentmodule.name, DEFAULT_NAME_SIZE);
	
	dataPtr->Modules[dataPtr->currentmodule.module_no]->buff_sz = dataPtr->currentmodule.buff_sz;
	if ((dataPtr->currentmodule.type == QTC) || (dataPtr->currentmodule.type == FFT) || (dataPtr->currentmodule.type == FILTER))  dataPtr->Modules[dataPtr->currentmodule.module_no]->buff_sz *= BUFFERMULTIPLIER ; // allow extra room
	if ((dataPtr->currentmodule.type == FILTER) && (!strcmp(dataPtr->currentmodule.ParamPtr[FILTERTYPE]->label, "Ch")))  dataPtr->Modules[dataPtr->currentmodule.module_no]->buff_sz *= BUFFERSPLIT ; // if Ch then double it again !

	if (dataPtr->currentmodule.type == DIFFINT) dataPtr->Modules[dataPtr->currentmodule.module_no]->buff_sz = dataPtr->Modules[dataPtr->currentmodule.module_no]->buff_sz * BUFFERSPLIT + 1;

	for (i=0; i < MAX_MODULE_PARAMETERS_SIZE; i++) {
		snprintf( dataPtr->Modules[dataPtr->currentmodule.module_no]->ParamPtr[i]->name , DEFAULT_NAME_SIZE, "%s", dataPtr->currentmodule.ParamPtr[i]->name); 
		snprintf( dataPtr->Modules[dataPtr->currentmodule.module_no]->ParamPtr[i]->label , DEFAULT_NAME_SIZE, "%s", dataPtr->currentmodule.ParamPtr[i]->label); 
		dataPtr->Modules[dataPtr->currentmodule.module_no]->ParamPtr[i]->value = dataPtr->currentmodule.ParamPtr[i]->value;
		dataPtr->Modules[dataPtr->currentmodule.module_no]->ParamPtr[i]->intvalue = (int) dataPtr->currentmodule.ParamPtr[i]->value;						// convert the double into an integer variable 

// debugging the select switch on the pots ... still looking 16 dec 2011
//		fprintf (stderr, "current %d %f %s %s \n", i, dataPtr->Modules[dataPtr->currentmodule.module_no]->ParamPtr[i]->value,  dataPtr->Modules[dataPtr->currentmodule.module_no]->ParamPtr[i]->label, dataPtr->Modules[dataPtr->currentmodule.module_no]->ParamPtr[i]->name); fflush(stderr);
//		fprintf (stderr, "loaded %d %f %s %s \n\n", dataPtr->currentmodule.module_no, dataPtr->Modules[dataPtr->currentmodule.module_no]->ParamPtr[i]->value,  dataPtr->Modules[dataPtr->currentmodule.module_no]->ParamPtr[i]->label, dataPtr->Modules[dataPtr->currentmodule.module_no]->ParamPtr[i]->name); fflush(stderr);
		
	}

}


void CopyToCurrent(struct module *modulePtr, struct module *currentmodule) { // copy module over to current module
	int  i;

	currentmodule->module_no = modulePtr->module_no ; // properly numbered !
	currentmodule->type = modulePtr->type ;
	strncpy (currentmodule->name, modulePtr->name , DEFAULT_NAME_SIZE);
	
	if ((currentmodule->type == QTC) || (currentmodule->type == FFT) || (currentmodule->type == FILTER))  currentmodule->buff_sz = modulePtr->buff_sz/ BUFFERMULTIPLIER ; // allow extra room
	else if ((currentmodule->type == FILTER) && (!strcmp(currentmodule->ParamPtr[FILTERTYPE]->label, "Ch")))  currentmodule->buff_sz = modulePtr->buff_sz/BUFFERSPLIT ; // if Ch then double it again !
	else if (currentmodule->type == DIFFINT) currentmodule->buff_sz = (modulePtr->buff_sz - 1) / BUFFERSPLIT ;
	else currentmodule->buff_sz  = modulePtr->buff_sz;

	for (i=0; i < MAX_MODULE_PARAMETERS_SIZE; i++) {
		snprintf(currentmodule->ParamPtr[i]->name  , DEFAULT_NAME_SIZE, "%s", modulePtr->ParamPtr[i]->name); 
		snprintf(currentmodule->ParamPtr[i]->label, DEFAULT_NAME_SIZE, "%s",  modulePtr->ParamPtr[i]->label ); 
		currentmodule->ParamPtr[i]->value = modulePtr->ParamPtr[i]->value ;
		currentmodule->ParamPtr[i]->intvalue = (int) modulePtr->ParamPtr[i]->value  ;						// convert the double into an integer variable 
	}

}



/********************************************************************************************************************/
/*							*/
/*	init xml structures		*/
/*							*/
/********************************************************************************************************************/

void Init_build (struct pertecs_build *buildPtr) {

	buildPtr->build_size = 0;
	buildPtr->status = -1;

	buildPtr->System_mod_label[0]	= 0;
	buildPtr->System_pause[0] 		= 0;
	buildPtr->System_stop[0] 		= 0;
	buildPtr->System_reset[0] 		= 0;
	buildPtr->System_Sample_Rate 	= 0;		// default to milliseconds

	buildPtr->Input_channel_no = 0;
	buildPtr->Input_label[0] = 0; 
	buildPtr->Input_out_name[0] = 0;

	buildPtr->Scope_mod_label[0] 	= 0;
	buildPtr->Scope_orienta[0] 		= 0;
	buildPtr->Scope_orientb[0] 		= 0;

	buildPtr->Pot_channel_no = 0;
	buildPtr->Pot_label[0] = 0;
	buildPtr->Pot_out_namex[0] = 0;
	buildPtr->Pot_out_namey[0] = 0;
	buildPtr->Pot_out_namez[0] = 0;

	buildPtr->Noise_channel_size = 0;
	buildPtr->Noise_mod_label[0] = 0;
	buildPtr->Noise_out_name[0] = 0;

	buildPtr->Output_channel_no = 0;
	buildPtr->Output_label[0] = 0;
	buildPtr->Output_in_name[0] = 0;

	buildPtr->Trace_build_size 		= 0;
	buildPtr->Trace_mod_label[0] 	= 0;
	buildPtr->Trace_xin_name[0] 	= 0;
	buildPtr->Trace_yin_name[0] 	= 0;
	buildPtr->Trace_zin_name[0] 	= 0;
		

	buildPtr->Delay_build_size = 0;
	buildPtr->Delay_mod_label[0] = 0;
	buildPtr->Delay_in_name[0] = 0;
	buildPtr->Delay_in_sethold_name[0] = 0;
	buildPtr->Delay_out_name[0] = 0;

	buildPtr->ID_build_size = 0;
	buildPtr->ID_mod_label[0] = 0;
	buildPtr->ID_out_a[0] = 0;
	buildPtr->ID_out_b[0] = 0;
	buildPtr->ID_out_c[0] = 0;
	buildPtr->ID_out_d[0] = 0;
	buildPtr->ID_out_e[0] = 0;
	buildPtr->ID_out_f[0] = 0;
	buildPtr->ID_in_i[0] = 0;
	buildPtr->ID_in_j[0] = 0;
	buildPtr->ID_in_sethold[0] = 0;
	
	buildPtr->Ent_build_size = 0;
	buildPtr->Ent_mod_label[0] = 0;
	buildPtr->Ent_delayed[0] = 0;
	buildPtr->Ent_out[0] = 0;
	buildPtr->Ent_in[0] = 0;

	buildPtr->Comp_build_size = 0;
	buildPtr->Comp_mod_label[0] = 0;
	buildPtr->Comp_delayed[0] = 0;
	buildPtr->Comp_out[0] = 0;
	buildPtr->Comp_zero[0] = 0;
	buildPtr->Comp_nonpos[0] = 0;
	buildPtr->Comp_neg[0] = 0;
	buildPtr->Comp_nonneg[0] = 0;
	buildPtr->Comp_delayed[0] = 0;

	buildPtr->FFT_build_size = 0;
	buildPtr->FFT_mod_label[0] = 0;
	buildPtr->FFT_inx[0] = 0;
	buildPtr->FFT_iny[0] = 0;
	buildPtr->FFT_sync[0] = 0;
	buildPtr->FFT_realout[0] = 0;
	buildPtr->FFT_imagout[0] = 0;
	buildPtr->FFT_delayedx[0] = 0;
	buildPtr->FFT_delayedy[0] = 0;

	buildPtr->Filter_build_size = 0;	
	buildPtr->Filter_mod_label[0] = 0;
	buildPtr->Filter_delayed[0] = 0;
	buildPtr->Filter_out[0] = 0;
	buildPtr->Filter_in[0] = 0;
	
	buildPtr->Plr_build_size = 0;
	buildPtr->Plr_mod_label[0] = 0;
	buildPtr->Plr_xin[0] = 0;
	buildPtr->Plr_yin[0] = 0;
	buildPtr->Plr_zin[0] = 0;
	buildPtr->Plr_theta[0] = 0;
	buildPtr->Plr_phi[0] = 0;
	buildPtr->Plr_norm[0] = 0;
	buildPtr->Plr_xdelayed[0] = 0;
	buildPtr->Plr_ydelayed[0] = 0;
	buildPtr->Plr_zdelayed[0] = 0;

	buildPtr->Math_build_size = 0;
	buildPtr->Math_mod_label[0] = 0;
	buildPtr->Math_delayed[0] = 0;
	buildPtr->Math_sqrt[0] = 0;
	buildPtr->Math_log[0] = 0;
	buildPtr->Math_exp[0] = 0;
	buildPtr->Math_sin[0] = 0;
	buildPtr->Math_recip[0] = 0;
	buildPtr->Math_in[0] = 0;

	buildPtr->Audio_build_size = 0;
	buildPtr->Audio_mod_label[0] = 0;
	buildPtr->Audio_leftin[0] = 0;
	buildPtr->Audio_rightin[0] = 0;
	buildPtr->Audio_leftout[0] = 0;
	buildPtr->Audio_rightout[0] = 0;
	buildPtr->Audio_aout[0] = 0;
	buildPtr->Audio_bout[0] = 0;
	buildPtr->Audio_cout[0] = 0;
	buildPtr->Audio_dout[0] = 0;

	buildPtr->Extern_build_size = 0;
	buildPtr->Extern_mod_label[0] = 0;
	buildPtr->Extern_argx_label[0] = 0;
	buildPtr->Extern_argy_label[0] = 0;
	buildPtr->Extern_pushz_label[0] = 0;
	buildPtr->Extern_arg1_label[0] = 0;
	buildPtr->Extern_arg2_label[0] = 0;
	buildPtr->Extern_arg3_label[0] = 0;
	buildPtr->Extern_arg4_label[0] = 0;
	buildPtr->Extern_arg5_label[0] = 0;
	buildPtr->Extern_arg6_label[0] = 0;


	strcpy(buildPtr->ext_build, "build"); // default
	buildPtr->ext_ionms[0] = 0;
	buildPtr->ext_prests[0] = 0;
	buildPtr->ext_init[0] = 0;
	buildPtr->ext_circ[0] = 0;
	buildPtr->ext_filter[0] = 0;
	buildPtr->ext_display[0] = 0;
	buildPtr->ext_views[0] = 0;
	
	buildPtr->Origin_X= 100				;
	buildPtr->Origin_Y = 100			;
	buildPtr->WIdth	= 1200				;
	buildPtr->Height = 600				;

	buildPtr->background_r = 0.0;		// init scope entities
	buildPtr->background_g = 0.0;		// 
	buildPtr->background_b = 0.0;		// 
}


// redo for the io_modules
void Init_ionames (struct io_module *currentPtr) {

	currentPtr->module_no = -1;
	currentPtr->name[0] = 0;
	currentPtr->put.In->name[0] = 0;
	currentPtr->put.In->label[0] = 0;
	currentPtr->put.In->metaref[0] = 0;
}



void Init_presets (struct module *currentPtr) {

	currentPtr->module_no = -1;
	currentPtr->type = -1;
	currentPtr->name[0] = 0;

	currentPtr->ParamPtr[0]->module_no = -1;
	currentPtr->ParamPtr[0]->name[0] = 0;
	currentPtr->ParamPtr[0]->label[0] = 0;
	currentPtr->ParamPtr[0]->intvalue = 0;
	currentPtr->ParamPtr[0]->value = 0;

	currentPtr->ParamPtr[1]->module_no = -1;
	currentPtr->ParamPtr[1]->name[0] = 0;
	currentPtr->ParamPtr[1]->label[0] = 0;
	currentPtr->ParamPtr[1]->intvalue = 0;
	currentPtr->ParamPtr[1]->value = 0;

	currentPtr->ParamPtr[2]->module_no = -1;
	currentPtr->ParamPtr[2]->name[0] = 0;
	currentPtr->ParamPtr[2]->label[0] = 0;
	currentPtr->ParamPtr[2]->intvalue = 0;
	currentPtr->ParamPtr[2]->value = 0;

	currentPtr->ParamPtr[3]->module_no = -1;
	currentPtr->ParamPtr[3]->name[0] = 0;
	currentPtr->ParamPtr[3]->label[0] = 0;
	currentPtr->ParamPtr[3]->intvalue = 0;
	currentPtr->ParamPtr[3]->value = 0;

	currentPtr->ParamPtr[4]->module_no = -1;
	currentPtr->ParamPtr[4]->name[0] = 0;
	currentPtr->ParamPtr[4]->label[0] = 0;
	currentPtr->ParamPtr[4]->intvalue = 0;
	currentPtr->ParamPtr[4]->value = 0;

	currentPtr->ParamPtr[5]->module_no = -1;
	currentPtr->ParamPtr[5]->name[0] = 0;
	currentPtr->ParamPtr[5]->label[0] = 0;
	currentPtr->ParamPtr[5]->intvalue = 0;
	currentPtr->ParamPtr[5]->value = 0;

	currentPtr->buff_sz = -1;
}



void init_connection (struct connection *connectionPtr ) {
	
	connectionPtr->No = -1;
	connectionPtr->input_No = -1;
	connectionPtr->input_module[0] = 0;
	connectionPtr->input_label[0] = 0;
	connectionPtr->input_name[0] = 0;
	
	connectionPtr->InputPtr = NULL;
		
	connectionPtr->output1_No = -1;
	connectionPtr->output2_No = -1;
	connectionPtr->output3_No = -1;
	connectionPtr->output4_No = -1;
	connectionPtr->output5_No = -1;
	connectionPtr->output6_No = -1;

	connectionPtr->output1_module[0] = 0;
	connectionPtr->output2_module[0] = 0;
	connectionPtr->output3_module[0] = 0;
	connectionPtr->output4_module[0] = 0;
	connectionPtr->output5_module[0] = 0;
	connectionPtr->output6_module[0] = 0;

	connectionPtr->output1_label[0] = 0;
	connectionPtr->output2_label[0] = 0;
	connectionPtr->output3_label[0] = 0;
	connectionPtr->output4_label[0] = 0;
	connectionPtr->output5_label[0] = 0;
	connectionPtr->output6_label[0] = 0;

	connectionPtr->output1_name[0] = 0;
	connectionPtr->output2_name[0] = 0;
	connectionPtr->output3_name[0] = 0;
	connectionPtr->output4_name[0] = 0;
	connectionPtr->output5_name[0] = 0;
	connectionPtr->output6_name[0] = 0;

	connectionPtr->OutputPtr = NULL;
	
	connectionPtr->weight = 0.0;
	connectionPtr->offset = 0.0;
};


