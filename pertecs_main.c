/*
# PERTECS (Programmable Environment for Real-Time emulation of Continuous Systems) is a suite of tools. At the core of this suite is an executable UNIX tool, configured using the language of analogue computer, to give flexible real-time data processing capabilities
# Copyright (C) 2008,2009, 2010  Author, Mark R. Titchener, email mark@tcode.auckland.ac.nz
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

/************************************************************************************/
/*	PERTECS																			*/
/*	Programmable environment for Real-Time Emulation of Continuous Systems			*/
/*																					*/
/************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <GLUT/glut.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <math.h>


#include "pertecs.h"

	int threadcount = 0;		// no longer needing  this ?????
	struct xml_data	pertecs_data;



#define INTEGER	2
#define	FLOAT	4

long  iframe_counter = 0, oframe_counter = 0;
static int	subframe_counter = 0, subframe_in_counter = 0;
double sleep_fraction = 0.25;

/****************************************************************************/
/*																			*/
/*  Input Data etc					 										*/
/*																			*/
/****************************************************************************/

int	InputDataFrame(struct xml_data *DataPtr) {

	int	channel, done = 0, j;
	float tempfloat;
	float *tempfloatPtr;
	short int	tempinteger;

// standard 
	if (DataPtr->build.Input_channel_no) {
		if (DataPtr->in_sampleflg == NORMAL)  { // sampling at specified rate
			if (DataPtr->rawinflg == FALSE) { // Input the whole as a text frame.
				done = fscanf(DataPtr->input_fid, " %ld",&DataPtr->IO_Modules[0]->put.In->intvalue); 					// getting the frame counter
				for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) {
					done = fscanf(DataPtr->input_fid, " %f",&tempfloat);
          DataPtr->IO_Modules[channel]->put.In->value = (double) tempfloat;
					DataPtr->mirror_IO_Modules[channel]  = tempfloat;
				}
				iframe_counter =  DataPtr->IO_Modules[0]->put.In->intvalue ; 
			}
			else if ((DataPtr->rawinflg == FLOAT) || (DataPtr->rawinflg == TRUE)) {  //Input as floats will add a future feature to allow for doubles
				iframe_counter++;
				for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) { 
					fread(&tempfloat, sizeof (float), 1,DataPtr->input_fid );
          DataPtr->IO_Modules[channel]->put.In->value = (double) tempfloat; 
					DataPtr->mirror_IO_Modules[channel] = tempfloat;
				}
			}
			else if (DataPtr->rawinflg == INTEGER) {
				iframe_counter++;
				for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) { 
					fread(&tempinteger, sizeof (short int), 1,DataPtr->input_fid ); 
					DataPtr->IO_Modules[channel]->put.In->value = (double) tempinteger; 
					DataPtr->mirror_IO_Modules[channel] = (float) tempinteger;
				}
			}
		} 

// -Ui switch
		else if (DataPtr->in_sampleflg == UP_INSAMPLE) { // upsample but with sample stuffing zeros
			if (DataPtr->rawinflg == FALSE) { // Input text frame.
				iframe_counter++;			
				if ((subframe_in_counter >= DataPtr->in_resample - 1) || (subframe_in_counter < 0 )) {
					done = fscanf(DataPtr->input_fid, " %ld",&DataPtr->IO_Modules[0]->put.In->intvalue); // read frame counter
					for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) {
					//	DataPtr->last_InputFrame[channel] = DataPtr->mirror_IO_Modules[channel]; // save last read value
						done = fscanf(DataPtr->input_fid, " %f",&tempfloat); 
						DataPtr->mirror_IO_Modules[channel]  = tempfloat;
						DataPtr->IO_Modules[channel]->put.In->value = (double) tempfloat;
					}
					subframe_in_counter = 0; // reset counter
				} else {
					subframe_in_counter++;
					for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) DataPtr->IO_Modules[channel]->put.In->value = 0;		// sample stuffing
				}
			}
			else if ((DataPtr->rawinflg == FLOAT) || (DataPtr->rawinflg == TRUE)) {  //Input as floats will add a future feature to allow for doubles
				iframe_counter++;
				if ((subframe_in_counter >= DataPtr->in_resample - 1) || (subframe_in_counter < 0 )) {
					for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) { 
						fread(&tempfloat, sizeof (float), 1,DataPtr->input_fid );
            DataPtr->IO_Modules[channel]->put.In->value = (double) tempfloat; // sample stuffing
						DataPtr->mirror_IO_Modules[channel] = tempfloat;
					}
					subframe_in_counter = 0; // reset counter
				} else {
					subframe_in_counter++;
					for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) DataPtr->IO_Modules[channel]->put.In->value = 0;
				}
			}
			else if (DataPtr->rawinflg == INTEGER) {
				iframe_counter++;
				if ((subframe_in_counter >= DataPtr->in_resample - 1) || (subframe_in_counter < 0 )) {
					for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) { 
						fread(&tempinteger, sizeof (short int), 1,DataPtr->input_fid ); 
						DataPtr->mirror_IO_Modules[channel] = (float) tempinteger;
						DataPtr->IO_Modules[channel]->put.In->value = (double) tempinteger; 
					}
					subframe_in_counter = 0; // reset counter
				} else {
					subframe_in_counter++;
					for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) DataPtr->IO_Modules[channel]->put.In->value = 0; // sample stuffing
				}
			}
		
		}

// -ui switch
		else if (DataPtr->in_sampleflg == UP_INSAMPLE_INTERPOLATE) { // upsample with gaussian sinc interpolation		
			if (DataPtr->rawinflg == FALSE) { // Input text frame.
				iframe_counter++;			
				if ((subframe_in_counter >= DataPtr->in_resample - 1) || (subframe_in_counter < 0 )) {
					done = fscanf(DataPtr->input_fid, " %ld",&DataPtr->IO_Modules[0]->put.In->intvalue); // read frame counter

					for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) {
						done = fscanf(DataPtr->input_fid, " %f",&tempfloat); 
						DataPtr->mirror_IO_Modules[channel]  = tempfloat;

						// enter data to filter array, weighted accordingly
						tempfloatPtr = DataPtr->interpfilterPtr[channel];
						for (j=0; j < DataPtr->interp_size - 1; j++) {					 									
							tempfloatPtr[DataPtr->farray_index] += tempfloat * DataPtr->weightsPtr[j]; 
							DataPtr->farray_index  = (DataPtr->farray_index + 1) % DataPtr->interp_size; 					
						}
						DataPtr->farray_index  = (DataPtr->farray_index + 1) % DataPtr->interp_size;  // and again						
					}
					subframe_in_counter = -1; // reset counter
				} 
				subframe_in_counter++;
				DataPtr->farray_index  = (DataPtr->farray_index + 1) % DataPtr->interp_size ;										// step along the ring buffer .. next position will be the very earliest entry
				for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) {
//					tempfloatPtr = DataPtr->interpfilterPtr[channel];
					DataPtr->IO_Modules[channel]->put.In->value = (double) (DataPtr->interpfilterPtr[channel])[DataPtr->farray_index] ;								// so output this and
					(DataPtr->interpfilterPtr[channel])[DataPtr->farray_index] = 0 ; 													// clear ready for new data to be added
				}

			}
			else if ((DataPtr->rawinflg == FLOAT) || (DataPtr->rawinflg == TRUE)) {  //Input as floats will add a future feature to allow for doubles
				iframe_counter++;
				if ((subframe_in_counter >= DataPtr->in_resample - 1) || (subframe_in_counter < 0 )) {
					for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) { 
						fread(&tempfloat, sizeof (float), 1,DataPtr->input_fid ); DataPtr->IO_Modules[channel]->put.In->value = (double) tempfloat; 
						DataPtr->mirror_IO_Modules[channel] = tempfloat;


						// enter data to filter array, weighted accordingly
						tempfloatPtr = DataPtr->interpfilterPtr[channel];
						for (j=0; j < DataPtr->interp_size - 1; j++) {					 									
							tempfloatPtr[DataPtr->farray_index] += tempfloat * DataPtr->weightsPtr[j]; 
							DataPtr->farray_index  = (DataPtr->farray_index + 1) % DataPtr->interp_size; 					
						}
						DataPtr->farray_index  = (DataPtr->farray_index + 1) % DataPtr->interp_size;  // and again						
					}
					subframe_in_counter = -1; // reset counter
				} 
				subframe_in_counter++;
				DataPtr->farray_index  = (DataPtr->farray_index + 1) % DataPtr->interp_size ;										// step along the ring buffer .. next position will be the very earliest entry
				for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) {
//					tempfloatPtr = DataPtr->interpfilterPtr[channel];
					DataPtr->IO_Modules[channel]->put.In->value = (double) (DataPtr->interpfilterPtr[channel])[DataPtr->farray_index] ;								// so output this and
					(DataPtr->interpfilterPtr[channel])[DataPtr->farray_index] = 0 ; 													// clear ready for new data to be added
				}
				
			}
			else if (DataPtr->rawinflg == INTEGER) {
				iframe_counter++;
				for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++)
				if ((subframe_in_counter >= DataPtr->in_resample - 1) || (subframe_in_counter < 0 )) {
					for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) { 
						fread(&tempinteger, sizeof (short int), 1,DataPtr->input_fid ); 
            tempfloat = (float) tempinteger ;
						DataPtr->mirror_IO_Modules[channel] = tempfloat ;
						DataPtr->IO_Modules[channel]->put.In->value = (double) tempfloat ; 

						// enter data to filter array, weighted accordingly
						tempfloatPtr = DataPtr->interpfilterPtr[channel];
						for (j=0; j < DataPtr->interp_size - 1; j++) {					 									
							tempfloatPtr[DataPtr->farray_index] += tempfloat * DataPtr->weightsPtr[j]; 
							DataPtr->farray_index  = (DataPtr->farray_index + 1) % DataPtr->interp_size; 					
						}
						DataPtr->farray_index  = (DataPtr->farray_index + 1) % DataPtr->interp_size;  // and again						
					}
					subframe_in_counter = -1; // reset counter
				} 
				subframe_in_counter++;
				DataPtr->farray_index  = (DataPtr->farray_index + 1) % DataPtr->interp_size ;										// step along the ring buffer .. next position will be the very earliest entry
				for (channel=1; channel <= DataPtr->build.Input_channel_no; channel++) {
//					tempfloatPtr = DataPtr->interpfilterPtr[channel];
					DataPtr->IO_Modules[channel]->put.In->value = (double) (DataPtr->interpfilterPtr[channel])[DataPtr->farray_index] ;								// so output this and
					(DataPtr->interpfilterPtr[channel])[DataPtr->farray_index] = 0 ; 													// clear ready for new data to be added
				}

			}
		
		}

		// copy accross the time field		
	if ( feof(DataPtr->input_fid)) { done = EOF; fprintf(stderr, "end of input\n"); fflush(stderr); }
	}
	else { iframe_counter++; }
	return (done);
}



/****************************************************************************/
/*																			*/
/*  output downsampling etc	(upsampling & strobing)	 						*/
/*																			*/
/****************************************************************************/
void	OutputDataFrame( struct xml_data *DataPtr) {

	int i;
	short int	tempinteger;
	
	if (DataPtr->build.Output_channel_no) {
		if ((DataPtr->out_sampleflg != DOWNSAMPLE) && (DataPtr->out_sampleflg != UP_OUTSAMPLE) && (DataPtr->out_sampleflg != STROBE) && (DataPtr->out_sampleflg != STROBEOSC)) { // assume either no downsampling or averaging with downsampling
			if (DataPtr->rawoutflg == FALSE) { // output the whole as a text frame.
				if (subframe_counter == 0) {
					fprintf(DataPtr->output_fid, "\n%ld ", DataPtr->Output_Modules[0]->put.Out->intvalue = oframe_counter++ ); 					// output time (channel 0)
					for (i=1; i <= DataPtr->build.Output_channel_no; i++) {
						fprintf(DataPtr->output_fid, "%8g ", (float) ( DataPtr->Output_Modules[i]->put.Out->value/DataPtr->out_resample));  		//  output channel averaged over the resampling rate
						DataPtr->mirror_Output_Modules[i] = DataPtr->Output_Modules[i]->put.Out->value;
						DataPtr->Output_Modules[i]->put.Out->value = 0.0; 											// then zero out the result
					}
	
				}
				subframe_counter = (subframe_counter + 1) % DataPtr->out_resample;
				
			} else if ((DataPtr->rawoutflg == TRUE) || ( DataPtr->rawoutflg == FLOAT)){ // output the whole as a float raw frame.
				if (subframe_counter == 0) {
					DataPtr->Output_Modules[0]->put.Out->intvalue = oframe_counter++ ; 					// output time (channel 0)
					for (i=1; i <= DataPtr->build.Output_channel_no; i++) {
						DataPtr->frameoutPtr[i-1] = (float) ( DataPtr->Output_Modules[i]->put.Out->value/DataPtr->out_resample) ; 				// averaged over the resampling rate
						DataPtr->mirror_Output_Modules[i] = DataPtr->Output_Modules[i]->put.Out->value;
						DataPtr->Output_Modules[i]->put.Out->value = 0.0;
					}
					fwrite(DataPtr->frameoutPtr, DataPtr->build.Output_channel_no * sizeof(float), 1, DataPtr->output_fid);
				}
				subframe_counter = (subframe_counter + 1) % DataPtr->out_resample;
			
			}
			else if (DataPtr->rawoutflg == INTEGER) {					// output the whole as a integer raw frame.
				if (subframe_counter == 0) {
					DataPtr->Output_Modules[0]->put.Out->intvalue = oframe_counter++ ; 					// output time (channel 0)
					for (i=1; i <= DataPtr->build.Output_channel_no; i++) {
						DataPtr->frameoutPtr[i-1] = (float) ( DataPtr->Output_Modules[i]->put.Out->value/DataPtr->out_resample) ; 				// averaged over the resampling rate
						tempinteger = (short int) DataPtr->frameoutPtr[i-1];
						fwrite(&tempinteger, sizeof(short int), 1, DataPtr->output_fid);
						DataPtr->mirror_Output_Modules[i] = DataPtr->Output_Modules[i]->put.Out->value;
						DataPtr->Output_Modules[i]->put.Out->value = 0.0;
					}
				}
				subframe_counter = (subframe_counter + 1) % DataPtr->out_resample;			
			}
		} else if (DataPtr->out_sampleflg == DOWNSAMPLE) { // real Downsampling, no averaging
			if (DataPtr->rawoutflg == FALSE) { // output the whole as a text frame.
				if (subframe_counter == 0) {
					fprintf(DataPtr->output_fid, "\n%ld ", DataPtr->Output_Modules[0]->put.Out->intvalue = oframe_counter++ ); 					// output time (channel 0)
					for (i=1; i <= DataPtr->build.Output_channel_no; i++) fprintf(DataPtr->output_fid, "%g ", DataPtr->mirror_Output_Modules[i] = (float) ( DataPtr->Output_Modules[i]->put.Out->value)); 
				}
				
			} else if ((DataPtr->rawoutflg == TRUE) || ( DataPtr->rawoutflg == FLOAT)){ // output the whole as a float raw frame.
				if (subframe_counter == 0) {
					DataPtr->Output_Modules[0]->put.Out->intvalue = oframe_counter++ ; 					// output time (channel 0)
					for (i=1; i <= DataPtr->build.Output_channel_no; i++) DataPtr->mirror_Output_Modules[i] = DataPtr->frameoutPtr[i-1] = (float) ( DataPtr->Output_Modules[i]->put.Out->value) ;
					fwrite(DataPtr->frameoutPtr, DataPtr->build.Output_channel_no * sizeof(float), 1, DataPtr->output_fid);
				}
			
			}
			else if (DataPtr->rawoutflg == INTEGER) {					// output the whole as a integer raw frame
				if (subframe_counter == 0) {
					DataPtr->Output_Modules[0]->put.Out->intvalue = oframe_counter++ ; 					// output time (channel 0)
					for (i=1; i <= DataPtr->build.Output_channel_no; i++) {
						DataPtr->mirror_Output_Modules[i] = DataPtr->frameoutPtr[i-1] = (float) ( DataPtr->Output_Modules[i]->put.Out->value ) ;
						tempinteger = (short int) DataPtr->frameoutPtr[i-1];
						fwrite(&tempinteger, sizeof(short int), 1, DataPtr->output_fid);
					}
				}
			}
			for (i=1; i <= DataPtr->build.Output_channel_no; i++) {
				DataPtr->mirror_Output_Modules[i] = DataPtr->Output_Modules[i]->put.Out->value;
				DataPtr->Output_Modules[i]->put.Out->value = 0.0; 
			}
			subframe_counter = (subframe_counter + 1) % DataPtr->out_resample;
		} else if (DataPtr->out_sampleflg == STROBE) { // strobes on chaneel 1 == (1)
			if (DataPtr->Output_Modules[1]->put.Out->value == 1.0) {
				if (DataPtr->rawoutflg == FALSE) { // output the whole as a text frame.
					if (subframe_counter == 0) {
						fprintf(DataPtr->output_fid, "\n%ld ", DataPtr->Output_Modules[0]->put.Out->intvalue = oframe_counter++ ); 					// output time (channel 0)
						for (i=1; i <= DataPtr->build.Output_channel_no; i++) fprintf(DataPtr->output_fid, "%g ", DataPtr->mirror_Output_Modules[i] = (float) ( DataPtr->Output_Modules[i]->put.Out->value)); 
					}
					
				} else if ((DataPtr->rawoutflg == TRUE) || ( DataPtr->rawoutflg == FLOAT)){ // output the whole as a float raw frame.
					if (subframe_counter == 0) {
						DataPtr->Output_Modules[0]->put.Out->intvalue = oframe_counter++ ; 					// output time (channel 0)
						for (i=1; i <= DataPtr->build.Output_channel_no; i++) DataPtr->mirror_Output_Modules[i] = DataPtr->frameoutPtr[i-1] = (float) ( DataPtr->Output_Modules[i]->put.Out->value) ;
						fwrite(DataPtr->frameoutPtr, DataPtr->build.Output_channel_no * sizeof(float), 1, DataPtr->output_fid);
					}
				
				}
				else if (DataPtr->rawoutflg == INTEGER) {					// output the whole as a integer raw frame
					if (subframe_counter == 0) {
						DataPtr->Output_Modules[0]->put.Out->intvalue = oframe_counter++ ; 					// output time (channel 0)
						for (i=1; i <= DataPtr->build.Output_channel_no; i++) {
							DataPtr->mirror_Output_Modules[i] = DataPtr->frameoutPtr[i-1] = (float) ( DataPtr->Output_Modules[i]->put.Out->value ) ;
							tempinteger = (short int) DataPtr->frameoutPtr[i-1];
							fwrite(&tempinteger, sizeof(short int), 1, DataPtr->output_fid);
						}
					}
				}
				for (i=1; i <= DataPtr->build.Output_channel_no; i++) {
					DataPtr->mirror_Output_Modules[i] = DataPtr->Output_Modules[i]->put.Out->value;
					DataPtr->Output_Modules[i]->put.Out->value = 0.0; 
				}
				subframe_counter = (subframe_counter + 1) % DataPtr->out_resample;
			} else  {
				for (i=1; i <= DataPtr->build.Output_channel_no; i++) {
					DataPtr->mirror_Output_Modules[i] = (float) ( DataPtr->Output_Modules[i]->put.Out->value); 
					DataPtr->Output_Modules[i]->put.Out->value = 0.0;
				}
			}
		} else if (DataPtr->out_sampleflg == STROBEOSC) { // strobes on chaneel 1 == (1) but outputs channels 2,... on \ddata OSC format
			if (DataPtr->Output_Modules[1]->put.Out->value == 1.0) { // only outputs  text
			
				if (DataPtr->rawoutflg == FALSE) { // output the whole as a text frame.
					if (subframe_counter == 0) {
						for (i=2; i <= DataPtr->build.Output_channel_no; i++)  {
							DataPtr->mirror_Output_Modules[i] = (float) ( DataPtr->Output_Modules[i]->put.Out->value);
						}
						ddataframe("/ddata","127.0.0.1", portdoscnumber, DataPtr->build.Output_channel_no - 1, &DataPtr->Output_Modules[2]); // don't include the first channel which is the strobe =1 
					}
				} 
				for (i=1; i <= DataPtr->build.Output_channel_no; i++) {
					DataPtr->mirror_Output_Modules[i] = DataPtr->Output_Modules[i]->put.Out->value;
					DataPtr->Output_Modules[i]->put.Out->value = 0.0; 
				}
				subframe_counter = (subframe_counter + 1) % DataPtr->out_resample;
			} else  {
				for (i=1; i <= DataPtr->build.Output_channel_no; i++) {
					DataPtr->mirror_Output_Modules[i] = (float) ( DataPtr->Output_Modules[i]->put.Out->value); 
					DataPtr->Output_Modules[i]->put.Out->value = 0.0;
				}
			}
		} else if (DataPtr->out_sampleflg == UP_OUTSAMPLE) { // real Upsampling
			
			if (DataPtr->rawoutflg == FALSE) { // output the whole as a text frame.
				for (subframe_counter = 0;  subframe_counter < DataPtr->out_resample; subframe_counter++) {
					fprintf(DataPtr->output_fid, "\n%ld ", DataPtr->Output_Modules[0]->put.Out->intvalue = oframe_counter++ ); 					// output time (channel 0)
					for (i=1; i <= DataPtr->build.Output_channel_no; i++) {
						fprintf(DataPtr->output_fid, "%g ", (float) ( DataPtr->mirror_Output_Modules[i] +  subframe_counter * (DataPtr->Output_Modules[i]->put.Out->value - DataPtr->mirror_Output_Modules[i])/DataPtr->out_resample));  		
					}
				}			
			} else if ((DataPtr->rawoutflg == TRUE) || ( DataPtr->rawoutflg == FLOAT)){ // output the whole as a float raw frame.
				for (subframe_counter = 0;  subframe_counter < DataPtr->out_resample; subframe_counter++) {
					DataPtr->Output_Modules[0]->put.Out->intvalue = oframe_counter++ ; 					// output time (channel 0)
					for (i=1; i <= DataPtr->build.Output_channel_no; i++) DataPtr->frameoutPtr[i-1] = (float) DataPtr->mirror_Output_Modules[i] +  subframe_counter * (DataPtr->Output_Modules[i]->put.Out->value - DataPtr->mirror_Output_Modules[i])/DataPtr->out_resample ;
					fwrite(DataPtr->frameoutPtr, DataPtr->build.Output_channel_no * sizeof(float), 1, DataPtr->output_fid);
				}
			
			}
			else if (DataPtr->rawoutflg == INTEGER) {					// output the whole as a integer raw frame
				for (subframe_counter = 0;  subframe_counter < DataPtr->out_resample; subframe_counter++) {
					DataPtr->Output_Modules[0]->put.Out->intvalue = oframe_counter++ ; 					// output time (channel 0)
					for (i=1; i <= DataPtr->build.Output_channel_no; i++) {
						DataPtr->frameoutPtr[i-1] = (float) ( DataPtr->Output_Modules[i]->put.Out->value ) ;
						tempinteger = (short int) DataPtr->frameoutPtr[i-1];
						fwrite(&tempinteger, sizeof(short int), 1, DataPtr->output_fid);
					}
				}
			}
			
			for (i=1; i <= DataPtr->build.Output_channel_no; i++) {
				DataPtr->mirror_Output_Modules[i] = DataPtr->Output_Modules[i]->put.Out->value;
				DataPtr->Output_Modules[i]->put.Out->value = 0.0; 											// then zero out the result	
			}	
		}
		fflush(DataPtr->output_fid); // complete the frame
	}
}

/****************************************************************************/
/*																			*/
/*  pots thread	launched from main											*/
/*																			*/
/****************************************************************************/
void *pots_loop(void *threadarg)
{
	struct xml_data *DataPtr;
	char Filename[FILE_NAME_SIZE];	
	char buf[250];

	struct stat stbuf;
	time_t last = stbuf.st_mtimespec.tv_sec;
	long	nlast = stbuf.st_mtimespec.tv_nsec;
	int	i=0, done, confirm;
	FILE *fd;


	DataPtr = (struct xml_data *) threadarg;

	if ( DataPtr->OSC_flg == TRUE) {
		// get the pots data from the modules configuration .. not from the pots file.		
		
		// then we need to service incoming UDP updates.
		monitorUDPio(DataPtr);
		exit(0);  // bring down the process... should exit more gracefully than this.
	
	}
	else if (( DataPtr->build.Pot_channel_no )  && (DataPtr->config_name != NULL) ) {  // only if there are pots
		sprintf(Filename,"%s.pertPts", DataPtr->config_name);
		
		while( TRUE) { // perhaps we need to look at the run status for this test?
			confirm = stat( Filename, &stbuf);
			if (confirm == -1) { usleep(50000); } // pause for file to re appear
			else {
				if ((last == stbuf.st_mtimespec.tv_sec) && (nlast == stbuf.st_mtimespec.tv_nsec)) {	usleep(10000); } // wait for change
				else if ( DataPtr->run_mode == RUN) { // file changed and in run mode
					potentiometers( DataPtr );  // then read potentiometrs file
					last = stbuf.st_mtimespec.tv_sec;
					nlast = stbuf.st_mtimespec.tv_nsec;
				}
			}
		}
	}
	else { // shut down the thread
	
	}
//	fprintf(stderr, "exiting pots thread\n"); fflush(stderr);	
   	pthread_exit((void *) 0);	

}


/****************************************************************************/
/*																			*/
/*  controller thread																*/
/*																			*/
/****************************************************************************/
void *control_loop(void *threadarg)
{
	struct xml_data *DataPtr;
	char Filename[FILE_NAME_SIZE];	
	char buf[250];

	struct stat stbuf;
	time_t last = 0;
	long	nlast = 0 ;
	int	i=0, done, confirm, check = -1;
	FILE *fd;

	DataPtr = (struct xml_data *) threadarg;
	if (DataPtr->config_name != NULL) { // have a name... 	try to open a control file
		sprintf(Filename,"%s.pertecsctrl", DataPtr->config_name);

	
		while( (confirm = stat( Filename, &stbuf) != -1) && (check != 0)) {
			if (confirm == -1) { usleep(5000); check--;}
			else {
				if ((last == stbuf.st_mtimespec.tv_sec) && (nlast == stbuf.st_mtimespec.tv_nsec)) {	usleep(5000);}
				else {
					last = stbuf.st_mtimespec.tv_sec;
					nlast = stbuf.st_mtimespec.tv_nsec;
	
					fd = fopen (&Filename[0],"r+");
					if (fd) {
						fread(&buf, sizeof (char), 245,fd );
						switch ( buf[209] ) {
							case '0':
								DataPtr->run_mode = PAUSE;
								break;
							case '1':
								DataPtr->run_mode = RESET;
								break;
							case '2':
								DataPtr->run_mode = RUN;
								break;
							default:
								break;
						}
						//fprintf(stderr,"jhgjhgj %c\n",buf[209]); fflush(stderr);
						//fclose(fd);
					}
					else { 
						fprintf(stderr, "problem opening control file %s \n", Filename); fflush(stderr); 
						fprintf(stderr, "exiting control thread\n"); fflush(stderr);	
   						pthread_exit((void *) 0);
					}
				}
			}
		} // end while
	}
//	fprintf(stderr, "exiting control thread\n"); fflush(stderr);	
   	pthread_exit((void *) 0);
}

/****************************************************************************/
/*																			*/
/* zero out delayline... load function file data, if file extension         */
/*																			*/
/****************************************************************************/
void Init_deline(struct xml_data *DataPtr, int i) {

	int j, count = 0, inptcount, warningflg = 0;
	char Filename[FILE_NAME_SIZE];	
	FILE *fd;
	float value;


	if (( DataPtr->Modules[i]->type == DELINE ) && (DataPtr->Modules[i]->input_buffPtr != NULL)) { // make sure we are really dealing  with the right device
		
		if ((!strncmp( DataPtr->Modules[i]->ParamPtr[DELAYMODE]->label, "delay", 5)) || (!strncmp( DataPtr->Modules[i]->ParamPtr[DELAYMODE]->label, "func", 4)) || (!strncmp( DataPtr->Modules[i]->ParamPtr[DELAYMODE]->label, "FIR", 3)))  { // load a inital data from file
			for (j = 0; j < DataPtr->Modules[i]->buff_sz; j++)  DataPtr->Modules[i]->input_buffPtr[j] = 0.0; // zero out delay line"
			if (strlen(DataPtr->Modules[i]->ParamPtr[DELFUNCFILE]->label)) {
				sprintf(Filename,"%s.%s", DataPtr->config_name, DataPtr->Modules[i]->ParamPtr[DELFUNCFILE]->label ); 
				fd = fopen (&Filename[0],"r+");
				if (!fd) {
					if ((!strncmp( DataPtr->Modules[i]->ParamPtr[DELAYMODE]->label, "func", 4)) || (!strncmp( DataPtr->Modules[i]->ParamPtr[DELAYMODE]->label, "FIR", 3))) {
						fprintf(stderr,"Problem opening function or FIR data file: %s in module %s\n", Filename, DataPtr->Modules[i]->name); fflush(stderr); 
					}
				} 
				else {
					inptcount = 0; 
					j = 0;
					while ((j < DataPtr->Modules[i]->buff_sz) && (inptcount != EOF)) {
						inptcount = fscanf(fd, "%d %f", &count, &value );
						if (inptcount != EOF) {
							 if (count != j) warningflg++;
							 DataPtr->Modules[i]->input_buffPtr[j] = (double) value ;
							j++;
						}
					}
					fclose(fd);	
					if (warningflg) { fprintf(stderr, "Warning! In file %s, index irregularity (%d)\n", Filename, warningflg); fflush (stderr); }
//					fprintf(stderr, "buffsize = %d\n", DataPtr->Modules[i]->buff_sz); fflush(stderr);
					
//					if ((DataPtr->Modules[i]->buff_sz < j-1)) { fprintf(stderr, "Error! In file %s, Buffer size (%d) too small\n", Filename, DataPtr->Modules[i]->buff_sz); fflush (stderr); exit (1); }
					if (inptcount == EOF ) { fprintf(stderr, "Warning: file %s has only %d points where as buffer-size given as %d\n", Filename, j, DataPtr->Modules[i]->buff_sz); fflush (stderr);  }
					DataPtr->Modules[i]->buff_sz = j; // set the size correctly
				
				}
			}
			else if (strncmp( DataPtr->Modules[i]->ParamPtr[DELAYMODE]->label, "delay", 5)) fprintf(stderr,"Missing file extension in module %s \n", DataPtr->Modules[i]->name); fflush(stderr);
		}
	}
}



/****************************************************************************/
/*																			*/
/*  main thread																*/
/*																			*/
/****************************************************************************/
void *main_loop(void *threadarg)
{
	struct xml_data *DataPtr;
    struct timeval tvtime;
	unsigned long long	last_lapsed_useconds, lapsed_useconds, start_useconds, endloop_useconds, cycles , totcycles;
	long totcycleincr;
	double	cyclerate;
	pthread_t 	pots_thread; 

	int	done, i, j, rc, conterinside=0;

	DataPtr = (struct xml_data *) threadarg;

	DataPtr->thread_status = RUN ;	// thread set to run state


/*************** SETUP IO MODULES  **********************/
// only read if there is io in the build
	if (( DataPtr->build.Output_channel_no ) || ( DataPtr->build.Input_channel_no ))  // only if there are either input or output
		 ionames( DataPtr ); // do the io names and presets together now


/*************** SETUP MODULES: initialisation/connections/sampling  **********************/

	Create_Module_Space( DataPtr ); 				// set up memory allocation for modules ... assign default module and node names


// always create a modules configuration file including default system and scope control 
	if ( presets( DataPtr ) ) exit(1);  // created a presets file and then exit


// buffer sizes are now known so can set up buffers
	Create_Module_Buffers( DataPtr ); 	// only allocates  buffers space


// and generate any parameters required for Ch Bu Be type filters  //
	if( DataPtr->build.Filter_build_size ) mkfilter(*DataPtr);

// and set up for the FFT  //
	if( DataPtr->build.FFT_build_size ) setupFFTspace(*DataPtr);


// output pots file if none exists
	if (!DataPtr->OSC_flg ) 
		potentiometers( DataPtr );  // created a potentiometrs file

		rc = pthread_create(&pots_thread, NULL, pots_loop, (void *) &pertecs_data);
		if (rc) {
			fprintf(stderr, ">>ERROR; failed to establish a potentiometer monitor thread: %d\n", rc);
			exit(-1);
		}


// any scope modules?
	if( DataPtr->build.Trace_build_size ) mkSpodobjects( DataPtr ); // create all the spod buffers for the trace graphics, currently can only enter this once... per build
				

	DataPtr->run_mode = RESET;	// set up for initialisation

	DataPtr->thread_status = STOP;	// thread status set to STOP 

/*************** INITIALISATION LOOP **********************/
	do {

	/*  The connections  */	
		DataPtr->CircuitTree = NULL;					//  root node for circuit tree.
		DataPtr->SamplingTree = NULL;					//  root node for output telemetry sampling.
		create ( &DataPtr->CircuitTree );					// start Circuit Tree by placing first empty node
		create ( &DataPtr->SamplingTree );					// start Sampling Tree by placing first empty node


		if ( load_connections( DataPtr )) exit(1);



// outputting processed xml files
		if(DataPtr->debugflg) {
			if ( DataPtr->suppressnamesflag == TRUE) {
				InitInputOutputNames ( DataPtr );				
				InitModuleNames ( DataPtr );
				write_pots_file( DataPtr, DataPtr->debugflg );
				cpy_Build_debug( DataPtr );
			}
			IO_Names_debug ( DataPtr );			// create the debug config file;
			Mod_Names_debug ( DataPtr );		//
			Circuit_Names_debug ( DataPtr ); 	// create circuit debug
			exit(0);
		}
			
// ******** Init output circuit nodes   ********** //
// have resolved to use the parameter slots to give module initialisation
		for (i=1; i < DataPtr->build.Output_channel_no ; i++) DataPtr->Output_Modules[i]->put.Out->value = 0.0; 				// zero out the output channels for first use


		for (i=0; i < DataPtr->build.build_size ; i++) {
			if ( DataPtr->Modules[i]->type == DIFFINT ) {
				DataPtr->Modules[i]->Output[INTEGRATED]->value = DataPtr->Modules[i]->ParamPtr[INITIALISATION]->value;  // initalise the integrators
			}
			else if ( DataPtr->Modules[i]->type == COMPAR ) { // initialisation of comparators
				DataPtr->Modules[i]->Output[DELAYED]->value = DataPtr->Modules[i]->ParamPtr[0]->value;
				DataPtr->Modules[i]->Output[POS]->value = DataPtr->Modules[i]->ParamPtr[1]->value;
				DataPtr->Modules[i]->Output[ZERO]->value = DataPtr->Modules[i]->ParamPtr[2]->value;
				DataPtr->Modules[i]->Output[NONPOS]->value = DataPtr->Modules[i]->ParamPtr[3]->value;
				DataPtr->Modules[i]->Output[NEG]->value = DataPtr->Modules[i]->ParamPtr[4]->value;
				DataPtr->Modules[i]->Output[NONNEG]->value = DataPtr->Modules[i]->ParamPtr[5]->value;
			}
			else if ( DataPtr->Modules[i]->type == DELINE ) Init_deline(DataPtr, i); // zero out delayline or load function data
		}


//debug fprintf(stderr, "multiplier rate %f\n", DataPtr->build.multiplier_Rate); fflush(stderr);

	/*************** MAIN LOOP START UP**********************/
		if (DataPtr->OSC_flg == TRUE) DataPtr->run_mode = pertecs_data.stepincr; // do a number of steps cycles to update 
		else DataPtr->run_mode = RUN;


		cycles = 0;
		totcycles = 0;
		realtime_rate = DataPtr->build.System_Sample_Rate;

		if (realtime_rate > 0) {
			cyclerate = ((double) realtime_rate) / 1000000.0; // cycles/usec
//debug fprintf(stderr, "cyclerate %f\n", cyclerate); fflush(stderr);

			usleep_time = (int) (sleep_fraction/cyclerate); // is the equivalent sleep time if no processing was required
//debug fprintf(stderr, "sleep cycles %d\n", usleep_time); fflush(stderr);

			last_lapsed_useconds = lapsed_useconds = 0;
    		gettimeofday(&tvtime, NULL); // last thing we do before entering loop, is get start time
    		start_useconds = tvtime.tv_sec;
    		start_useconds = start_useconds * 1000000 + tvtime.tv_usec; // make sure we are utilising 64 bit
		}
  
  
  	/*************** MAIN LOOP **********************/
		do { 
	
			// collect the input frame
			done = InputDataFrame(DataPtr);


			if (done == EOF) {
				DataPtr->run_mode = STOP;
				if (pertecs_data.terminate_on_EOF == TRUE) exit (0);
			}
					
			// Prime module inputs
			for (i=0; i < DataPtr->build.build_size; i++) 
				for (j=0; j <  MAX_MODULE_INPUTS_SIZE; j++) DataPtr->Modules[i]->Input[j]->value = 0.0; 			// zero out all inputs
	
			Sampling(DataPtr->CircuitTree);															// calculate afresh all inputs for modules according to processing Circuit
			
											// copy lugs to mirrorframe then zero out the lugs here... this allows forward propagation forward from previous cycle
			if ( DataPtr->build.Connection_build_size ) {
				for (i=0; i < DataPtr->build.Connection_build_size ; i++) {
					DataPtr->mirror_Lug_Modules[i] = DataPtr->lug_Modules[i]->put.Out->value ;
					DataPtr->lug_Modules[i]->put.In->value = 0.0; 
				}
			}
			
			// process the modules
			if (( DataPtr->run_mode == RUN) ||  (DataPtr->run_mode > PAUSE)) // allows for stepping 
				DataPtr->run_mode = Process_Module( DataPtr->Modules, &DataPtr->build, iframe_counter, DataPtr->run_mode);					// include time to do a time check, on integration correction

			if 	(DataPtr->run_mode > 0) DataPtr->run_mode--;
			
			// sample circuit nodes and output telemetry frame 
			Sampling(DataPtr->SamplingTree);													 	// recalculate outputs according to monitor Sample Circuit, including the lugs
	
			// output data
			OutputDataFrame(DataPtr);
				

				cycles += 1; // counting the total cycles executed so far

//						fprintf(stderr, " cycles %d %d\n", cycles, totcycles); fflush(stderr);

			if (realtime_rate != 0) {
				if (DataPtr->build.multiplier_Rate == -1) {
					do {
						usleep(usleep_time); 
						gettimeofday(&tvtime, NULL);					 
						lapsed_useconds  = tvtime.tv_sec * 1000000 + tvtime.tv_usec - start_useconds;
						totcycles = (unsigned long long) ((double) (lapsed_useconds ) * cyclerate );
					} while ( cycles  > (unsigned long long)  totcycles );
				}
				else {
					do {
						gettimeofday(&tvtime, NULL);					 
						lapsed_useconds  = tvtime.tv_sec * 1000000 + tvtime.tv_usec - start_useconds;
						totcycleincr = (long) (((double) (lapsed_useconds - last_lapsed_useconds)) * cyclerate * (double) DataPtr->build.multiplier_Rate);
					} while ( cycles  > (unsigned long long) totcycleincr + totcycles );
					last_lapsed_useconds = lapsed_useconds;
					totcycles += (unsigned long long) totcycleincr;	
				}
			}
			

		} while ( (DataPtr->run_mode == PAUSE) || (DataPtr->run_mode == RUN) ||  (DataPtr->run_mode > 0) ); // i.e. reset or stop or step
	/*************** END MAIN LOOP **********************/
		freeTree ( DataPtr->CircuitTree );					// free Circuit Tree by placing first empty node
		freeTree ( DataPtr->SamplingTree );					// free Sampling Tree by placing first empty node

	} while  (DataPtr->run_mode != STOP);  // sustain idle if DataPtr->run_mode is either initialise, run  or pause 
/*************** END INITIALISATION LOOP **********************/

	takedownFFTspace();
	fprintf(stderr, "exiting main thread\n"); fflush(stderr);	
	DataPtr->thread_status = QUIT ;	
   	pthread_exit((void *) 0);	
}








extern void spod(struct xml_data *DataPtr);



/****************************************************************************/
/*																			*/ 
/*  close out															*/
/*																			*/
/****************************************************************************/
closeout(struct xml_data *xml_dataPtr ) {
	int i;
	
	for (i = 1; i < Num_surface; i++)	{ if (surfacePtr[i] != NULL) free(surfacePtr[i]) ;  surfacePtr[i] = NULL; }
	for (i = 1; i < Num_curve; i++)		{ if (curvePtr[i] != NULL) free(curvePtr[i]) ; curvePtr[i] = NULL; }
	for (i = 1; i < Num_curve; i++)		{ if (ballPtr[i] != NULL) free(ballPtr[i]) ; ballPtr[i] = NULL; }
	for (i = 1; i < NUM_TXTLINES; i++)	{ if (textPtr[i] != NULL) free(textPtr[i]) ; textPtr[i] = NULL; }
	for (i = 1; i < JUMP_SIZE; i++)		{ if (jumpPtr[i] != NULL) free(jumpPtr[i]) ; jumpPtr[i] = NULL; }


	
	// need to look at doing a thorough free up of all resources
	// input interpolation filter arrays
	for (i=0; i <= xml_dataPtr->build.Input_channel_no; i++) if (xml_dataPtr->interpfilterPtr[i]) free( xml_dataPtr->interpfilterPtr[i]);
	if (xml_dataPtr->weightsPtr) free (xml_dataPtr->weightsPtr);

	for (i=0; i < pertecs_data.build.build_size; i++) {
		if (pertecs_data.Modules[i]->input_buffPtr) { free(pertecs_data.Modules[i]->input_buffPtr); pertecs_data.Modules[i]->input_buffPtr = NULL; }
		free(pertecs_data.Modules[i]); pertecs_data.Modules[i] = NULL;
	}
	
	free (xml_dataPtr->frameoutPtr); 
	xml_dataPtr->frameoutPtr = NULL;
	
	fclose(xml_dataPtr->input_fid); 
	xml_dataPtr->input_fid = NULL;

	if (xml_dataPtr->rawoutflg == FALSE ) 
		fprintf(xml_dataPtr->output_fid,"\n"); // a final new line if text telemetry output
	fclose(xml_dataPtr->output_fid);



}

/****************************************************************************/
/*																			*/
/*  Create Runtime Control HUD													*/
/*																			*/
/****************************************************************************/
CreateRunTimeControls (struct xml_data *dataPtr) {
	char Filename[FILE_NAME_SIZE];	
	FILE *fd;

	sprintf(Filename,"%s.pertecsctrl", dataPtr->config_name); 
	fd = fopen( Filename, "w+");
	if (fd) {
		print_pheader(fd);
		fprintf(fd, "\t<dict>\n\t\t<key>Mode</key>\n\t\t<integer>1</integer>\n\t</dict>\n");
		print_pclose(fd);

		sprintf(Filename,"open %s.pertecsctrl", dataPtr->config_name); 
		system(Filename);
		
		return 0;
	}
	fprintf(stderr, "failed to create %s\n", Filename); fflush(stderr);
	return 1;
 
}


/****************************************************************************/
/*																			*/
/*  print too few args and exit													*/
/*																			*/
/****************************************************************************/
void toofewargs () { 
	fprintf(stderr, "too few arguments\n"); 
	exit (-1);  
}


/****************************************************************************/
/*																			*/
/*  main																	*/
/*																			*/
/****************************************************************************/
int main ( int argc, char** argv )
{

	pthread_t 	main_thread, control_thread; 
   	pthread_attr_t attr;
	int	rc;
	int pause = 0;
	void *status;


	char Command[MAXCOMMANDSZ];


	pertecs_data.runtime_controls_flg = FALSE;
	pertecs_data.openflg = FALSE;
	pertecs_data.opencontrolsflg = FALSE;
	pertecs_data.OSC_flg = FALSE;

	
	pertecs_data.input_fid = NULL;
	pertecs_data.output_fid = NULL;
	pertecs_data.rawinflg = FALSE;
	pertecs_data.rawoutflg = FALSE;						// rawoutput flag
	pertecs_data.in_sampleflg = NORMAL;
	pertecs_data.out_sampleflg = NORMAL;

	pertecs_data.terminate_on_EOF = FALSE;
	
	pertecs_data.buildflg = FALSE;
	pertecs_data.debugflg = FALSE;	
	pertecs_data.suppressnamesflag = FALSE;	
	pertecs_data.codeflag = FALSE;	

	
								// signalling of control panels to be opened 

	pertecs_data.frameoutPtr = NULL;
	
	struct stack mystack;								// used in making connections
	pertecs_data.mystackPtr = &mystack;


	mystack.pointer = 0;
	
	
	pertecs_data.out_resample = 1;
	pertecs_data.in_resample = 1;
	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	Init_build( &pertecs_data.build);  					// initialise  build details before any command line entry...
	pertecs_data.System_Rate = 0; //Hz default
	
	pertecs_data.stepincr=100; // some default number

/* command line preferences */
	int i;
	for(i=1; i < argc; i++) {
		if ((argc <= 1) || ( !strcmp(argv[i], "-help")) ||  ( !strcmp(argv[i], "-h")) || ( !strcmp(argv[i], "-?"))) {
			if (argc <= 1) { fprintf(stderr, "No configuration file specified,  see -c switch \n\n"); }

			else {
				fprintf(stderr, "PERTECS: A Programmable Environment for Real-Time Emulation of Continuous Systems\n");
				fprintf(stderr, "Version: 10.1\n");
				fprintf(stderr, "Copyrighted software: 2008, 2009, 2010\n");
				fprintf(stderr, "Author: Dr M. R. Titchener.\n");
				fprintf(stderr, "This software free for educational and non-commercial use.\n\n");
				
				fprintf(stderr, "This program is distributed in the hope that it will be useful,\n");
				fprintf(stderr, "but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
				fprintf(stderr, "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");
			}
			fprintf(stderr, "[-h][-?][help][-help][]\t prints this help menu\n");
			fprintf(stderr, "[-d][downsample] n\t\tdownsample with averaging of 'n' consecutive points\n");
			fprintf(stderr, "[-D][Downsample] n\t\tstrict downsampling by a factor 'n', i.e. outputs every nth point.\n");
			fprintf(stderr, "[-Uo] n \t\t\tUpsampling on output, by a factor 'n'.\n");
			fprintf(stderr, "[-Ui] n \t\t\tUpsampling on input, by a factor 'n' with 'zero stuffing'\n");
			fprintf(stderr, "[-ui] n \t\t\tUpsampling on input by factor 'n' with gausian weighted sinc interpolation\n\n");
			fprintf(stderr, "[-c][-config] config_name \tspecifying the configuration files\n");

			fprintf(stderr, "[-b][-build] \t\t\tused with -c option, creates an empty build configuration file\n\n");
			fprintf(stderr, "[-o][-open] \t\t\topen configuration files and exit pertecs\n\n");
			fprintf(stderr, "[-ctrls] \t\t\topen control panels on initialisation\n\n");

			fprintf(stderr, "FOR IN-LINE-COMMAND BUILDS:\n");
			
			fprintf(stderr, "[-inputs][-ip] n  \t\tcommand line build  'inputs channels' n\n");
			fprintf(stderr, "[-outputs][-op] n \t\tcommand line build  'output channels' n\n");
			fprintf(stderr, "[-connections][-cn] n  \t\tcommand line build  'connections lugs' n\n");
			fprintf(stderr, "[-funcdelay][-fd] n  \t\tcommand line build  'function/delay lines' n\n");
			fprintf(stderr, "[-noise][-ns] n  \t\tcommand line build  'random source' n\n");
			fprintf(stderr, "[-trace][-tr] n  \t\tcommand line build  'scope channels' n\n");
			fprintf(stderr, "[-pots][-pt] n  \t\tcommand line build  'potentiometers' n\n");
			fprintf(stderr, "[-entropy][-en] n  \t\tcommand line build  'entropy modules' n\n");
			fprintf(stderr, "[-calculus][-ca] n  \t\tcommand line build  'calculus modules' n\n");
			fprintf(stderr, "[-externalports][-ep] n\t\tcommand line build  'serial ports' n\n");
			fprintf(stderr, "[-polar][-po] n  \t\tcommand line build  'pola math modulesr' n\n");
			fprintf(stderr, "[-maths][-ma] n  \t\tcommand line build  'math modules' n\n");
			fprintf(stderr, "[-comparators][-co] n  \t\tcommand line build  'comparators' n\n\n");
			fprintf(stderr, "[-scopebgnd] f  \t\tcommand line build  sets the scope neutral background black through to white, f in the range [0.0, 1.0] \n");
			fprintf(stderr, "[-scopeposn] x-pos y-pos  \twith integer values; x-pos,y-pos for the top-left corner of the scope window in pixels\n");
			fprintf(stderr, "[-scopesize] xwidth ywidth  \twith integers; x-width, y-width  giving size of the scope window in pixels\n\n");

			fprintf(stderr, "[-rtc] \t\t\t\tcreates the run time controls file: pause/restart/resume\n");
			fprintf(stderr, "[-debug] \t\t\tgenerate  configuration files fully populated with default module numbering/labels/names \n");
			fprintf(stderr, "[-modulenames][-mn] \t\t\tgenerate configuration files with no signal names and module numbers. Helpful in a rebuild.\n");
			fprintf(stderr, "[-code] \t\t\toutput the connections in a .c file for compilation with embeded PERTECS code\n");
			fprintf(stderr, "[-rii] \t\t\t\traw input telemetry, integer format\n");
			fprintf(stderr, "[-rif][-rfi] \t\t\traw input telemetry, float format\n");
			fprintf(stderr, "[-roi][-rio] \t\t\traw output telemetry, integer format\n");
			fprintf(stderr, "[-rof][-rfo] \t\t\traw output telemetry, float format\n");
			fprintf(stderr, "[-r][-raw] \t\t\traw input/output telemetry, float formats\n");
			fprintf(stderr, "[-ineof][-terminateoneof]\tterminate PERTECS on receipt of input EOF (end of file/^D)\n");
			fprintf(stderr, "[-rate] n\t\t\tsample rate 'n'... command line overrides build rate-setting\n");
			fprintf(stderr, "[-p] n\t\t\t\tpause n microseconds at initialisation.\n");
			fprintf(stderr, "[-pause]\t\t\tgeneric pause before running\n");

			fprintf(stderr, "\nFurther help from within the scope window:\n");
			fprintf(stderr, "\tWith the pertecs scope window active, further controls may be initiated from simple key commands\n");
			fprintf(stderr, "\tTyping '?' will bring up an overlay in the scope window summarizing key commands.\n");
			fprintf(stderr, "\tNumerical display of all of the system modules and their nodes is visiible by typing 'I' (shift 'i', for information).\n");
			fprintf(stderr, "\tHolding the right-mouse button down allows the overlay content to be moved within the overlay subwindow.\n");
			fprintf(stderr, "\tSimilarly the arrow keys may be used to navigate the overlay content.\n");
			fprintf(stderr, "\tPresently the content is visible only by toggling the scope (key 'p' to toggle) to a dark background \n");

			fflush(stderr);
			exit (0);
			
		}
		if ( !strcmp(argv[i], "-d") || !strcmp(argv[i], "-downsample") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.out_resample); // avergeing
      			i++;
      			if (pertecs_data.out_resample <= 1) { fprintf(stderr, "Incorrect down-sample factor\n"); exit (-1); }
      			pertecs_data.out_sampleflg = NORMAL;
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-D") || !strcmp(argv[i], "-Downsample") ) { // will do no averaging
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.out_resample);
				i++;
				if (pertecs_data.out_resample <= 1) { fprintf(stderr, "Incorrect down-sample factor\n"); exit (-1); }
				pertecs_data.out_sampleflg = DOWNSAMPLE;
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-S") || !strcmp(argv[i], "-Strobe") ) { // stobes output on channel 1 being == (1)
				pertecs_data.out_sampleflg = STROBE;
		}
		else if ( !strcmp(argv[i], "-Sosc") || !strcmp(argv[i], "-StrobeOSC") ) { // stobes output on channel 1 being == (1)
			pertecs_data.out_sampleflg = STROBEOSC;
			sscanf(argv[i + 1], "%d", &portdoscnumber);
			i++;
			if (portdoscnumber <= 6000) { fprintf(stderr, "port number must be greater than 6000\n"); exit (-1); }
		}
		else if ( !strcmp(argv[i], "-Uo") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.out_resample);
				i++;
				if (pertecs_data.out_resample <= 1) { fprintf(stderr, "Incorrect up-sample factor\n"); exit (-1); }
				pertecs_data.in_sampleflg = UP_OUTSAMPLE;
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-Ui") ) { // zero stuffing 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.in_resample);
				i++;
				if (pertecs_data.in_resample <= 1) { fprintf(stderr, "Incorrect up-sample factor\n"); exit (-1); }
				pertecs_data.in_sampleflg = UP_INSAMPLE;
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-ui") ) { // zero stuffing 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.in_resample);
				i++;
				if (pertecs_data.in_resample < 1) { fprintf(stderr, "Incorrect up-sample factor\n"); exit (-1); } // must not be negative
				if (pertecs_data.in_resample > 1) pertecs_data.in_sampleflg = UP_INSAMPLE_INTERPOLATE;				// upsample of 1 means no upsampling but straight in.
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-config") || !strcmp(argv[i], "-c") ) { // input config file name
			if (i+1 < argc) {
				pertecs_data.config_name = argv[i + 1];
				i++;
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-rtc") ) { // run time controls
			pertecs_data.runtime_controls_flg = TRUE;
		}
		else if ( !strcmp(argv[i], "-OSC") ) { // use OSC controls instead default port has been 
			pertecs_data.OSC_flg = TRUE;
			pertecs_data.runtime_controls_flg = FALSE;
			pertecs_data.opencontrolsflg = FALSE;
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &portsndnumber);
				i++;
				if (portsndnumber <= 6000) { fprintf(stderr, "port number must be greater than 6000\n"); exit (-1); }
				else portrecnumber = portsndnumber + 1;
			} else { //default port numbers 9000 (out) and 9001 (received)
				portrecnumber = 9001;
				portsndnumber = 9000;
			}
		}
		else if ( !strcmp(argv[i], "-debug")) { // output ~files
			pertecs_data.debugflg = TRUE;
		}
		else if ( !strcmp(argv[i], "-modulenames") || !strcmp(argv[i], "-mn") ) { // allows one to ignore module numbers and signal names and return to the default names in the output debug files
			pertecs_data.suppressnamesflag = pertecs_data.debugflg = TRUE;
		}
		else if ( !strcmp(argv[i], "-code")) { // output .c
			pertecs_data.codeflag = CCODE;
		}
		else if ( !strcmp(argv[i], "-rii")) { // raw in
			pertecs_data.rawinflg = INTEGER;
		}
		else if ( !strcmp(argv[i], "-rfi") || !strcmp(argv[i], "-rif")) { // raw in
			pertecs_data.rawinflg = FLOAT;
		}
		else if ( !strcmp(argv[i], "-rio") || !strcmp(argv[i], "-roi") ) { // raw integer out
			pertecs_data.rawoutflg = INTEGER;
		}
		else if ( !strcmp(argv[i], "-rfo")  || !strcmp(argv[i], "-rof") ) { // raw float out
			pertecs_data.rawoutflg = FLOAT;
		}
		else if ( !strcmp(argv[i], "-raw") || !strcmp(argv[i], "-r") ) { // raw float in and out
			pertecs_data.rawinflg = TRUE;
			pertecs_data.rawoutflg = TRUE;
		}
		else if ( !strcmp(argv[i], "-ineof") || !strcmp(argv[i], "-terminateoneof") ) { // terminate on input eof
			pertecs_data.terminate_on_EOF = TRUE;
		}
		else if ( !strcmp(argv[i], "-rate")) { // override the build rate
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.System_Rate);
				i++;
				if (pertecs_data.System_Rate < 0) { fprintf(stderr, "Incorrect system-sample factor\n"); exit (-1); } // allow zero to indicate that the system should run at processor limits.
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-p") ) { 
				sscanf(argv[i + 1], "%d", &pause);
				i++;
		}
		else if ( !strcmp(argv[i], "-pause") ) { // wait 1 second before starting
			pause = 1000000;
		}
		else if ( !strcmp(argv[i], "-slfrac")) { // sleep fraction
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%lf", &sleep_fraction);
				i++;
				if ((sleep_fraction < 0.01) || (sleep_fraction > .5) ) { fprintf(stderr, "Sleep Fraction out of range [0.01, 0.5]\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-build") || !strcmp(argv[i], "-b") ) { // allow for auto build
			pertecs_data.buildflg = TRUE;
		}
		else if ( !strcmp(argv[i], "-o") || !strcmp(argv[i], "-open") ) { // open configuration files and exit pertecs
			pertecs_data.openflg = TRUE;
		}
		else if (!strcmp(argv[i], "-ctrls") ) { // open controls
			pertecs_data.opencontrolsflg = TRUE;
		}
		else if ( !strcmp(argv[i], "-inputs") || !strcmp(argv[i], "-ip") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Input_channel_no);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Input_channel_no < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-outputs") || !strcmp(argv[i], "-op") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Output_channel_no);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Output_channel_no < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs();
		}
		else if ( !strcmp(argv[i], "-connections") || !strcmp(argv[i], "-cn") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Connection_build_size);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Connection_build_size < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else { fprintf(stderr, "too few arguments\n"); exit (-1);  }
		}
		else if ( !strcmp(argv[i], "-funcdelay") || !strcmp(argv[i], "-fd") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Delay_build_size);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Delay_build_size < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else { fprintf(stderr, "too few arguments\n"); exit (-1);  }
		}
		else if ( !strcmp(argv[i], "-noise") || !strcmp(argv[i], "-ns") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Noise_channel_size);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Noise_channel_size < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-trace") || !strcmp(argv[i], "-tr") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Trace_build_size);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Trace_build_size < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-pots") || !strcmp(argv[i], "-pt") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Pot_channel_no);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Pot_channel_no < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-entropy") || !strcmp(argv[i], "en") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Ent_build_size);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Ent_build_size < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-audioports") || !strcmp(argv[i], "-ap") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Audio_build_size);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Audio_build_size < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-fft") || !strcmp(argv[i], "-ft") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.FFT_build_size);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.FFT_build_size < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-calculus") || !strcmp(argv[i], "-ca") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.ID_build_size);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.ID_build_size < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-externalports") || !strcmp(argv[i], "-ep") ) { 
			if (i+1 < argc) {
				pertecs_data.build.Extern_build_size = 1 ;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Extern_build_size < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-polar") || !strcmp(argv[i], "-po") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Plr_build_size);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Plr_build_size < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-maths") || !strcmp(argv[i], "-ma") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Math_build_size);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Math_build_size < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-comparators") || !strcmp(argv[i], "-co") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Comp_build_size);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Comp_build_size < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-filters") || !strcmp(argv[i], "-fr") ) { 
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Filter_build_size);
				i++;
				pertecs_data.buildflg = TRUE;
				if (pertecs_data.build.Filter_build_size < 0) { fprintf(stderr, "non-negative parameter required\n"); exit (-1); }
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-scopebgnd")) { // scope background colour (assumed neutral)
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%f", &pertecs_data.build.background_r);
				i++;
				pertecs_data.buildflg = TRUE;
				if ((pertecs_data.build.background_r < 0.0) || (pertecs_data.build.background_r > 1.0) ) { fprintf(stderr, "Scope background colour out of range [0.0, 1.0]\n"); exit (-1); }
				pertecs_data.build.background_g = pertecs_data.build.background_b = pertecs_data.build.background_r; // neutral
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-scopeposn")) { // scope background colour (assumed neutral)
			if (i+2 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.Origin_X);
				sscanf(argv[i + 2], "%d", &pertecs_data.build.Origin_Y);
				i+=2;
				pertecs_data.buildflg = TRUE;
      		} else toofewargs ();
		}
		else if ( !strcmp(argv[i], "-scopesize")) { // scope window position
			if (i+2 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.build.WIdth);
				sscanf(argv[i + 2], "%d", &pertecs_data.build.Height);
				i+=2;
				pertecs_data.buildflg = TRUE;
      		} else toofewargs ();
		} // grid scale, grid offset, origin and axis labels, grid intensity, colour, light angles
		else if ( !strcmp(argv[i], "-gxyz")) { // graticule x yz scale offset orgin axis labels
			if (i+6 < argc) {
				sscanf(argv[i + 1], "%f", &pertecs_data.build.xscale);
				sscanf(argv[i + 2], "%f", &pertecs_data.build.yscale);
				sscanf(argv[i + 3], "%f", &pertecs_data.build.zscale);
				sscanf(argv[i + 4], "%f", &pertecs_data.build.xoffset);
				sscanf(argv[i + 5], "%f", &pertecs_data.build.yoffset);
				sscanf(argv[i + 6], "%f", &pertecs_data.build.zoffset);
				sscanf(argv[i + 7], "%d", &pertecs_data.build.origin);
				sscanf(argv[i + 8], "%d", &pertecs_data.build.axis);
				i+=8;
				pertecs_data.buildflg = TRUE;
      		} else toofewargs ();
		} // grid scale, grid offset, origin and axis labels, grid intensity, colour, light angles
		else if ( !strcmp(argv[i], "-gica")) { // graticule intesity colour angles 
			if (i+6 < argc) {
				sscanf(argv[i + 1], "%f", &pertecs_data.build.yzintensity);
				sscanf(argv[i + 2], "%f", &pertecs_data.build.xyintensity);
				sscanf(argv[i + 3], "%f", &pertecs_data.build.xzintensity);
				sscanf(argv[i + 4], "%f", &pertecs_data.build.rcolour);
				sscanf(argv[i + 5], "%f", &pertecs_data.build.gcolour);
				sscanf(argv[i + 6], "%f", &pertecs_data.build.bcolour);
				sscanf(argv[i + 7], "%f", &pertecs_data.build.langle1);
				sscanf(argv[i + 8], "%f", &pertecs_data.build.langle2);
				i+=8;
				pertecs_data.buildflg = TRUE;
      		} else toofewargs ();
		} // initial step size
		else if ( !strcmp(argv[i], "-step") || !strcmp(argv[i], "-st") ) {  // step increment requires OSC
			if (i+1 < argc) {
				sscanf(argv[i + 1], "%d", &pertecs_data.stepincr);
				i++;
				if (pertecs_data.stepincr < 0) { fprintf(stderr, "non-negative step increment required\n"); exit (-1); }
      		} else toofewargs ();
		}
		
	}


	if(pertecs_data.config_name == NULL) { // allow for serial xml preamble  ... yet to be completed
		fprintf ( stderr, "no configuration set specified\n" );
		exit(-1);
	}



/*************** INITIALISATION **********************/

	

	if ( build( &pertecs_data ))  											// then build from base file name, exit if empty build
		exit(-1); 
	// lets set up pointers here for now



		if (pertecs_data.openflg == TRUE) {
	
			snprintf(Command, MAXCOMMANDSZ , "open %s.%s.pertMd", pertecs_data.config_name, pertecs_data.build.ext_build);
			system(Command);
	
			snprintf(Command, MAXCOMMANDSZ ,"open %s.%s.pertPr", pertecs_data.config_name, pertecs_data.build.ext_prests); 
			system(Command);		// open the interface
	
			snprintf(Command, MAXCOMMANDSZ ,"open %s.%s.pertNm", pertecs_data.config_name, pertecs_data.build.ext_ionms); 
			system(Command); // io panel
				
			snprintf(Command, MAXCOMMANDSZ ,"open %s.%s.pertPp", pertecs_data.config_name, pertecs_data.build.ext_circ); 
			system(Command);
	
			exit(-1);
		}
		else  if (pertecs_data.opencontrolsflg == TRUE) {
			if ( pertecs_data.build.Pot_channel_no ) {
				snprintf(Command, MAXCOMMANDSZ ,"open %s.pertPts", pertecs_data.config_name); // pots
				system(Command);
			}
		
			snprintf(Command, MAXCOMMANDSZ ,"open %s.pertecsctrl", pertecs_data.config_name); // controls
			system(Command);
		}


		if ( pertecs_data.build.Input_channel_no ) {
			pertecs_data.Output_Modules = &(pertecs_data.IO_Modules[ pertecs_data.build.Input_channel_no + 1]);  // set a pointer to the middle of the array
			pertecs_data.mirror_Output_Modules = &(pertecs_data.mirror_IO_Modules[pertecs_data.build.Input_channel_no + 1]); 
		}
		else {
			pertecs_data.Output_Modules = &(pertecs_data.IO_Modules[0]); 
			pertecs_data.mirror_Output_Modules = &(pertecs_data.mirror_IO_Modules[0]); 
		}
		if ( pertecs_data.build.Output_channel_no ) {
			pertecs_data.lug_Modules = &(pertecs_data.Output_Modules[ pertecs_data.build.Output_channel_no + 1]); 
			pertecs_data.mirror_Lug_Modules = &(pertecs_data.mirror_Output_Modules[pertecs_data.build.Output_channel_no + 1]);
		}
		else {
			pertecs_data.lug_Modules = &(pertecs_data.Output_Modules[0]); 
			pertecs_data.mirror_Lug_Modules = &(pertecs_data.mirror_Output_Modules[0]);
		}
		

	if ( pertecs_data.runtime_controls_flg ) if ( CreateRunTimeControls ( &pertecs_data ) ) exit(-1);
	
	InitInputOutput( &pertecs_data );

//	if (pertecs_data.build.Extern_build_size) {};
//		Init_External( &pertecs_data ); 									// init external i/o ports		

	if (pertecs_data.build.Trace_build_size) 								//  if scope required
		glutInit(&argc, argv);												//  GLUT Init


	if (pertecs_data.codeflag == FALSE) { // continue as per normal
	/*************** start CONTROL PANNEL THREAD **********************/
	
		rc = pthread_create(&control_thread, NULL, control_loop, (void *) &pertecs_data);
		if (rc) {
			fprintf(stderr, ">>ERROR; failed to start a control pannel thread: %d\n", rc);
			exit(-1);
		}
	
		if (pertecs_data.build.Trace_build_size) usleep(pause); //seems to need time to set up the thread....
	
	
	/*************** Start MAIN THREAD **********************/

	
		rc = pthread_create(&main_thread, &attr, main_loop, (void *) &pertecs_data);
		if (rc) {
			fprintf(stderr, ">>ERROR; failed to establish main thread: %d\n", rc);
			exit(-1);
		}
	
	
		while ( pertecs_data.suppressnamesflag == TRUE )  sleep(2);   // don't procede to read pots thread while name changes are taking place 	

	

	/*************** run scope as main loop **********************/
		if (pertecs_data.build.Trace_build_size) 
			spod(&pertecs_data);


	//	else {
	//		usleep_time =  (int) 1000000 / realtime_rate  - SLIPTIME;
	//		if (usleep_time < 0 ) usleep_time = 0;
	//		cycles = 0;			// resetting cycles
	//	}						
	
		rc = pthread_join(main_thread, &status);
		if (rc) {
			 fprintf(stderr, "ERROR; return code from pthread_join() is %d\n", rc);
			 exit(-1);
		}
	//	fprintf(stderr, "Completed join with thread status= %ld\n", (long)status);
	
	}
	else { // generate the code.c
	
	
	
	
	}


// ******** FINISH   ********** //
	pthread_attr_destroy(&attr);	
	closeout(&pertecs_data);	
	
	exit(0);
}



