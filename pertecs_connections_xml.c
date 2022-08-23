
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
#include <stdarg.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>

#include "expat.h"

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif



#include "pertecs.h"


/********************************************************************************************************************/

void enterfrom(struct xml_data *dataPtr, int Mod_No, char *name, char *label) { 

	int bound = dataPtr->build.build_size;
	int iobound = 0 ;
	int which;
	
	
	if (dataPtr->build.Input_channel_no) iobound += dataPtr->build.Input_channel_no + 1;
	if (dataPtr->build.Output_channel_no) iobound += dataPtr->build.Output_channel_no +1;
	if (dataPtr->build.Connection_build_size) iobound += dataPtr->build.Connection_build_size; //lugs
	
	if (Mod_No >= bound + iobound ) {
		fprintf(stderr, "Out of range Module No:  %d   ...exiting \n", Mod_No);
		exit (1);
	}
	
	
	if (Mod_No >= 0 ) {
		if (Mod_No < bound ) {	// a module proper.. as opposed to input/pots/output
			if ((!strcmp(name , dataPtr->build.ID_in_i) ) && (dataPtr->Modules[Mod_No]->type == DIFFINT))						which = STANDARD;
			else if (( !strcmp(name ,dataPtr->build.ID_in_j) ) && (dataPtr->Modules[Mod_No]->type == DIFFINT))					which = SPECIAL;
			else if (( !strcmp(name ,dataPtr->build.ID_in_sethold) ) && (dataPtr->Modules[Mod_No]->type == DIFFINT))			which = SETHOLD;
			else if (( !strcmp(name ,dataPtr->build.Ent_in ) ) && (dataPtr->Modules[Mod_No]->type == QTC))						which = STANDARD;
			else if (( !strcmp(name ,dataPtr->build.Filter_in ) ) && (dataPtr->Modules[Mod_No]->type == FILTER))				which = STANDARD;
			else if (( !strcmp(name ,dataPtr->build.Comp_in ) ) && (dataPtr->Modules[Mod_No]->type == COMPAR))					which = STANDARD;
			else if (( !strcmp(name ,dataPtr->build.Math_in ) ) && (dataPtr->Modules[Mod_No]->type == MATHMOD))					which = STANDARD;
			else if (( !strcmp(name ,dataPtr->build.Plr_xin ) ) && (dataPtr->Modules[Mod_No]->type == POLAR))					which = STANDARD;
			else if (( !strcmp(name ,dataPtr->build.Plr_yin ) ) && (dataPtr->Modules[Mod_No]->type == POLAR))					which = SPECIAL;
			else if (( !strcmp(name ,dataPtr->build.Plr_zin ) ) && (dataPtr->Modules[Mod_No]->type == POLAR))					which = SPECIAL2;
			else if (( !strcmp(name ,dataPtr->build.Delay_in_name ) ) && (dataPtr->Modules[Mod_No]->type == DELINE))			which = STANDARD;
			else if (( !strcmp(name ,dataPtr->build.FFT_inx ) ) && (dataPtr->Modules[Mod_No]->type == FFT))						which = STANDARD;
			else if (( !strcmp(name ,dataPtr->build.FFT_iny ) ) && (dataPtr->Modules[Mod_No]->type == FFT))						which = SPECIAL;


			else if (( !strcmp(name ,dataPtr->build.Audio_leftin ) ) && (dataPtr->Modules[Mod_No]->type == AUDIO))				which = STANDARD;
			else if (( !strcmp(name ,dataPtr->build.Audio_rightin ) ) && (dataPtr->Modules[Mod_No]->type == AUDIO))				which = SPECIAL;
			else if (( !strcmp(name ,dataPtr->build.Audio_out_optn ) ) && (dataPtr->Modules[Mod_No]->type == AUDIO))			which = SPECIAL2;

			else if (( !strcmp(name ,dataPtr->build.Extern_argx_label ) ) && (dataPtr->Modules[Mod_No]->type == EXTERN))		which = STANDARD;
			else if (( !strcmp(name ,dataPtr->build.Extern_argy_label ) ) && (dataPtr->Modules[Mod_No]->type == EXTERN))		which = SPECIAL;
			else if (( !strcmp(name ,dataPtr->build.Extern_pushz_label ) ) && (dataPtr->Modules[Mod_No]->type == EXTERN))		which = SPECIAL2;

			else if (( !strcmp(name ,dataPtr->build.Trace_xin_name ) ) && (dataPtr->Modules[Mod_No]->type == TRACE))			which = STANDARD;
			else if (( !strcmp(name ,dataPtr->build.Trace_yin_name ) ) && (dataPtr->Modules[Mod_No]->type == TRACE))			which = SPECIAL;
			else if (( !strcmp(name ,dataPtr->build.Trace_zin_name ) ) && (dataPtr->Modules[Mod_No]->type == TRACE))			which = SPECIAL2;

			else if (( !strcmp(name ,dataPtr->build.Scope_orienta ) ) && (dataPtr->Modules[Mod_No]->type == SCOPE))				which = STANDARD;
			else if (( !strcmp(name ,dataPtr->build.Scope_orientb ) ) && (dataPtr->Modules[Mod_No]->type == SCOPE))				which = SPECIAL;
			else if (( !strcmp(name ,dataPtr->build.System_pause ) ) && (dataPtr->Modules[Mod_No]->type == SYSTEM))				which = STANDARD;
			else if (( !strcmp(name ,dataPtr->build.System_stop ) ) && (dataPtr->Modules[Mod_No]->type == SYSTEM))				which = SPECIAL;
			else if (( !strcmp(name ,dataPtr->build.System_reset ) ) && (dataPtr->Modules[Mod_No]->type == SYSTEM))				which = SPECIAL2;
			else which = -1;

			if (which >=0) {
				pushto(dataPtr->mystackPtr, dataPtr->Modules[Mod_No]->Input[which]); // pushing onto stack
				// also assign name ... should check if it is valid?
				if (strlen(label)) strncpy(dataPtr->Modules[Mod_No]->Input[which]->label, label, MAX_LABEL_LENG) ; 
			}
			else {
			
				if (( !strcmp(name ,dataPtr->build.Pot_out_namex )) && (dataPtr->Modules[Mod_No]->type == PRESETS))				which = POTINX;
				else if (( !strcmp(name ,dataPtr->build.Pot_out_namey )) && (dataPtr->Modules[Mod_No]->type == PRESETS))		which = POTINY;
				else if (( !strcmp(name ,dataPtr->build.Pot_out_namez )) && (dataPtr->Modules[Mod_No]->type == PRESETS))		which = POTINZ;
				else if (( !strcmp(name ,dataPtr->build.Pot_sel_namex )) && (dataPtr->Modules[Mod_No]->type == PRESETS))		which = SELINX;
				else if (( !strcmp(name ,dataPtr->build.Pot_sel_namey )) && (dataPtr->Modules[Mod_No]->type == PRESETS))		which = SELINY;
				else if (( !strcmp(name ,dataPtr->build.Pot_sel_namez )) && (dataPtr->Modules[Mod_No]->type == PRESETS))		which = SELINZ;

				else if (( !strcmp(name ,dataPtr->build.Delay_out_name )) && (dataPtr->Modules[Mod_No]->type == DELINE))		which = DELAYED;

				else if ((!strcmp(name , dataPtr->build.ID_out_a )) && (dataPtr->Modules[Mod_No]->type == DIFFINT)) 			which = THRD_DERIV;
				else if (( !strcmp(name ,dataPtr->build.ID_out_b )) && (dataPtr->Modules[Mod_No]->type == DIFFINT))				which = SCND_DERIV;
				else if (( !strcmp(name ,dataPtr->build.ID_out_c )) && (dataPtr->Modules[Mod_No]->type == DIFFINT))				which = FST_DERIV;
				else if (( !strcmp(name ,dataPtr->build.ID_out_d )) && (dataPtr->Modules[Mod_No]->type == DIFFINT))				which = SMOOTHED;
				else if (( !strcmp(name ,dataPtr->build.ID_out_e )) && (dataPtr->Modules[Mod_No]->type == DIFFINT))				which = DELAYED;
				else if (( !strcmp(name ,dataPtr->build.ID_out_f )) && (dataPtr->Modules[Mod_No]->type == DIFFINT))				which = INTEGRATED;

				else if (( !strcmp(name ,dataPtr->build.Ent_delayed )) && (dataPtr->Modules[Mod_No]->type == QTC))				which = DELAYED;
				else if (( !strcmp(name ,dataPtr->build.Ent_out )) && (dataPtr->Modules[Mod_No]->type == QTC))					which = MAX_ENTROPY;

				else if (( !strcmp(name ,dataPtr->build.FFT_delayedx )) && (dataPtr->Modules[Mod_No]->type == FFT))				which = DELAYED;
				else if (( !strcmp(name ,dataPtr->build.FFT_delayedy )) && (dataPtr->Modules[Mod_No]->type == FFT))				which = DELAYED2;
				else if (( !strcmp(name ,dataPtr->build.FFT_realout )) && (dataPtr->Modules[Mod_No]->type == FFT))				which = REALFFT;
				else if (( !strcmp(name ,dataPtr->build.FFT_imagout )) && (dataPtr->Modules[Mod_No]->type == FFT))				which = IMAGFFT;
				
				else if (( !strcmp(name ,dataPtr->build.Comp_delayed )) && (dataPtr->Modules[Mod_No]->type == COMPAR))			which = DELAYED;
				else if (( !strcmp(name ,dataPtr->build.Comp_out )) && (dataPtr->Modules[Mod_No]->type == COMPAR))				which = POS;
				else if (( !strcmp(name ,dataPtr->build.Comp_zero )) && (dataPtr->Modules[Mod_No]->type == COMPAR))				which = ZERO;
				else if (( !strcmp(name ,dataPtr->build.Comp_nonpos )) && (dataPtr->Modules[Mod_No]->type == COMPAR))			which = NONPOS;
				else if (( !strcmp(name ,dataPtr->build.Comp_neg )) && (dataPtr->Modules[Mod_No]->type == COMPAR))				which = NEG;
				else if (( !strcmp(name ,dataPtr->build.Comp_nonneg )) && (dataPtr->Modules[Mod_No]->type == COMPAR))			which = NONNEG;

				else if (( !strcmp(name ,dataPtr->build.Noise_out_name )) && (dataPtr->Modules[Mod_No]->type == NOISE))			which = NOISEOUT;

				else if (( !strcmp(name ,dataPtr->build.Filter_delayed )) && (dataPtr->Modules[Mod_No]->type == FILTER))		which = DELAYED;
				else if (( !strcmp(name ,dataPtr->build.Filter_out )) && (dataPtr->Modules[Mod_No]->type == FILTER))			which = FILTEROUT;

				else if (( !strcmp(name ,dataPtr->build.Plr_xdelayed )) && (dataPtr->Modules[Mod_No]->type == POLAR))			which = DELAYED;
				else if (( !strcmp(name ,dataPtr->build.Plr_ydelayed )) && (dataPtr->Modules[Mod_No]->type == POLAR))			which = DELAYED2;
				else if (( !strcmp(name ,dataPtr->build.Plr_zdelayed )) && (dataPtr->Modules[Mod_No]->type == POLAR))			which = DELAYED3;
				else if (( !strcmp(name ,dataPtr->build.Plr_theta )) && (dataPtr->Modules[Mod_No]->type == POLAR))				which = THETA;
				else if (( !strcmp(name ,dataPtr->build.Plr_phi )) && (dataPtr->Modules[Mod_No]->type == POLAR))				which = PHI;
				else if (( !strcmp(name ,dataPtr->build.Plr_norm )) && (dataPtr->Modules[Mod_No]->type == POLAR))				which = NORM;

				else if (( !strcmp(name ,dataPtr->build.Math_delayed )) && (dataPtr->Modules[Mod_No]->type == MATHMOD))			which = DELAYED;
				else if (( !strcmp(name ,dataPtr->build.Math_recip )) && (dataPtr->Modules[Mod_No]->type == MATHMOD))			which = RECIPROCAL;
				else if (( !strcmp(name ,dataPtr->build.Math_sin )) && (dataPtr->Modules[Mod_No]->type == MATHMOD))				which = SIN;
				else if (( !strcmp(name ,dataPtr->build.Math_exp )) && (dataPtr->Modules[Mod_No]->type == MATHMOD))				which = EXP;
				else if (( !strcmp(name ,dataPtr->build.Math_log )) && (dataPtr->Modules[Mod_No]->type == MATHMOD))				which = LOG;
				else if (( !strcmp(name ,dataPtr->build.Math_sqrt )) && (dataPtr->Modules[Mod_No]->type == MATHMOD))			which = SQRT;

				else if (( !strcmp(name ,dataPtr->build.Audio_leftout )) && (dataPtr->Modules[Mod_No]->type == AUDIO))			which = LEFTOUT;
				else if (( !strcmp(name ,dataPtr->build.Audio_rightout )) && (dataPtr->Modules[Mod_No]->type == AUDIO))			which = RIGHTOUT;
				else if (( !strcmp(name ,dataPtr->build.Audio_aout )) && (dataPtr->Modules[Mod_No]->type == AUDIO))				which = AOUT;
				else if (( !strcmp(name ,dataPtr->build.Audio_bout )) && (dataPtr->Modules[Mod_No]->type == AUDIO))				which = BOUT;
				else if (( !strcmp(name ,dataPtr->build.Audio_cout )) && (dataPtr->Modules[Mod_No]->type == AUDIO))				which = COUT;
				else if (( !strcmp(name ,dataPtr->build.Audio_dout )) && (dataPtr->Modules[Mod_No]->type == AUDIO))				which = DOUT;

				else if (( !strcmp(name ,dataPtr->build.Extern_arg1_label )) && (dataPtr->Modules[Mod_No]->type == EXTERN))		which = ARG1;
				else if (( !strcmp(name ,dataPtr->build.Extern_arg2_label )) && (dataPtr->Modules[Mod_No]->type == EXTERN))		which = ARG2;
				else if (( !strcmp(name ,dataPtr->build.Extern_arg3_label )) && (dataPtr->Modules[Mod_No]->type == EXTERN))		which = ARG3;
				else if (( !strcmp(name ,dataPtr->build.Extern_arg4_label )) && (dataPtr->Modules[Mod_No]->type == EXTERN))		which = ARG4;
				else if (( !strcmp(name ,dataPtr->build.Extern_arg5_label )) && (dataPtr->Modules[Mod_No]->type == EXTERN))		which = ARG5;
				else if (( !strcmp(name ,dataPtr->build.Extern_arg6_label )) && (dataPtr->Modules[Mod_No]->type == EXTERN))		which = ARG6;
				else which = -1;

// else {fprintf(stderr,"trouble, %s\n", dataPtr->build.ID_out_d); fflush(stderr);}

				if (which >=0) {
					pushto(dataPtr->mystackPtr, dataPtr->Modules[Mod_No]->Output[which]); // pushing onto stack a pointer to a field
//	fprintf (stderr,"name %s\n", nodePtr->Output[0]->name ); fflush(stderr);
				// then also assign signal name
					if (strlen(label))strncpy(dataPtr->Modules[Mod_No]->Output[which]->label, label, MAX_LABEL_LENG) ; 

				}
				else {
					fprintf(stderr, "Could not resolve the name: %s  associated with module No: %d Type: %d   ... exiting \n", name, Mod_No, dataPtr->Modules[Mod_No]->type);
					exit (1);
				}

			}
		} else { // i/o
	//debug		fprintf(stderr, "Input output\n"); fflush(stderr);

			Mod_No -= bound; 
			// check if a name assignment is being attempted ... needs to be done... and user warning notified

			if ((dataPtr->build.Input_channel_no > 0) && (Mod_No >= 0 ) && (Mod_No <= dataPtr->build.Input_channel_no )) {
				pushto(dataPtr->mystackPtr, dataPtr->IO_Modules[Mod_No]->put.In); // pushing onto stack
				Mod_No = -1;
	
			}
			else if (dataPtr->build.Input_channel_no > 0) Mod_No = Mod_No - dataPtr->build.Input_channel_no - 1; //  allow for the frame counter
			
			if ((dataPtr->build.Output_channel_no > 0) && (Mod_No >= 0 ) &&  (Mod_No <= dataPtr->build.Output_channel_no )) {
				pushto(dataPtr->mystackPtr, dataPtr->Output_Modules[Mod_No]->put.Out); // pushing onto stack
	//debug				fprintf(stderr, "pushing output onto the stack; no: %d type: %d name: %s, label: %s\n", Mod_No, dataPtr->Output_Modules[Mod_No]->put.Out->type, dataPtr->Output_Modules[Mod_No]->put.Out->name, dataPtr->Output_Modules[Mod_No]->put.Out->label); fflush(stderr);
				Mod_No = -1;
			}
			else if (dataPtr->build.Output_channel_no > 0) Mod_No = Mod_No - dataPtr->build.Output_channel_no - 1;  //  allow for the frame counter

			if ((dataPtr->build.Connection_build_size > 0) && (Mod_No >= 0 ) &&  (Mod_No < dataPtr->build.Connection_build_size )) {
				pushto(dataPtr->mystackPtr, dataPtr->lug_Modules[Mod_No]->put.In); // pushing onto stack
				Mod_No = -1;
			}

		}
	}
	else {
		fprintf(stderr, "Module No must be non-negative. Could not resolve:  %d    ... exiting\n", Mod_No);
		exit (1);
	
	}
}

/********************************************************************************************************************/
/*																													*/
/*	Enter Current connection in Tree																					*/
/*																													*/
/********************************************************************************************************************/
void	Enter_Current_Connection(struct xml_data *dataPtr) {
	int whichtree;

	if (dataPtr->currentconnection.output1_No >= 0 ) {
		enterfrom(dataPtr, dataPtr->currentconnection.output1_No, dataPtr->currentconnection.output1_name, dataPtr->currentconnection.output1_label) ; 
		if (dataPtr->currentconnection.output2_No >= 0 ) {
			enterfrom(dataPtr, dataPtr->currentconnection.output2_No, dataPtr->currentconnection.output2_name, dataPtr->currentconnection.output2_label) ;
			if (dataPtr->currentconnection.output3_No >= 0 ) {
				enterfrom(dataPtr, dataPtr->currentconnection.output3_No, dataPtr->currentconnection.output3_name, dataPtr->currentconnection.output3_label) ;
				if (dataPtr->currentconnection.output4_No >= 0 ) {
					enterfrom(dataPtr, dataPtr->currentconnection.output4_No, dataPtr->currentconnection.output4_name, dataPtr->currentconnection.output4_label) ;
					if (dataPtr->currentconnection.output5_No >= 0 ) {
						enterfrom(dataPtr, dataPtr->currentconnection.output5_No, dataPtr->currentconnection.output5_name, dataPtr->currentconnection.output5_label) ;
						if (dataPtr->currentconnection.output6_No >= 0 ) {
							enterfrom(dataPtr, dataPtr->currentconnection.output6_No, dataPtr->currentconnection.output6_name, dataPtr->currentconnection.output6_label) ;
						}
					}
				}
			}
		}
		if ( dataPtr->currentconnection.input_No >= 0 ) {
			enterfrom(dataPtr, dataPtr->currentconnection.input_No, dataPtr->currentconnection.input_name, dataPtr->currentconnection.input_label) ; // input

			
			set_constant (dataPtr->currentconnection.weight, dataPtr->mystackPtr);	
			set_offset (dataPtr->currentconnection.offset, dataPtr->mystackPtr);	
			
			// check to see if module is in the output (sampling) range.
			
			whichtree = dataPtr->build.build_size ;
			if (dataPtr->build.Input_channel_no) whichtree += dataPtr->build.Input_channel_no + 1; // allow for the framecounter
			
			
			if (dataPtr->currentconnection.input_No >= whichtree)  {
				add_connection ( dataPtr->SamplingTree, dataPtr->currentconnection.No, dataPtr->mystackPtr ); // if it is an output 
//	fprintf(stderr,"added to sampling tree\n"); fflush(stderr);
			}
			else {
				add_connection ( dataPtr->CircuitTree, dataPtr->currentconnection.No, dataPtr->mystackPtr ); // else it is a standard connection
//	fprintf(stderr,"added to circuit tree\n"); fflush(stderr);
			}

			dataPtr->currentconnection.Last_No = dataPtr->currentconnection.No;
			dataPtr->currentconnection.Last_Input_No = dataPtr->currentconnection.input_No;
		}

	}
	// else handle the connection when there is only the offset
	else {
		if (( dataPtr->currentconnection.input_No >= 0 ) && (dataPtr->currentconnection.offset)) {
			enterfrom(dataPtr, dataPtr->currentconnection.input_No, dataPtr->currentconnection.input_name, dataPtr->currentconnection.input_label) ; // input
			set_constant (0, dataPtr->mystackPtr);	dataPtr->currentconnection.Last_weight = 0; // ensure it is set to 0
			set_offset (dataPtr->currentconnection.offset, dataPtr->mystackPtr);	
			// check to see if module is in the output (sampling) range.
			
			whichtree = dataPtr->build.build_size ;
			if (dataPtr->build.Input_channel_no) whichtree += dataPtr->build.Input_channel_no + 1; // allow for the framecounter
			
			
			if (dataPtr->currentconnection.input_No >= whichtree)  {
				add_connection ( dataPtr->SamplingTree, dataPtr->currentconnection.No, dataPtr->mystackPtr ); // if it is an output 
//	fprintf(stderr,"added to sampling tree\n"); fflush(stderr);
			}
			else {
				add_connection ( dataPtr->CircuitTree, dataPtr->currentconnection.No, dataPtr->mystackPtr ); // else it is a standard connection
//	fprintf(stderr,"added to circuit tree\n"); fflush(stderr);
			}

			dataPtr->currentconnection.Last_No = dataPtr->currentconnection.No;
			dataPtr->currentconnection.Last_Input_No = dataPtr->currentconnection.input_No;
		}
	
	
	}
	init_connection( &dataPtr->currentconnection);
}


/********************************************************************************************************************/
/*																													*/
/*	find																											*/
/*																													*/
/********************************************************************************************************************/

int Find_Name(struct xml_data *dataPtr, char* Module_Name) {

	int i,j, which;

	i = 0;
	which = dataPtr->build.build_size;
	
	while ((i  < which ) && ( 0 != strcmp( Module_Name, dataPtr->Modules[i]->name)))  i++;
	if (i  < which) return i;
	else if (dataPtr->build.Input_channel_no) {
		which += dataPtr->build.Input_channel_no + 1; 		// increase to allow for Input modules
		j = 0;
		while ((i  < which ) && ( 0 != strcmp( Module_Name, dataPtr->IO_Modules[j]->name )))  {j++; i++;}
	}
	if (i  < which) return i;
	else if (dataPtr->build.Output_channel_no) {
		which += dataPtr->build.Output_channel_no + 1;		// increase to allow for Output modules
		j = 0;
		while ((i  < which ) && ( 0 != strcmp( Module_Name, dataPtr->Output_Modules[j]->name))) {j++; i++;}	
	}
	if (i < which) return i;
	else if (dataPtr->build.Connection_build_size) {
		which += dataPtr->build.Connection_build_size ;		// increase to allow for Connection modules
		j = 0;
		while ((i  < which ) && ( 0 != strcmp( Module_Name, dataPtr->lug_Modules[j]->name))) {j++; i++;}	
	}
	if (i < which) return i;
	else return (-1);
}

/********************************************************************************************************************/
/*																													*/
/*	Have a module name.... now look in the trees to find the corresponding module number																						*/
/*																													*/
/********************************************************************************************************************/
// the idea here is to look up the input_name,and the output_names in the module and i/o trees, to obtain the corresponding module number (key)
// if the input_No and output_No's are already assigned then check for consistency, else fill in the No's

int Translate_Names_Nos(struct xml_data *dataPtr) {

	int	error = 0;
	int	Module_Number;

	if ( strlen(dataPtr->currentconnection.input_module)) {
		 Module_Number = Find_Name(dataPtr, dataPtr->currentconnection.input_module);
		if ((dataPtr->currentconnection.input_No >= 0) && (dataPtr->currentconnection.input_No != Module_Number)) {
			error++;
			fprintf(stderr,"In connection %d, Input Module No: %d does not correspond to Module Name: %s\n", dataPtr->currentconnection.No, dataPtr->currentconnection.input_No, dataPtr->currentconnection.input_module); fflush(stderr); 
		}
		else dataPtr->currentconnection.input_No = Module_Number;
	}

	if ( strlen(dataPtr->currentconnection.output1_module)) {
		 Module_Number = Find_Name(dataPtr, dataPtr->currentconnection.output1_module);
		if ((dataPtr->currentconnection.output1_No >= 0) && (dataPtr->currentconnection.output1_No != Module_Number)) {
			error++;
			fprintf(stderr,"In connection %d, Output[1] Module No: %d does not correspond to Module Name: %s\n", dataPtr->currentconnection.No, dataPtr->currentconnection.output1_No, dataPtr->currentconnection.output1_module); fflush(stderr); 
		}
		else dataPtr->currentconnection.output1_No = Module_Number;
	}
	
	if ( strlen(dataPtr->currentconnection.output2_module)) {
		 Module_Number = Find_Name(dataPtr, dataPtr->currentconnection.output2_module);
		if ((dataPtr->currentconnection.output2_No >= 0) && (dataPtr->currentconnection.output2_No != Module_Number)) {
			error++;
			fprintf(stderr,"In connection %d, Output[2] Module No: %d does not correspond to Module Name: %s\n", dataPtr->currentconnection.No, dataPtr->currentconnection.output2_No, dataPtr->currentconnection.output2_module); fflush(stderr); 
		}
		else dataPtr->currentconnection.output2_No = Module_Number;
	}
	
	if ( strlen(dataPtr->currentconnection.output3_module)) {
		 Module_Number = Find_Name(dataPtr, dataPtr->currentconnection.output3_module);
		if ((dataPtr->currentconnection.output3_No >= 0) && (dataPtr->currentconnection.output3_No != Module_Number)) {
			error++;
			fprintf(stderr,"In connection %d, Output[3] Module No: %d does not correspond to Module Name: %s\n", dataPtr->currentconnection.No, dataPtr->currentconnection.output3_No, dataPtr->currentconnection.output3_module); fflush(stderr); 
		}
		else dataPtr->currentconnection.output3_No = Module_Number;
	}
	
	if ( strlen(dataPtr->currentconnection.output4_module)) {
		 Module_Number = Find_Name(dataPtr, dataPtr->currentconnection.output4_module);
		if ((dataPtr->currentconnection.output4_No >= 0) && (dataPtr->currentconnection.output4_No != Module_Number)) {
			error++;
			fprintf(stderr,"In connection %d, Output[4] Module No: %d does not correspond to Module Name: %s\n", dataPtr->currentconnection.No, dataPtr->currentconnection.output4_No, dataPtr->currentconnection.output4_module); fflush(stderr); 
		}
		else dataPtr->currentconnection.output4_No = Module_Number;
	}
	
	if ( strlen(dataPtr->currentconnection.output5_module)) {
		 Module_Number = Find_Name(dataPtr, dataPtr->currentconnection.output5_module);
		if ((dataPtr->currentconnection.output5_No >= 0) && (dataPtr->currentconnection.output5_No != Module_Number)) {
			error++;
			fprintf(stderr,"In connection %d, Output[5] Module No: %d does not correspond to Module Name: %s\n", dataPtr->currentconnection.No, dataPtr->currentconnection.output5_No, dataPtr->currentconnection.output5_module); fflush(stderr); 
		}
		else dataPtr->currentconnection.output5_No = Module_Number;
	}
	
	if ( strlen(dataPtr->currentconnection.output6_module)) {
		 Module_Number = Find_Name(dataPtr, dataPtr->currentconnection.output6_module);
		if ((dataPtr->currentconnection.output6_No >= 0) && (dataPtr->currentconnection.output6_No != Module_Number)) {
			error++;
			fprintf(stderr,"In connection %d, Output[6] Module No: %d does not correspond to Module Name: %s\n", dataPtr->currentconnection.No, dataPtr->currentconnection.output6_No, dataPtr->currentconnection.output6_module); fflush(stderr); 
		}
		else dataPtr->currentconnection.output6_No = Module_Number;
	}

	
	return error ; //all went ok?
}

/********************************************************************************************************************/
/*																													*/
/*	connections and sampling																						*/
/*																													*/
/********************************************************************************************************************/
// will continue use the current module struct to temporarily store the data.

static void XMLCALL
end_connections_Element(void *userData, const char *name)
{
	struct xml_data *dataPtr = (struct xml_data *) userData;
	int *depthPtr = (int *) &dataPtr->depth;
	float tempfloat;

	if (*depthPtr == 4) {
		if (!strcmp("key", dataPtr->tag)) {

			if (!strcmp(TAG_CON_NO, dataPtr->content)) 	 					dataPtr->key = CON_NO;

			else if (!strcmp(TAG_CON_INPUT_NO, dataPtr->content)) 	 		dataPtr->key = CON_INPUT_NO;
			else if (!strcmp(TAG_CON_INPUT_MOD_LBL, dataPtr->content)) 	 	dataPtr->key = CON_INPUT_MOD_LBL;
			else if (!strcmp(TAG_CON_INPUT_NAME, dataPtr->content)) 	 	dataPtr->key = CON_INPUT_NAME;
			else if (!strcmp(TAG_CON_INPUT_LABLE, dataPtr->content)) 	 	dataPtr->key = CON_INPUT_LABLE;

			else if (!strcmp(TAG_CON_OFFSET, dataPtr->content)) 	 		dataPtr->key = CON_OFFSET;
			else if (!strcmp(TAG_CON_WEIGHT, dataPtr->content)) 	 		dataPtr->key = CON_WEIGHT;

			else if (!strcmp(TAG_CON_OUTPUT1_NO, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT1_NO; 
			else if (!strcmp(TAG_CON_OUTPUT2_NO, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT2_NO;
			else if (!strcmp(TAG_CON_OUTPUT3_NO, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT3_NO;
			else if (!strcmp(TAG_CON_OUTPUT4_NO, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT4_NO;
			else if (!strcmp(TAG_CON_OUTPUT5_NO, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT5_NO;
			else if (!strcmp(TAG_CON_OUTPUT6_NO, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT6_NO;

			else if (!strcmp(TAG_CON_OUTPUT1_MOD_LBL, dataPtr->content)) 	dataPtr->key = CON_OUTPUT1_MOD_LBL;
			else if (!strcmp(TAG_CON_OUTPUT2_MOD_LBL, dataPtr->content)) 	dataPtr->key = CON_OUTPUT2_MOD_LBL;
			else if (!strcmp(TAG_CON_OUTPUT3_MOD_LBL, dataPtr->content)) 	dataPtr->key = CON_OUTPUT3_MOD_LBL;
			else if (!strcmp(TAG_CON_OUTPUT4_MOD_LBL, dataPtr->content)) 	dataPtr->key = CON_OUTPUT4_MOD_LBL;
			else if (!strcmp(TAG_CON_OUTPUT5_MOD_LBL, dataPtr->content)) 	dataPtr->key = CON_OUTPUT5_MOD_LBL;
			else if (!strcmp(TAG_CON_OUTPUT6_MOD_LBL, dataPtr->content)) 	dataPtr->key = CON_OUTPUT6_MOD_LBL;

			else if (!strcmp(TAG_CON_OUTPUT1_NAME, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT1_NAME;
			else if (!strcmp(TAG_CON_OUTPUT2_NAME, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT2_NAME;
			else if (!strcmp(TAG_CON_OUTPUT3_NAME, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT3_NAME;
			else if (!strcmp(TAG_CON_OUTPUT4_NAME, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT4_NAME;
			else if (!strcmp(TAG_CON_OUTPUT5_NAME, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT5_NAME;
			else if (!strcmp(TAG_CON_OUTPUT6_NAME, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT6_NAME;

			else if (!strcmp(TAG_CON_OUTPUT1_LBL, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT1_LBL;
			else if (!strcmp(TAG_CON_OUTPUT2_LBL, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT2_LBL;
			else if (!strcmp(TAG_CON_OUTPUT3_LBL, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT3_LBL;
			else if (!strcmp(TAG_CON_OUTPUT4_LBL, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT4_LBL;
			else if (!strcmp(TAG_CON_OUTPUT5_LBL, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT5_LBL;
			else if (!strcmp(TAG_CON_OUTPUT6_LBL, dataPtr->content)) 	 	dataPtr->key = CON_OUTPUT6_LBL;
			else dataPtr->key = -1;
		}
		else if (!strcmp("string",dataPtr->tag)) {
			switch (dataPtr->key) {
	
				case CON_NO:				sscanf(dataPtr->content,"%d", &dataPtr->currentconnection.No);										// connection number
												if ( (dataPtr->content[0] == '+') || (dataPtr->content[0] == '-') ) 
													dataPtr->currentconnection.No += dataPtr->currentconnection.Last_No;
												dataPtr->currentconnection.Last_No = dataPtr->currentconnection.No;
												break;

				case CON_INPUT_NO:			sscanf(dataPtr->content,"%d", &dataPtr->currentconnection.input_No);								// module number
												if ( (dataPtr->content[0] == '+') || (dataPtr->content[0] == '-') ) 
													dataPtr->currentconnection.input_No += dataPtr->currentconnection.Last_Input_No;
												dataPtr->currentconnection.Last_Input_No = dataPtr->currentconnection.input_No;
												break;
				case CON_INPUT_MOD_LBL:		strcpy(dataPtr->currentconnection.input_module, dataPtr->content);		break;						// module name
				case CON_INPUT_LABLE:		strcpy(dataPtr->currentconnection.input_label, dataPtr->content);		break;						// module node label
				case CON_INPUT_NAME:		strcpy(dataPtr->currentconnection.input_name, dataPtr->content);		break;						// connection signal name


				case CON_OUTPUT1_NO:		sscanf(dataPtr->content,"%d", &dataPtr->currentconnection.output1_No);				// input always read before output because alphabetic order of plist ! good assumption ?
												if ( (dataPtr->content[0] == '+') || (dataPtr->content[0] == '-') ) 			// relative to current input module number 
													dataPtr->currentconnection.output1_No += dataPtr->currentconnection.Last_Input_No;
//fprintf(stderr,"module %d \n",dataPtr->currentconnection.output1_No); fflush(stderr);
												break;

				case CON_OUTPUT2_NO:		sscanf(dataPtr->content,"%d", &dataPtr->currentconnection.output2_No);	
												if ( (dataPtr->content[0] == '+') || (dataPtr->content[0] == '-') ) 			// relative to current input module number 
													dataPtr->currentconnection.output2_No += dataPtr->currentconnection.Last_Input_No;
												break;
				case CON_OUTPUT3_NO:		sscanf(dataPtr->content,"%d", &dataPtr->currentconnection.output3_No);	
												if ( (dataPtr->content[0] == '+') || (dataPtr->content[0] == '-') ) 			// relative to current input module number 
													dataPtr->currentconnection.output3_No += dataPtr->currentconnection.Last_Input_No;
												break;
				case CON_OUTPUT4_NO:		sscanf(dataPtr->content,"%d", &dataPtr->currentconnection.output4_No);	
												if ( (dataPtr->content[0] == '+') || (dataPtr->content[0] == '-') ) 			// relative to current input module number 
													dataPtr->currentconnection.output4_No += dataPtr->currentconnection.Last_Input_No;
												break;
				case CON_OUTPUT5_NO:		sscanf(dataPtr->content,"%d", &dataPtr->currentconnection.output5_No);	
												if ( (dataPtr->content[0] == '+') || (dataPtr->content[0] == '-') ) 			// relative to current input module number 
													dataPtr->currentconnection.output5_No += dataPtr->currentconnection.Last_Input_No;
												break;
				case CON_OUTPUT6_NO:		sscanf(dataPtr->content,"%d", &dataPtr->currentconnection.output6_No);	
												if ( (dataPtr->content[0] == '+') || (dataPtr->content[0] == '-') ) 			// relative to current input module number 
													dataPtr->currentconnection.output6_No += dataPtr->currentconnection.Last_Input_No;
												break;

				case CON_OUTPUT1_MOD_LBL:	strcpy(dataPtr->currentconnection.output1_module, dataPtr->content);	break;
				case CON_OUTPUT2_MOD_LBL:	strcpy(dataPtr->currentconnection.output2_module, dataPtr->content);	break;
				case CON_OUTPUT3_MOD_LBL:	strcpy(dataPtr->currentconnection.output3_module, dataPtr->content);	break;
				case CON_OUTPUT4_MOD_LBL:	strcpy(dataPtr->currentconnection.output4_module, dataPtr->content);	break;
				case CON_OUTPUT5_MOD_LBL:	strcpy(dataPtr->currentconnection.output5_module, dataPtr->content);	break;
				case CON_OUTPUT6_MOD_LBL:	strcpy(dataPtr->currentconnection.output6_module, dataPtr->content);	break;

				case CON_OUTPUT1_NAME:		strcpy(dataPtr->currentconnection.output1_name, dataPtr->content);		break;
//fprintf(stderr,"name %s \n",dataPtr->currentconnection.output1_name); fflush(stderr); break;

				
				case CON_OUTPUT2_NAME:		strcpy(dataPtr->currentconnection.output2_name, dataPtr->content);		break;
				case CON_OUTPUT3_NAME:		strcpy(dataPtr->currentconnection.output3_name, dataPtr->content);		break;
				case CON_OUTPUT4_NAME:		strcpy(dataPtr->currentconnection.output4_name, dataPtr->content);		break;
				case CON_OUTPUT5_NAME:		strcpy(dataPtr->currentconnection.output5_name, dataPtr->content);		break;
				case CON_OUTPUT6_NAME:		strcpy(dataPtr->currentconnection.output6_name, dataPtr->content);		break;

				case CON_OUTPUT1_LBL:		strcpy(dataPtr->currentconnection.output1_label, dataPtr->content);		break;
				case CON_OUTPUT2_LBL:		strcpy(dataPtr->currentconnection.output2_label, dataPtr->content);		break;
				case CON_OUTPUT3_LBL:		strcpy(dataPtr->currentconnection.output3_label, dataPtr->content);		break;
				case CON_OUTPUT4_LBL:		strcpy(dataPtr->currentconnection.output4_label, dataPtr->content);		break;
				case CON_OUTPUT5_LBL:		strcpy(dataPtr->currentconnection.output5_label, dataPtr->content);		break;
				case CON_OUTPUT6_LBL:		strcpy(dataPtr->currentconnection.output6_label, dataPtr->content);		break;

				case CON_OFFSET:			if  (dataPtr->content[0] == '+' ) {			// assume relative to last offset 
												sscanf(&dataPtr->content[1],"%f", &tempfloat);
												dataPtr->currentconnection.offset = dataPtr->currentconnection.Last_offset + (double) tempfloat;
											}
											else {
												sscanf(dataPtr->content,"%f", &tempfloat);
												dataPtr->currentconnection.offset = (double) tempfloat;	
											}	
											dataPtr->currentconnection.Last_offset = dataPtr->currentconnection.offset;
											break;
											
				case CON_WEIGHT:			if  (dataPtr->content[0] == '+' ) {			// assume relative to last offset 
												sscanf(&dataPtr->content[1],"%f", &tempfloat);
												dataPtr->currentconnection.weight = dataPtr->currentconnection.Last_weight + (double) tempfloat;
											}
											else {
												sscanf(dataPtr->content,"%f", &tempfloat);
												dataPtr->currentconnection.weight = (double) tempfloat;		
											}
											dataPtr->currentconnection.Last_weight = dataPtr->currentconnection.weight;
											break;
				default:
					break;			
			}
		}
		else if (!strcmp("real",dataPtr->tag)){
			switch (dataPtr->key) {
				case CON_OFFSET:			if  (dataPtr->content[0] == '+' ) {			// assume relative to last offset 
												sscanf(&dataPtr->content[1],"%f", &tempfloat);
												dataPtr->currentconnection.offset = dataPtr->currentconnection.Last_offset + (double) tempfloat;
											}
											else {
												sscanf(dataPtr->content,"%f", &tempfloat);
												dataPtr->currentconnection.offset = (double) tempfloat;	
											}	
											dataPtr->currentconnection.Last_offset = dataPtr->currentconnection.offset;
											break;

				case CON_WEIGHT:			if  (dataPtr->content[0] == '+' ) {			// assume relative to last offset 
												sscanf(&dataPtr->content[1],"%f", &tempfloat);
												dataPtr->currentconnection.weight = dataPtr->currentconnection.Last_weight + (double) tempfloat;
											}
											else {
												sscanf(dataPtr->content,"%f", &tempfloat);
												dataPtr->currentconnection.weight = (double) tempfloat;		
											}
											dataPtr->currentconnection.Last_weight = dataPtr->currentconnection.weight;
											break;
				default:
					break;			
			}
		}	
	}
// now transfer the details into the module array but check validity of entries as one goes
	else if ( (*depthPtr == 3) && (!strcmp("dict",name))) {


		if (dataPtr->currentconnection.No == -1) {
				fprintf(stderr,"Missing connection number .... exiting \n"); fflush(stderr); exit (1);
		}

// good place to check the module name info... and tie up the module numbers
		if ( Translate_Names_Nos(dataPtr) ) {fprintf(stderr," .... exiting \n"); fflush(stderr); exit (1); } // errors noted


// Enter the connection in the tree
		Enter_Current_Connection(dataPtr);

	} 
	
	*depthPtr -= 1;
}

/********************************************************************************************************************/
// getting started ... will simplify this in due course  
int write_tree2file(char *name, struct Node *root, struct xml_data *xml_dataPtr ) {

	FILE *fd;

	fd = fopen( name, "w+");
	if (fd) {
		print_pheader(fd);
		InorderXML(fd, root, NAME, xml_dataPtr);  // save  connections
		print_pclose(fd);
		return 0;
	}
	fprintf(stderr, "failed to write file %s\n", name); fflush(stderr);
	return 1;
}

int write_trees2file(char *name, struct xml_data *xml_dataPtr ) {

	FILE *fd;

}


/********************************************************************************************************************/

void Circuit_Names_debug(struct xml_data *DataPtr) {	
	
	FILE *fd;
	char Filename[FILE_NAME_SIZE];	
	 
	sprintf(Filename,"%s~.%s.pertPp", DataPtr->config_name, DataPtr->build.ext_circ); 
	fd = fopen( Filename, "w+");
	if (!fd) {
		fprintf(stderr,"Problem creating file: %s\n", Filename); fflush(stderr);
	} 
	else {
		fprintf(stderr,"Creating file: %s\n", Filename); fflush(stderr);
		print_pheader(fd);
		InorderXML(fd, DataPtr->CircuitTree, NAME, DataPtr); // save circuit connections
		InorderXML(fd, DataPtr->SamplingTree, NAME, DataPtr); // save circuit connections
		print_pclose(fd);
	}
}




/********************************************************************************************************************/
/********** connections *******/
int load_connections( struct xml_data *dataPtr ) {
	char Filename[FILE_NAME_SIZE];	
	FILE *fd;
 
/* lets get some connections */

	init_connection( &dataPtr->currentconnection);
	dataPtr->currentconnection.Last_No = 0;
	dataPtr->currentconnection.Last_Input_No = 0;
	dataPtr->currentconnection.Last_offset = 0;
	dataPtr->currentconnection.Last_weight = 0;

//***************************************************************	
//  circuit and sampling now done as one file 
	dataPtr->currenttree = dataPtr->CircuitTree;

	if (dataPtr->config_name == NULL) fd = stdin;
	else {

		sprintf(Filename,"%s.%s.pertPp", dataPtr->config_name, dataPtr->build.ext_circ); 
		fd = fopen (&Filename[0],"r+");
		if (!fd) {
			if ( write_tree2file(Filename, dataPtr->CircuitTree, dataPtr)) return 1; 
			fprintf(stderr,"Creating circuit file: %s\n", Filename); fflush(stderr);

			sprintf(Filename,"open %s.%s.pertPp", dataPtr->config_name, dataPtr->build.ext_circ); 
			system(Filename);

			return 1;
		}
	}
	//  read the connections file
 
 	if (parse_file(fd, end_connections_Element, dataPtr) ) return 1;
// 	else  fprintf ( stderr, "Connections complete '%s'\n",Filename); fflush(stderr);

}

