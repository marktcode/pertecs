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


#if defined(__amigaos__) && defined(__USE_INLINE__)
#include <proto/expat.h>
#endif

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


//****************************************************************************************//
//****************************************************************************************//

char *tags[] = {
TAG_INPT_LABEL,
TAG_INPT_CHAN_NO,
TAG_INPT_NAME,
TAG_POT_LABEL,
TAG_POT_CHAN_NO,
TAG_POT_OUTX,
TAG_POT_OUTY,
TAG_POT_OUTZ,
TAG_POT_SELX,
TAG_POT_SELY,
TAG_POT_SELZ,
TAG_RND_LABEL,
TAG_RND_BUILD_SIZE,
TAG_RND_OUTPUT,
TAG_OUTPT_LABEL,
TAG_OUTPT_CHNL_NO,
TAG_OUTPT_NAME,
TAG_CNXN_LABEL,
TAG_CNXN_BUILD_SIZE,
TAG_CNXN_LUG,
TAG_SCP_LABEL,
TAG_SCP_CHAN_NO,
TAG_SCP_XINPUT,
TAG_SCP_YINPUT,
TAG_SCP_ZINPUT,
TAG_SCP_ORIENTA,
TAG_SCP_ORIENTB,
TAG_DLY_MOD_LABEL,
TAG_DLY_BUILD_SIZE,
TAG_DLY_INPUT,
TAG_DLY_IN_SH,
TAG_DLY_OUTPUT,
TAG_ID_BUILD_SIZE,
TAG_ID_MOD_LABEL,
TAG_ID_OUT_A,
TAG_ID_OUT_B,
TAG_ID_OUT_C,
TAG_ID_OUT_D,
TAG_ID_OUT_E,
TAG_ID_OUT_F,
TAG_ID_IN_I,
TAG_ID_IN_J,
TAG_ID_IN_SH,
TAG_ENT_BUILD_SIZE,
TAG_ENT_MOD_LABEL,
TAG_ENT_OUT_DELAYED,
TAG_ENT_OUT_ENTROPY,
TAG_ENT_INPUT,
TAG_COMP_BUILD_SIZE,
TAG_COMP_MOD_LABEL,
TAG_COMP_DELAYED,
TAG_COMP_POS,
TAG_COMP_ZERO,
TAG_COMP_NONPOS,
TAG_COMP_NEG,
TAG_COMP_NONNEG,
TAG_COMP_INPUT,
TAG_FFT_MOD_LABEL,
TAG_FFT_BUILD_SIZE,
TAG_FFT_INPUTX,
TAG_FFT_INPUTY,
TAG_FFT_SYNC,
TAG_FFT_REALOUT,
TAG_FFT_IMAGOUT,
TAG_FFT_DELAYEDX,
TAG_FFT_DELAYEDY,
TAG_FIR_BUILD_SIZE,
TAG_FIR_MOD_LABEL,
TAG_FIR_OUT_DELAYED,
TAG_FIR_OUT,
TAG_FIR_INPUT,
TAG_PLR_MOD_LABEL,
TAG_PLR_BUILD_SIZE,
TAG_PLR_XINPUT,
TAG_PLR_YINPUT,
TAG_PLR_ZINPUT,
TAG_PLR_THETA,
TAG_PLR_PHI,
TAG_PLR_NORM,
TAG_PLR_XDELAY,
TAG_PLR_YDELAY,
TAG_PLR_ZDELAY,
TAG_AUDIO_MOD_LABEL,
TAG_AUDIO_BUILD_SIZE,
TAG_AUDIO_INL,
TAG_AUDIO_INR,
TAG_AUDIO_OUT_OPTN,
TAG_AUDIO_OUTL,
TAG_AUDIO_OUTR,
TAG_AUDIO_OUTA,
TAG_AUDIO_OUTB,
TAG_AUDIO_OUTC,
TAG_AUDIO_OUTD,
TAG_MATH_BUILD_SIZE,
TAG_MATH_MOD_LABEL,
TAG_MATH_DELAYED,
TAG_MATH_SQRT,
TAG_MATH_LOG,
TAG_MATH_EXP,
TAG_MATH_SIN,
TAG_MATH_RECIP,
TAG_MATH_INPUT,
TAG_EXTRN_MOD_LABEL,
TAG_EXTRN_BUILD_SIZE,
TAG_EXTRN_ARGX,
TAG_EXTRN_ARGY,
TAG_EXTRN_PUSHZ,
TAG_EXTRN_ARG1,
TAG_EXTRN_ARG2,
TAG_EXTRN_ARG3,
TAG_EXTRN_ARG4,
TAG_EXTRN_ARG5,
TAG_EXTRN_ARG6,
TAG_EXT_BUILD,
TAG_EXT_INOT,
TAG_EXT_PREST,
TAG_EXT_INIT,
TAG_EXT_CIRC,
TAG_EXT_FILT,
TAG_EXT_DISP,
TAG_EXT_VIEW,
TAG_SYS_STOP,
TAG_SYS_PAUSE,
TAG_SYS_RESET,
TAG_RATE,
STR_ORIGIN_X,
STR_ORIGIN_Y,
STR_DIMENSION_X,
STR_DIMENSION_Y,
STR_TITLE,
STR_C1,
STR_C2,
STR_C3,
STR_C4,
STR_C5,
STR_C6,
STR_C7,
STR_G1,
STR_G2,
STR_G3,
STR_G4,
STR_G5,
STR_G6,
STR_G7,
STR_G8,
STR_G9,
STR_G10,
STR_G11,
STR_G12,
STR_G13,
STR_G14,
STR_G15,
STR_G_Origin,
STR_G_Labels,
TAG_AXIS_LOCK,
TAG_AXIS_LIGHT_LOCK,
TAG_MKRS_LIGHT_LOCK,
TAG_SFCE_LIGHT_LOCK,
STR_L1,
STR_L2,
STR_L3,
STR_L4,
STR_L5,
STR_L6,
STR_O1,
STR_O2,
STR_SURFACE_RED,
STR_SURFACE_GREEN,
STR_SURFACE_BLUE,
STR_SURFACE_INTSTY,
STR_SURFACE_SHADOW
};


//****************************************************************************************//
//****************************************************************************************//

int tag_assigns[] = {
INPUT_LABEL,
INPUT_CHNL_NO,
INPUT_NAME,
POT_LABEL,
POT_CHNL_NO,
POT_OUTPUTX,
POT_OUTPUTY,
POT_OUTPUTZ,
POT_SELECTX,
POT_SELECTY,
POT_SELECTZ,
RND_LABEL,
RND_CHNL_NO,
RND_NAME,
OUTPUT_LABEL,
OUTPUT_CHNL_NO,
OUTPUT_NAME,
CNXN_LABEL,
CNXN_BUILD_SIZE,
CNXN_LUG,
SCP_LABEL,
SCP_CHAN_NO,
SCP_XINPUT,
SCP_YINPUT,
SCP_ZINPUT,
SCP_ORIENTA,
SCP_ORIENTB,
DLY_MOD_LABEL,
DLY_BUILD_SIZE,
DLY_INPUT,
DLY_IN_SH,
DLY_OUTPUT,
ID_BUILD_SIZE,
ID_MOD_LABEL,
ID_OUT_A,
ID_OUT_B,
ID_OUT_C,
ID_OUT_D,
ID_OUT_E,
ID_OUT_F,
ID_IN_I,
ID_IN_J,
ID_IN_SH,
ENT_BUILD_SIZE,
ENT_MOD_LABEL,
ENT_OUT_DELAYED,
ENT_OUT_ENTROPY,
ENT_INPUT,
COMP_BUILD_SIZE,
COMP_MOD_LABEL,
COMP_DELAYED,
COMP_POS,
COMP_ZERO,
COMP_NONPOS,
COMP_NEG,
COMP_NONNEG,
COMP_INPUT,
FFT_MOD_LABEL,
FFT_BUILD_SIZE,
FFT_INPUTX,
FFT_INPUTY,
FFT_SYNC,
FFT_REALOUT,
FFT_IMAGOUT,
FFT_DELAYEDX,
FFT_DELAYEDY,
FIR_BUILD_SIZE,
FIR_MOD_LABEL,
FIR_OUT_DELAYED,
FIR_OUT,
FIR_INPUT,
PLR_MOD_LABEL,
PLR_BUILD_SIZE,
PLR_XINPUT,
PLR_YINPUT,
PLR_ZINPUT,
PLR_THETA,
PLR_PHI,
PLR_NORM,
PLR_XDELAY,
PLR_YDELAY,
PLR_ZDELAY,
AUDIO_MOD_LABEL,
AUDIO_BUILD_SIZE,
AUDIO_INPUTL,
AUDIO_INPUTR,
AUDIO_OUT_OPT,
AUDIO_OUTPUTL,
AUDIO_OUTPUTR,
AUDIO_OUTPUTA,
AUDIO_OUTPUTB,
AUDIO_OUTPUTC,
AUDIO_OUTPUTD,
MATH_BUILD_SIZE,
MATH_MOD_LABEL,
MATH_DELAYED,
MATH_SQRT,
MATH_LOG,
MATH_EXP,
MATH_SIN,
MATH_RECIP,
MATH_INPUT,
EXTRN_MOD_LABEL,
EXTRN_BUILD_SIZE,
EXTRN_ARGX,
EXTRN_ARGY,
EXTRN_PUSHZ,
EXTRN_ARG1,
EXTRN_ARG2,
EXTRN_ARG3,
EXTRN_ARG4,
EXTRN_ARG5,
EXTRN_ARG6,
EXT_BILD,
EXT_INOT,
EXT_PREST,
EXT_INIT,
EXT_CIRC,
EXT_FILT,
EXT_DISP,
EXT_VIEW,
SYSTEM_STOP,
SYSTEM_PAUSE,
SYSTEM_RESET,
SYSTEM_RATE,
ORIGIN_X,
ORIGIN_Y,
DIMENSION_X,
DIMENSION_Y,
TITLE,
C1,
C2,
C3,
C4,
C5,
C6,
C7,
G1,
G2,
G3,
G4,
G5,
G6,
G7,
G8,
G9,
G10,
G11,
G12,
G13,
G14,
G15,
G_Origin,
G_Labels,
LOCK_AXIS,
LOCK_AXIS_LIGHT,
LOCK_MKRS_LIGHT,
LOCK_SFCE_LIGHT,
L1,
L2,
L3,
L4,
L5,
L6,
O1,
O2,
SURFACE_RED,
SURFACE_GREEN,
SURFACE_BLUE,
SURFACE_INTSTY,
SURFACE_SHADOW
};
//****************************************************************************************//
//****************************************************************************************//





void	print_pheader(FILE *fd) { // print plist header
	fprintf(fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n");
	fprintf(fd, "<array>\n"); fflush(fd);
}

void	print_pclose(FILE *fd) {
	fprintf(fd, "</array>\n</plist>\n");
	fclose(fd);
}




/********************************************************************************************************************/
/*					*/
/*	xml				*/
/*					*/
/********************************************************************************************************************/

void XMLCALL
startElement(void *userData, const char *name, const char **atts)
{
	int i;
	struct xml_data *dataPtr = (struct xml_data *) userData;
	int *depthPtr = (int *) &dataPtr->depth;


	strncpy(dataPtr->tag, name, XML_STR_SIZE );
	*depthPtr += 1;
	dataPtr->content_index = 0;
}

void XMLCALL
charhndl(void *userData, const XML_Char *s, int len)
{
	int i;
  
	struct xml_data *dataPtr = (struct xml_data *) userData;
  
	for (i=0; i < len; i++) dataPtr->content[dataPtr->content_index++] = s[i];
	dataPtr->content[dataPtr->content_index] = 0;
}




int parse_file(FILE *fd, void *endElement, struct xml_data *xml_dataPtr) {
	XML_Parser parser;
	int done;
	char buf[BUFSIZ];
 	
  	parser = XML_ParserCreate(NULL);
  	
 	xml_dataPtr->depth = 0;
	XML_SetUserData(parser, xml_dataPtr);
	
	XML_SetElementHandler(parser, startElement, endElement);
	XML_SetCharacterDataHandler(parser, charhndl) ;

	
	do {
		int len = (int)fread(buf, 1, sizeof(buf), fd);
		done = len < sizeof(buf);
		
		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
		  fprintf(stderr,
				  "%s at line %" XML_FMT_INT_MOD "u\n",
				  XML_ErrorString(XML_GetErrorCode(parser)),
				  XML_GetCurrentLineNumber(parser));
		  return 1;
		}
	} while (!done);
	
	if (fd != stdin) fclose(fd);
	XML_ParserFree(parser);

	return 0;
}







//****************************************************************************************//
//****************************************************************************************//
//****************************************************************************************//


			


/********************************************************************************************************************/
/*					*/
/*	build			*/
/*					*/
/********************************************************************************************************************/
static void XMLCALL
end_Build_Element(void *userData, const char *name)
{
	struct xml_data *dataPtr = (struct xml_data *) userData;
	int *depthPtr = (int *) &dataPtr->depth;
	int NumTags = sizeof tags / sizeof *tags;
	int i;

	if (*depthPtr == 4) {
		if (!strcmp("key", dataPtr->tag)) {
		
			i = 0; 	dataPtr->key = -1;

			while ((i < NumTags) && (0 != strcmp(tags[i], dataPtr->content) )) { i++;}
			
			if (i < NumTags) {
				if  (!strcmp(tags[i], dataPtr->content))  dataPtr->key = tag_assigns[i];
			}

			
		}
		else if (!strcmp(STR_STR ,dataPtr->tag)) {

			switch (dataPtr->key) {
				case INPUT_LABEL: 		strncpy(dataPtr->build.Input_label, dataPtr->content, DEFAULT_NAME_SIZE);		 		break;
				case INPUT_CHNL_NO: 	sscanf(dataPtr->content,"%d", &dataPtr->build.Input_channel_no); 						break;
				case INPUT_NAME: 		strncpy(dataPtr->build.Input_out_name, dataPtr->content, DEFAULT_NAME_SIZE);		 	break;
				
				case RND_LABEL: 		strncpy(dataPtr->build.Noise_mod_label, dataPtr->content, DEFAULT_NAME_SIZE);		 	break;
				case RND_CHNL_NO: 		sscanf(dataPtr->content,"%d", &dataPtr->build.Noise_channel_size); 						break;
				case RND_NAME: 			strncpy(dataPtr->build.Noise_out_name, dataPtr->content, DEFAULT_NAME_SIZE);		 	break;

				case POT_LABEL: 		strncpy(dataPtr->build.Pot_label, dataPtr->content, DEFAULT_NAME_SIZE);		 			break;
				case POT_CHNL_NO: 		sscanf(dataPtr->content,"%d", &dataPtr->build.Pot_channel_no); 							break;
				case POT_OUTPUTX: 		strncpy(dataPtr->build.Pot_out_namex, dataPtr->content, DEFAULT_NAME_SIZE);		 		break;
				case POT_OUTPUTY: 		strncpy(dataPtr->build.Pot_out_namey, dataPtr->content, DEFAULT_NAME_SIZE);		 		break;
				case POT_OUTPUTZ: 		strncpy(dataPtr->build.Pot_out_namez, dataPtr->content, DEFAULT_NAME_SIZE);		 		break;
				case POT_SELECTX: 		strncpy(dataPtr->build.Pot_sel_namex, dataPtr->content, DEFAULT_NAME_SIZE);		 		break;
				case POT_SELECTY: 		strncpy(dataPtr->build.Pot_sel_namey, dataPtr->content, DEFAULT_NAME_SIZE);		 		break;
				case POT_SELECTZ: 		strncpy(dataPtr->build.Pot_sel_namez, dataPtr->content, DEFAULT_NAME_SIZE);		 		break;

				case OUTPUT_LABEL: 		strncpy(dataPtr->build.Output_label, dataPtr->content, DEFAULT_NAME_SIZE);		 		break;
				case OUTPUT_CHNL_NO: 	sscanf(dataPtr->content,"%d", &dataPtr->build.Output_channel_no); 						break;
				case OUTPUT_NAME: 		strncpy(dataPtr->build.Output_in_name, dataPtr->content, DEFAULT_NAME_SIZE);		 	break;

				case CNXN_LABEL: 		strncpy(dataPtr->build.Connection_label, dataPtr->content, DEFAULT_NAME_SIZE);		 	break;
				case CNXN_BUILD_SIZE: 	sscanf(dataPtr->content,"%d", &dataPtr->build.Connection_build_size); 					break;
				case CNXN_LUG: 			strncpy(dataPtr->build.Connection_lug_name, dataPtr->content, DEFAULT_NAME_SIZE);		break;
			
				case SCP_CHAN_NO: 		sscanf(dataPtr->content,"%d", &dataPtr->build.Trace_build_size); 						break;
				case SCP_LABEL: 		strncpy(dataPtr->build.Trace_mod_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;	
				case SCP_XINPUT: 		strncpy(dataPtr->build.Trace_xin_name, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case SCP_YINPUT: 		strncpy(dataPtr->build.Trace_yin_name, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case SCP_ZINPUT: 		strncpy(dataPtr->build.Trace_zin_name, dataPtr->content, DEFAULT_NAME_SIZE);			break;

				case SCP_ORIENTA:		strncpy(dataPtr->build.Scope_orienta, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case SCP_ORIENTB:		strncpy(dataPtr->build.Scope_orientb, dataPtr->content, DEFAULT_NAME_SIZE);				break;

				case SYSTEM_PAUSE:		strncpy(dataPtr->build.System_pause, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case SYSTEM_STOP:		strncpy(dataPtr->build.System_stop, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case SYSTEM_RESET:		strncpy(dataPtr->build.System_reset, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case SYSTEM_RATE:		sscanf(dataPtr->content,"%d", &dataPtr->build.System_Sample_Rate);						break;

				case DLY_BUILD_SIZE: 	sscanf(dataPtr->content,"%d", &dataPtr->build.Delay_build_size); 						break;
				case DLY_MOD_LABEL: 	strncpy(dataPtr->build.Delay_mod_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;	
				case DLY_INPUT: 		strncpy(dataPtr->build.Delay_in_name, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case DLY_IN_SH: 		strncpy(dataPtr->build.Delay_in_sethold_name, dataPtr->content, DEFAULT_NAME_SIZE);		break;
				case DLY_OUTPUT: 		strncpy(dataPtr->build.Delay_out_name, dataPtr->content, DEFAULT_NAME_SIZE);			break;

				case ID_BUILD_SIZE: 	sscanf(dataPtr->content,"%d", &dataPtr->build.ID_build_size); 							break;
				case ID_MOD_LABEL: 		strncpy(dataPtr->build.ID_mod_label, dataPtr->content, DEFAULT_NAME_SIZE);				break;	
				case ID_OUT_A: 			strncpy(dataPtr->build.ID_out_a, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case ID_OUT_B: 			strncpy(dataPtr->build.ID_out_b, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case ID_OUT_C: 			strncpy(dataPtr->build.ID_out_c, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case ID_OUT_D: 			strncpy(dataPtr->build.ID_out_d, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case ID_OUT_E: 			strncpy(dataPtr->build.ID_out_e, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case ID_OUT_F: 			strncpy(dataPtr->build.ID_out_f, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case ID_IN_I: 			strncpy(dataPtr->build.ID_in_i, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case ID_IN_J: 			strncpy(dataPtr->build.ID_in_j, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case ID_IN_SH: 			strncpy(dataPtr->build.ID_in_sethold, dataPtr->content, DEFAULT_NAME_SIZE);				break;

				case ENT_BUILD_SIZE:	sscanf(dataPtr->content,"%d", &dataPtr->build.Ent_build_size); 							break;
				case ENT_MOD_LABEL: 	strncpy(dataPtr->build.Ent_mod_label, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case ENT_OUT_DELAYED:	strncpy(dataPtr->build.Ent_delayed, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case ENT_OUT_ENTROPY:	strncpy(dataPtr->build.Ent_out, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case ENT_INPUT:			strncpy(dataPtr->build.Ent_in, dataPtr->content, DEFAULT_NAME_SIZE);					break;

				case COMP_BUILD_SIZE:	sscanf(dataPtr->content,"%d", &dataPtr->build.Comp_build_size); 						break;
				case COMP_MOD_LABEL:	strncpy(dataPtr->build.Comp_mod_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case COMP_DELAYED:		strncpy(dataPtr->build.Comp_delayed, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case COMP_POS:			strncpy(dataPtr->build.Comp_out, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case COMP_ZERO:			strncpy(dataPtr->build.Comp_zero, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case COMP_NONPOS:		strncpy(dataPtr->build.Comp_nonpos, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case COMP_NEG:			strncpy(dataPtr->build.Comp_neg, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case COMP_NONNEG:		strncpy(dataPtr->build.Comp_nonneg, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case COMP_INPUT:		strncpy(dataPtr->build.Comp_in, dataPtr->content, DEFAULT_NAME_SIZE);					break;

				case FFT_BUILD_SIZE: 	sscanf(dataPtr->content,"%d", &dataPtr->build.FFT_build_size); 							break;
				case FFT_MOD_LABEL: 	strncpy(dataPtr->build.FFT_mod_label, dataPtr->content, DEFAULT_NAME_SIZE);				break;	
				case FFT_INPUTX: 		strncpy(dataPtr->build.FFT_inx, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case FFT_INPUTY: 		strncpy(dataPtr->build.FFT_iny, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case FFT_SYNC:	 		strncpy(dataPtr->build.FFT_sync, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case FFT_REALOUT: 		strncpy(dataPtr->build.FFT_realout, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case FFT_IMAGOUT: 		strncpy(dataPtr->build.FFT_imagout, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case FFT_DELAYEDX: 		strncpy(dataPtr->build.FFT_delayedx, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case FFT_DELAYEDY: 		strncpy(dataPtr->build.FFT_delayedy, dataPtr->content, DEFAULT_NAME_SIZE);				break;

				case FIR_BUILD_SIZE:	sscanf(dataPtr->content,"%d", &dataPtr->build.Filter_build_size);						break;
				case FIR_MOD_LABEL: 	strncpy(dataPtr->build.Filter_mod_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case FIR_OUT_DELAYED:	strncpy(dataPtr->build.Filter_delayed, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case FIR_OUT:			strncpy(dataPtr->build.Filter_out, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case FIR_INPUT:			strncpy(dataPtr->build.Filter_in, dataPtr->content, DEFAULT_NAME_SIZE);					break;

				case PLR_BUILD_SIZE:	sscanf(dataPtr->content,"%d", &dataPtr->build.Plr_build_size); 							break;
				case PLR_MOD_LABEL: 	strncpy(dataPtr->build.Plr_mod_label, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case PLR_XINPUT:		strncpy(dataPtr->build.Plr_xin, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case PLR_YINPUT:		strncpy(dataPtr->build.Plr_yin, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case PLR_ZINPUT:		strncpy(dataPtr->build.Plr_zin, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case PLR_THETA:			strncpy(dataPtr->build.Plr_theta, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case PLR_PHI:			strncpy(dataPtr->build.Plr_phi, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case PLR_NORM:			strncpy(dataPtr->build.Plr_norm, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case PLR_XDELAY:		strncpy(dataPtr->build.Plr_xdelayed, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case PLR_YDELAY:		strncpy(dataPtr->build.Plr_ydelayed, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case PLR_ZDELAY:		strncpy(dataPtr->build.Plr_zdelayed, dataPtr->content, DEFAULT_NAME_SIZE);				break;

				case AUDIO_BUILD_SIZE:	sscanf(dataPtr->content,"%d", &dataPtr->build.Audio_build_size); 						break;
				case AUDIO_MOD_LABEL: 	strncpy(dataPtr->build.Audio_mod_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case AUDIO_INPUTL:		strncpy(dataPtr->build.Audio_leftin, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case AUDIO_INPUTR:		strncpy(dataPtr->build.Audio_rightin, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case AUDIO_OUT_OPT:		strncpy(dataPtr->build.Audio_out_optn, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case AUDIO_OUTPUTL:		strncpy(dataPtr->build.Audio_leftout, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case AUDIO_OUTPUTR:		strncpy(dataPtr->build.Audio_rightout, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case AUDIO_OUTPUTA:		strncpy(dataPtr->build.Audio_aout, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case AUDIO_OUTPUTB:		strncpy(dataPtr->build.Audio_bout, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case AUDIO_OUTPUTC:		strncpy(dataPtr->build.Audio_cout, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case AUDIO_OUTPUTD:		strncpy(dataPtr->build.Audio_dout, dataPtr->content, DEFAULT_NAME_SIZE);				break;

				case MATH_BUILD_SIZE:	sscanf(dataPtr->content,"%d", &dataPtr->build.Math_build_size); 						break;
				case MATH_MOD_LABEL:	strncpy(dataPtr->build.Math_mod_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case MATH_DELAYED:		strncpy(dataPtr->build.Math_delayed, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case MATH_SQRT:			strncpy(dataPtr->build.Math_sqrt, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case MATH_LOG:			strncpy(dataPtr->build.Math_log, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case MATH_EXP:			strncpy(dataPtr->build.Math_exp, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case MATH_SIN:			strncpy(dataPtr->build.Math_sin, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case MATH_RECIP:		strncpy(dataPtr->build.Math_recip, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case MATH_INPUT:		strncpy(dataPtr->build.Math_in, dataPtr->content, DEFAULT_NAME_SIZE);					break;				

				case EXTRN_BUILD_SIZE:	sscanf(dataPtr->content,"%d", &dataPtr->build.Extern_build_size); 						break;
				case EXTRN_MOD_LABEL: 	strncpy(dataPtr->build.Extern_mod_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case EXTRN_ARGX:		strncpy(dataPtr->build.Extern_argx_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;				
				case EXTRN_ARGY:		strncpy(dataPtr->build.Extern_argy_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;				
				case EXTRN_PUSHZ:		strncpy(dataPtr->build.Extern_pushz_label, dataPtr->content, DEFAULT_NAME_SIZE);		break;				
				case EXTRN_ARG1:		strncpy(dataPtr->build.Extern_arg1_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case EXTRN_ARG2:		strncpy(dataPtr->build.Extern_arg2_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case EXTRN_ARG3:		strncpy(dataPtr->build.Extern_arg3_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case EXTRN_ARG4:		strncpy(dataPtr->build.Extern_arg4_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case EXTRN_ARG5:		strncpy(dataPtr->build.Extern_arg5_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				case EXTRN_ARG6:		strncpy(dataPtr->build.Extern_arg6_label, dataPtr->content, DEFAULT_NAME_SIZE);			break;
				

				case EXT_BILD:			strncpy(dataPtr->build.ext_build, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case EXT_INOT:			strncpy(dataPtr->build.ext_ionms, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case EXT_PREST:			strncpy(dataPtr->build.ext_prests, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case EXT_INIT:			strncpy(dataPtr->build.ext_init, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case EXT_CIRC:			strncpy(dataPtr->build.ext_circ, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				case EXT_FILT:			strncpy(dataPtr->build.ext_filter, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case EXT_DISP:			strncpy(dataPtr->build.ext_display, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case EXT_VIEW:			strncpy(dataPtr->build.ext_views, dataPtr->content, DEFAULT_NAME_SIZE);					break;
				
				case ORIGIN_X:		if ( 1 != sscanf(dataPtr->content, "%d", &winPos_x))	dataPtr->status = TRUE ;			break;
				case ORIGIN_Y:		if ( 1 != sscanf(dataPtr->content, "%d", &winPos_y))	dataPtr->status = TRUE ;			break;
				case DIMENSION_X:	if ( 1 != sscanf(dataPtr->content, "%d", &winSize_x))	dataPtr->status = TRUE ;			break;
				case DIMENSION_Y:	if ( 1 != sscanf(dataPtr->content, "%d", &winSize_y))	dataPtr->status = TRUE ;			break;

				case TITLE:			strncpy(title, dataPtr->content, DEFAULT_NAME_SIZE);	newtitle = TRUE;					break;							


				case G4:	if ( 1 != sscanf(dataPtr->content, "%f", &xlimit)) 		dataPtr->status = TRUE ;					break;
				case G5:	if ( 1 != sscanf(dataPtr->content, "%f", &ylimit))		dataPtr->status = TRUE ;					break;
				case G6:	if ( 1 != sscanf(dataPtr->content, "%f", &zlimit))		dataPtr->status = TRUE ;					break;
				case G7:	if ( 1 != sscanf(dataPtr->content, "%f", &xorig))		dataPtr->status = TRUE ;					break;
				case G8:	if ( 1 != sscanf(dataPtr->content, "%f", &yorig))		dataPtr->status = TRUE ;					break;
				case G9:	if ( 1 != sscanf(dataPtr->content, "%f", &zorig))		dataPtr->status = TRUE ;					break;
				case G10:		sscanf(dataPtr->content, "%f", &grid_r);			break;
				case G11:		sscanf(dataPtr->content, "%f", &grid_g);			break;
				case G12:		sscanf(dataPtr->content, "%f", &grid_b);			break;

				case G13:		sscanf(dataPtr->content, "%f", &xlinlog);			break;
				case G14:		sscanf(dataPtr->content, "%f", &ylinlog);			break;
				case G15:		sscanf(dataPtr->content, "%f", &zlinlog);			break;
				
				case L1:	if ( 1 != sscanf(dataPtr->content, "%f", &spinxlight))	dataPtr->status = TRUE ;					break;
				case L2:	if ( 1 != sscanf(dataPtr->content, "%f", &spinylight))	dataPtr->status = TRUE ;					break;
				case L3:	if ( 1 != sscanf(dataPtr->content, "%f", &spinxlight2))	dataPtr->status = TRUE ;					break;
				case L4:	if ( 1 != sscanf(dataPtr->content, "%f", &spinylight2))	dataPtr->status = TRUE ;					break;
				case L5:	if ( 1 != sscanf(dataPtr->content, "%f", &spinxlight3))	dataPtr->status = TRUE ;					break;
				case L6:	if ( 1 != sscanf(dataPtr->content, "%f", &spinylight3))	dataPtr->status = TRUE ;					break;
				
				case O1:	if ( 1 != sscanf(dataPtr->content, "%f", &spinxsurface)) dataPtr->status = TRUE ;					break;
				case O2:	if ( 1 != sscanf(dataPtr->content, "%f", &spinysurface)) dataPtr->status = TRUE ;					break;

// complete these later
//				case SURFACE_RED:	 if ( 1 != sscanf(dataPtr->content, "%f", &surface_red)) dataPtr->status = TRUE ;					break;
//				case SURFACE_GREEN:	 if ( 1 != sscanf(dataPtr->content, "%f", &surface_green)) dataPtr->status = TRUE ;					break;
//				case SURFACE_BLUE:	 if ( 1 != sscanf(dataPtr->content, "%f", &surface_blue)) dataPtr->status = TRUE ;					break;
// using the intesity for setting brightness of the balls for example
				case SURFACE_INTSTY: if ( 1 != sscanf(dataPtr->content, "%f", &surface_intsty)) dataPtr->status = TRUE ;					break;
				case SURFACE_SHADOW: if ( 1 != sscanf(dataPtr->content, "%f", &surface_shadow)) dataPtr->status = TRUE ;					break;

				default:
					break;			
			}
		}
		else if (!strcmp(STR_REAL,dataPtr->tag)){
			switch (dataPtr->key) {
				case C1: 		sscanf(dataPtr->content, "%f", &Glowr);				break;
				case C2:		sscanf(dataPtr->content, "%f", &Glowg);				break;
				case C3:		sscanf(dataPtr->content, "%f", &Glowb);				break;
				case C4:		sscanf(dataPtr->content, "%f", &ScaleIntensity);	break;
				case C5:		sscanf(dataPtr->content, "%f", &background_r);		break;
				case C6:		sscanf(dataPtr->content, "%f", &background_g);		break;
				case C7:		sscanf(dataPtr->content, "%f", &background_b);		break;
//				case C8:		sscanf(dataPtr->content, "%f", &complement);		break;

				case G1:		sscanf(dataPtr->content, "%f", &xaxis);				break;
				case G2:		sscanf(dataPtr->content, "%f", &yaxis);				break;
				case G3:		sscanf(dataPtr->content, "%f", &zaxis);				break;
				case G4:	if ( 1 != sscanf(dataPtr->content, "%f", &xlimit)) 		dataPtr->status = TRUE ;					break;
				case G5:	if ( 1 != sscanf(dataPtr->content, "%f", &ylimit))		dataPtr->status = TRUE ;					break;
				case G6:	if ( 1 != sscanf(dataPtr->content, "%f", &zlimit))		dataPtr->status = TRUE ;					break;

				case G7:		sscanf(dataPtr->content, "%f", &xorig);				break;
				case G8:		sscanf(dataPtr->content, "%f", &yorig);				break;
				case G9:		sscanf(dataPtr->content, "%f", &zorig);				break;
				case G10:		sscanf(dataPtr->content, "%f", &grid_r);			break;
				case G11:		sscanf(dataPtr->content, "%f", &grid_g);			break;
				case G12:		sscanf(dataPtr->content, "%f", &grid_b);			break;

				case G13:		sscanf(dataPtr->content, "%f", &xlinlog);			break;
				case G14:		sscanf(dataPtr->content, "%f", &ylinlog);			break;
				case G15:		sscanf(dataPtr->content, "%f", &zlinlog);			break;


				case O1:		sscanf(dataPtr->content, "%f", &spinxsurface);		break;
				case O2:		sscanf(dataPtr->content, "%f", &spinysurface);		break;

				case L1:		sscanf(dataPtr->content, "%f", &spinxlight);		break;
				case L2:		sscanf(dataPtr->content, "%f", &spinylight);		break;
				case L3:		sscanf(dataPtr->content, "%f", &spinxlight2);		break;
				case L4:		sscanf(dataPtr->content, "%f", &spinylight2);		break;
				case L5:		sscanf(dataPtr->content, "%f", &spinxlight3);		break;
				case L6:		sscanf(dataPtr->content, "%f", &spinylight3);		break;

// complete these later
//				case SURFACE_RED:	 if ( 1 != sscanf(dataPtr->content, "%f", &surface_red)) dataPtr->status = TRUE ;					break;
//				case SURFACE_GREEN:	 if ( 1 != sscanf(dataPtr->content, "%f", &surface_green)) dataPtr->status = TRUE ;					break;
//				case SURFACE_BLUE:	 if ( 1 != sscanf(dataPtr->content, "%f", &surface_blue)) dataPtr->status = TRUE ;					break;
// using the intesity for setting brightness of the balls for example
				case SURFACE_INTSTY: if ( 1 != sscanf(dataPtr->content, "%f", &surface_intsty)) dataPtr->status = TRUE ;					break;
				case SURFACE_SHADOW: if ( 1 != sscanf(dataPtr->content, "%f", &surface_shadow)) dataPtr->status = TRUE ;					break;

				default:   break;
			}
			dataPtr->key = -1;
		}
		else if (!strcmp("true",dataPtr->tag)){
			switch (dataPtr->key) {
				case INPUT_CHNL_NO: 	dataPtr->build.Input_channel_no = 1; 		break;
				case POT_CHNL_NO: 		dataPtr->build.Pot_channel_no = 1; 			break;
				case RND_CHNL_NO: 		dataPtr->build.Noise_channel_size = 1; 		break;
				case OUTPUT_CHNL_NO: 	dataPtr->build.Output_channel_no = 1; 		break;
				case CNXN_BUILD_SIZE: 	dataPtr->build.Connection_build_size = 1; 	break;
				case SCP_CHAN_NO: 		dataPtr->build.Trace_build_size = 1; 		break;
				case DLY_BUILD_SIZE: 	dataPtr->build.Delay_build_size = 1; 		break;
				case ID_BUILD_SIZE: 	dataPtr->build.ID_build_size = 1; 			break;
				case ENT_BUILD_SIZE:	dataPtr->build.Ent_build_size = 1; 			break;
				case COMP_BUILD_SIZE:	dataPtr->build.Comp_build_size = 1; 		break;
				case FFT_BUILD_SIZE: 	dataPtr->build.FFT_build_size = 1; 			break;
				case FIR_BUILD_SIZE:	dataPtr->build.Filter_build_size = 1;		break;
				case PLR_BUILD_SIZE:	dataPtr->build.Plr_build_size = 1; 			break;
				case AUDIO_BUILD_SIZE:	dataPtr->build.Audio_build_size = 1; 		break;
				case MATH_BUILD_SIZE:	dataPtr->build.Math_build_size = 1; 		break;
				case EXTRN_BUILD_SIZE:	dataPtr->build.Extern_build_size = 1; 		break;


				case G_Origin:
					center_axis_flag = TRUE; break;
				case G_Labels:
			 		axis_markers_flag = TRUE; break;

				case LOCK_AXIS:
					lock_axis_flag = TRUE; break;
				case LOCK_AXIS_LIGHT:
					lock_axis_light_flag = TRUE; break;
				case LOCK_SFCE_LIGHT:
					lock_surface_light_flag = TRUE; break;
				case LOCK_MKRS_LIGHT:
					lock_markers_light_flag = TRUE;
			 	default: break;
			}
		}
		else if (!strcmp("false",dataPtr->tag)){
			switch (dataPtr->key) {
				case INPUT_CHNL_NO: 	dataPtr->build.Input_channel_no = 0; 		break;
				case POT_CHNL_NO: 		dataPtr->build.Pot_channel_no = 0; 			break;
				case RND_CHNL_NO: 		dataPtr->build.Noise_channel_size = 0; 		break;
				case OUTPUT_CHNL_NO: 	dataPtr->build.Output_channel_no = 0; 		break;
				case CNXN_BUILD_SIZE: 	dataPtr->build.Connection_build_size = 0; 	break;
				case SCP_CHAN_NO: 		dataPtr->build.Trace_build_size = 0; 		break;
				case DLY_BUILD_SIZE: 	dataPtr->build.Delay_build_size = 0; 		break;
				case ID_BUILD_SIZE: 	dataPtr->build.ID_build_size = 0; 			break;
				case ENT_BUILD_SIZE:	dataPtr->build.Ent_build_size = 0; 			break;
				case COMP_BUILD_SIZE:	dataPtr->build.Comp_build_size = 0; 		break;
				case FFT_BUILD_SIZE: 	dataPtr->build.FFT_build_size = 0; 			break;
				case FIR_BUILD_SIZE:	dataPtr->build.Filter_build_size = 0;		break;
				case PLR_BUILD_SIZE:	dataPtr->build.Plr_build_size = 0; 			break;
				case AUDIO_BUILD_SIZE:	dataPtr->build.Audio_build_size = 0; 		break;
				case MATH_BUILD_SIZE:	dataPtr->build.Math_build_size = 0; 		break;
				case EXTRN_BUILD_SIZE:	dataPtr->build.Extern_build_size = 0; 		break;

				case G_Origin:
					center_axis_flag = FALSE; break;
				case G_Labels:
			 		axis_markers_flag = FALSE; break;

				case LOCK_AXIS:
					lock_axis_flag = FALSE; break;
				case LOCK_AXIS_LIGHT:
					lock_axis_light_flag = FALSE; break;
				case LOCK_SFCE_LIGHT:
					lock_surface_light_flag = FALSE; break;
				case LOCK_MKRS_LIGHT:
					lock_markers_light_flag = FALSE; 
			 	default: break;
			}
		}
/* debug */
		else if ( (*depthPtr == 3) && (!strcmp("dict",dataPtr->tag))) {
				setview((int) spinxsurface, (int) spinysurface);

				setlight_obj((int) spinxlight, (int) spinylight);
				setlight_2((int) spinxlight2, (int) spinylight2);
				setlight_3((int) spinxlight3, (int) spinylight3);

				if (Glowr > MAX_CHROM) Glowr  = MAX_CHROM ;
				else if (Glowr < MIN_CHROM) Glowr = MIN_CHROM ;

				if (Glowg > MAX_CHROM) Glowg  = MAX_CHROM ;
				else if (Glowg < MIN_CHROM) Glowg = MIN_CHROM ;

				if (Glowb > MAX_CHROM) Glowb = MAX_CHROM ;
				else if (Glowb  < MIN_CHROM) Glowb  = MIN_CHROM ;
				
				if (ScaleIntensity > MAX_LUM) ScaleIntensity  = MAX_LUM ;
				else if (ScaleIntensity < MIN_LUM) ScaleIntensity = MIN_LUM ;
				
				if (background_r > MAX_CHROM) background_r  = MAX_CHROM ;
				else if (background_r < MIN_CHROM) background_r = MIN_CHROM ;
				
				if (background_g > MAX_CHROM) background_g  = MAX_CHROM ;
				else if (background_g < MIN_CHROM) background_g = MIN_CHROM ;
				
				if  (background_b > MAX_CHROM) background_b  = MAX_CHROM ;
				else if (background_b < MIN_CHROM) background_b = MIN_CHROM ;
//				xaxis = yaxis = zaxis = DEF_LUM ; 						// defaults brightness
//				xlimit = DEF_X ; ylimit = DEF_Y ; zlimit = DEF_Z ;		// defaults 
//				xorig = yorig = zorig = ORIGIN ; 						// defaults 

				if (xaxis > MAX_LUM) xaxis  = MAX_LUM ;
					else if (xaxis < MIN_CHROM ) xaxis = 0.0 ;

				if (yaxis > MAX_LUM) yaxis  = MAX_LUM ;
					else if (yaxis < MIN_CHROM ) yaxis = MIN_CHROM ;

				if (zaxis > MAX_LUM) zaxis  = MAX_LUM ;
					else if (zaxis < MIN_CHROM ) zaxis = MIN_CHROM ;

				if (xlimit > MAX_X ) xlimit  = MAX_X ;
					else if (xlimit < ORIGIN ) xlimit = NORMALE ;

				if (ylimit > MAX_Y ) ylimit  = MAX_Y ;
					else if (ylimit < ORIGIN ) ylimit = NORMALE ;

				if (zlimit > MAX_Z ) zlimit  = MAX_Z ;
					else if (zlimit < ORIGIN) zlimit = NORMALE ;

				if (xorig > MAX_X ) xorig  = MAX_X ;
					else if (xorig < -MAX_X) xorig = -MAX_X ;

				if (yorig > MAX_Y) yorig  = MAX_Y ;
					else if (yorig < -MAX_Y) yorig = -MAX_Y ;

				if (zorig > MAX_Z) zorig  = MAX_Z ;
					else if (zorig < -MAX_Z) zorig = -MAX_Z ;

				if (grid_r > MAX_CHROM) grid_r  = MAX_CHROM ;
					else if (grid_r < MIN_CHROM) grid_r = MIN_CHROM ;

				if (grid_g > MAX_CHROM) grid_g  = MAX_CHROM ;
					else if (grid_g < MIN_CHROM) grid_g = MIN_CHROM;

				if (grid_b > MAX_CHROM) grid_b  = MAX_CHROM ;
					else if (grid_b < MIN_CHROM) grid_b = MIN_CHROM ;

				// other spod things to set for now
					simultrace = FALSE;
					flat = TRUE;
					Showlabels = FALSE;
					Tracelight = 10;
					
					start_scan = -1.0;
					scan_incr = 0.0;
					width_scan = 1.8;
					trace = 0.0;				// tumble
					tracex = 0.0;				// role
//				// visible/invisible object selection
//				int i;
//				if (dataPtr->view.obj_enable == TRUE)
//  {
//					for (i=0; i < 10; i++) {
//						if (surfacePtr[i]) surfacePtr[i]->visible = dataPtr->view.surf_obj[i];
//						if (curvePtr[i]) curvePtr[i]->visible = dataPtr->view.graph_obj[i];
//					}
//				}
		}
	} 
	*depthPtr -= 1;
}

//****************************************************************************************//
//****************************************************************************************//
//****************************************************************************************//
//****************************************************************************************//
//****************************************************************************************//


void plist_dict_pair(FILE *fd, char * keyPtr, char *valuePtr, char *type) {
	fprintf(fd, "\t\t<key>%s</key>\n\t\t<%s>%s</%s>\n", keyPtr, type, valuePtr, type);
}
	


/********************************************************************************************************************/
/********** build *******/
int build( struct xml_data *DataPtr ) { 

	char Filename[FILE_NAME_SIZE];	
	char buf[BUFSIZ];
	char temp_string[100];
	
	XML_Parser parser;
	int done;
	FILE *fd; 			
 
/* lets get the build */
	if (DataPtr->config_name == NULL) 
		fd = stdin;
	else {
		sprintf(Filename,"%s.%s.pertMd", DataPtr->config_name, DataPtr->build.ext_build);
		fd = fopen (&Filename[0],"r+");
/*		if (!fd) { // otherwise try in  search in the PERTECSsripts folder?
			sprintf(Filename,"/Users/mark/PERTScripts/%s/%s.%s.pertMd", DataPtr->config_name, DataPtr->config_name, DataPtr->build.ext_build);
			fd = fopen (&Filename[0],"a");
		}*/
		 if ((!fd) && (DataPtr->buildflg == TRUE)) {
			fprintf ( stderr, "Creating build file: %s\n", Filename);
			fd = fopen (&Filename[0],"w+");
			if (fd) {
	// Extension_Filter to be changed to Extension_Pots
				print_pheader(fd);
				fprintf(fd, "<dict>\n");
				plist_dict_pair(fd, TAG_COMMENT, 		"Enter brief user notes here...", 						STR_STR );
				plist_dict_pair(fd, TAG_COMP_INPUT, 	"input",												STR_STR );
				plist_dict_pair(fd, TAG_COMP_MOD_LABEL, "COMPARATOR",											STR_STR );
				
				
				sprintf( temp_string, "%d", DataPtr->build.Comp_build_size);
				plist_dict_pair(fd, TAG_COMP_BUILD_SIZE,	temp_string,										STR_STR ); 		//Comparator_build_size
				
				plist_dict_pair(fd, TAG_COMP_DELAYED,		"delay",											STR_STR );		//Comparator_delayed
				plist_dict_pair(fd, TAG_COMP_NEG,			"neg",												STR_STR );		//Comparator_neg
				plist_dict_pair(fd, TAG_COMP_NONNEG,		"non-neg",											STR_STR ); 		//Comparator_nonneg
				plist_dict_pair(fd, TAG_COMP_NONPOS,		"non-pos",											STR_STR ); 		//Comparator_nonpos
				plist_dict_pair(fd, TAG_COMP_POS,			"pos",												STR_STR ); 		//Comparator_out
				plist_dict_pair(fd, TAG_COMP_ZERO,			"zero",												STR_STR );		//Comparator_zero

				sprintf( temp_string, "%d", DataPtr->build.Delay_build_size);
				plist_dict_pair(fd, TAG_DLY_BUILD_SIZE,		temp_string,										STR_STR ); 		//Delay_build_size
				
				plist_dict_pair(fd, TAG_DLY_INPUT,			"input",											STR_STR ); 		//Delay_inputname
				plist_dict_pair(fd, TAG_DLY_MOD_LABEL,		"DELAY",			  								STR_STR );		//Delay_mod_label
				plist_dict_pair(fd, TAG_DLY_OUTPUT,			"delay",											STR_STR ); 		//Delay_outputname
				plist_dict_pair(fd, TAG_DLY_IN_SH,			"set-hold",		 									STR_STR ); 		//Delay_reset_hold
				
				sprintf( temp_string, "%d", DataPtr->build.Extern_build_size);
				plist_dict_pair(fd, TAG_EXTRN_BUILD_SIZE,	temp_string, 										STR_STR ); 		//EXT_build_size
				plist_dict_pair(fd, TAG_EXTRN_MOD_LABEL,	"EXTERN",											STR_STR ); 		//EXT_mod_label
				plist_dict_pair(fd, TAG_EXTRN_ARG1,			"arg-1",											STR_STR ); 		//arg-1
				plist_dict_pair(fd, TAG_EXTRN_ARG2,			"arg-2",											STR_STR ); 		//arg-2
				plist_dict_pair(fd, TAG_EXTRN_ARG3,			"arg-3",											STR_STR ); 		//arg-3
				plist_dict_pair(fd, TAG_EXTRN_ARG4,			"arg-4",											STR_STR ); 		//arg-4
				plist_dict_pair(fd, TAG_EXTRN_ARG5,			"arg-5",											STR_STR ); 		//arg-5
				plist_dict_pair(fd, TAG_EXTRN_ARG6,			"arg-6",											STR_STR ); 		//arg-6
				plist_dict_pair(fd, TAG_EXTRN_ARGX,			"arg-x",											STR_STR ); 		//arg-x
				plist_dict_pair(fd, TAG_EXTRN_ARGY,			"arg-y",											STR_STR ); 		//arg-y
				plist_dict_pair(fd, TAG_EXTRN_PUSHZ,		"push-z",											STR_STR ); 		//push-z
				
				plist_dict_pair(fd, TAG_EXT_BUILD,			"build",											STR_STR ); 		//Extension_Build
				plist_dict_pair(fd, TAG_EXT_CIRC,			"circuit",											STR_STR ); 		//Extension_Circuitry
				plist_dict_pair(fd, TAG_EXT_FILT,			"pots",												STR_STR ); 		//Extension_Filter
				plist_dict_pair(fd, TAG_EXT_INOT,			"io",												STR_STR ); 		//Extension_IO
				plist_dict_pair(fd, TAG_EXT_PREST,			"mods",												STR_STR ); 		//Extension_Presets
				plist_dict_pair(fd, TAG_EXT_DISP,			"disp",												STR_STR ); 		//Extension_Display
				plist_dict_pair(fd, TAG_EXT_VIEW,			"views",											STR_STR ); 		//Extension_Views
				
				sprintf( temp_string, "%d", DataPtr->build.Ent_build_size);
				plist_dict_pair(fd, TAG_ENT_BUILD_SIZE,		temp_string, 										STR_STR );		//Ent_build_size
				plist_dict_pair(fd, TAG_ENT_OUT_DELAYED,	"delay",											STR_STR ); 		//Ent_delayed
				plist_dict_pair(fd, TAG_ENT_INPUT,			"input",											STR_STR ); 		//Ent_in
				plist_dict_pair(fd, TAG_ENT_MOD_LABEL,		"ENTROPY",											STR_STR );		//Ent_mod_label
				plist_dict_pair(fd, TAG_ENT_OUT_ENTROPY,	"entropy",											STR_STR ); 		//Ent_out
				

				
				sprintf( temp_string, "%d", DataPtr->build.Filter_build_size);
				plist_dict_pair(fd, TAG_FIR_BUILD_SIZE,		temp_string, 										STR_STR );		//Filter_build_size
				plist_dict_pair(fd, TAG_FIR_MOD_LABEL,		"FILTER",											STR_STR ); 		//FIR_mod_label
				plist_dict_pair(fd, TAG_FIR_INPUT,			"input",											STR_STR ); 		//Fir_in
				plist_dict_pair(fd, TAG_FIR_OUT_DELAYED,	"delay",											STR_STR ); 		//filter_delayed
				plist_dict_pair(fd, TAG_FIR_OUT,			"filter",											STR_STR ); 		//filter_out
				
				sprintf( temp_string, "%d", DataPtr->build.ID_build_size);
				plist_dict_pair(fd, TAG_ID_BUILD_SIZE,		temp_string, 										STR_STR );		//ID_build_size
				plist_dict_pair(fd, TAG_ID_MOD_LABEL,		"INTDIFF",											STR_STR ); 		//ID_mod_label
				plist_dict_pair(fd, TAG_ID_IN_SH,			"set-hold",											STR_STR ); 		//ID_reset_hold
				plist_dict_pair(fd, TAG_ID_OUT_A,			"3rd",												STR_STR ); 		//a_out
				plist_dict_pair(fd, TAG_ID_OUT_B,			"2nd",												STR_STR );		//b_out
				plist_dict_pair(fd, TAG_ID_OUT_C,			"1st",												STR_STR ); 		//c_out
				plist_dict_pair(fd, TAG_ID_OUT_D,			"0th",												STR_STR );		//d_out
				plist_dict_pair(fd, TAG_ID_OUT_E,			"delay",											STR_STR ); 		//e_out
				plist_dict_pair(fd, TAG_ID_OUT_F,			"integ",											STR_STR ); 		//f_out
				plist_dict_pair(fd, TAG_ID_IN_I,			"input",											STR_STR ); 		//label_i
				plist_dict_pair(fd, TAG_ID_IN_J,			"special",											STR_STR );		//label_j
				
				sprintf( temp_string, "%d", DataPtr->build.Input_channel_no);
				plist_dict_pair(fd, TAG_INPT_CHAN_NO,		temp_string, 										STR_STR );		//Input_channel_no
				plist_dict_pair(fd, TAG_INPT_LABEL,			"INPUT",											STR_STR ); 		// Input_label
				plist_dict_pair(fd, TAG_INPT_NAME,			"in",												STR_STR ); 		//Input_outputname
				
				sprintf( temp_string, "%d", DataPtr->build.Math_build_size);
				plist_dict_pair(fd, TAG_MATH_BUILD_SIZE,	temp_string, 										STR_STR );		//Math_build_size
				plist_dict_pair(fd, TAG_MATH_DELAYED,		"delay", 											STR_STR ); 		//Math_delayed_out
				plist_dict_pair(fd, TAG_MATH_EXP,			"exp",												STR_STR );		//Math_exp_out
				plist_dict_pair(fd, TAG_MATH_INPUT,			"input",											STR_STR );		//Math_in
				plist_dict_pair(fd, TAG_MATH_LOG,			"log",												STR_STR );		//Math_log_out
				plist_dict_pair(fd, TAG_MATH_MOD_LABEL,		"MATH",												STR_STR );		//Math_mod_label
				plist_dict_pair(fd, TAG_MATH_RECIP,			"recip",											STR_STR );		//Math_recip_out
				plist_dict_pair(fd, TAG_MATH_SIN,			"sin",												STR_STR );		//Math_sin_out
				plist_dict_pair(fd, TAG_MATH_SQRT,			"sqrt",												STR_STR );		//Math_sqrt_out
				
				sprintf( temp_string, "%d", DataPtr->build.Output_channel_no);
				plist_dict_pair(fd, TAG_OUTPT_CHNL_NO,		temp_string, 										STR_STR );		//Output_channel_no
				plist_dict_pair(fd, TAG_OUTPT_NAME,			"out",												STR_STR ); 		//Output_inputname
				plist_dict_pair(fd, TAG_OUTPT_LABEL,		"OUTPUT",											STR_STR ); 		//Output_label
				
				sprintf( temp_string, "%d", DataPtr->build.Plr_build_size);
				plist_dict_pair(fd, TAG_PLR_BUILD_SIZE,		temp_string, 										STR_STR );		//Polar_build_size
				plist_dict_pair(fd, TAG_PLR_THETA,			"theta",											STR_STR ); 		//Polar_theta_outputname
				plist_dict_pair(fd, TAG_PLR_PHI,			"phi",												STR_STR ); 		//Polar_phi_outputname
				plist_dict_pair(fd, TAG_PLR_XDELAY,			"delay-x",											STR_STR ); 		//Polar_delayed_x
				plist_dict_pair(fd, TAG_PLR_YDELAY,			"delay-y",											STR_STR ); 		//Polar_delayed_y
				plist_dict_pair(fd, TAG_PLR_ZDELAY,			"delay-z",											STR_STR ); 		//Polar_delayed_z
				plist_dict_pair(fd, TAG_PLR_MOD_LABEL,		"POLAR",											STR_STR ); 		//Polar_mod_label
				plist_dict_pair(fd, TAG_PLR_NORM,			"norm",												STR_STR ); 		//Polar_norm_outputname
				plist_dict_pair(fd, TAG_PLR_XINPUT,			"input-x",											STR_STR ); 		//Polar_x_inputname
				plist_dict_pair(fd, TAG_PLR_YINPUT,			"input-y",											STR_STR ); 		//Polar_y_inputname
				plist_dict_pair(fd, TAG_PLR_ZINPUT,			"input-z",											STR_STR ); 		//Polar_z_inputname
				
				sprintf( temp_string, "%d", DataPtr->build.Pot_channel_no);
				plist_dict_pair(fd, TAG_POT_CHAN_NO,		temp_string, 										STR_STR );		//Pot_channel_no
				plist_dict_pair(fd, TAG_POT_LABEL,			"PRESET",											STR_STR ); 		//Pot_label
				plist_dict_pair(fd, TAG_POT_OUTX,			"in-x",												STR_STR ); 		//Pot_out_x
				plist_dict_pair(fd, TAG_POT_OUTY,			"in-y",												STR_STR ); 		//Pot_out_y
				plist_dict_pair(fd, TAG_POT_OUTZ,			"in-z",												STR_STR ); 		//Pot_out_z
				plist_dict_pair(fd, TAG_POT_SELX,			"select-x",											STR_STR ); 		//Pot_sel_x
				plist_dict_pair(fd, TAG_POT_SELY,			"select-y",											STR_STR ); 		//Pot_sel_y
				plist_dict_pair(fd, TAG_POT_SELZ,			"select-z",											STR_STR ); 		//Pot_sel_z
				
				sprintf( temp_string, "%d", DataPtr->build.Noise_channel_size);
				plist_dict_pair(fd, TAG_RND_BUILD_SIZE,		temp_string, 										STR_STR );		//Random_build_size
				plist_dict_pair(fd, TAG_RND_LABEL,			"NOISE",											STR_STR ); 		//Rand_mod_label
				plist_dict_pair(fd, TAG_RND_OUTPUT,			"noise",											STR_STR ); 		//Random_outputname
				
				sprintf( temp_string, "%d", DataPtr->System_Rate);
				plist_dict_pair(fd, TAG_RATE,				temp_string, 										STR_STR );		//Rate
				
				sprintf( temp_string, "%d", DataPtr->build.Trace_build_size);
				plist_dict_pair(fd, TAG_SCP_CHAN_NO,		temp_string, 										STR_STR );		//Scope_channel_no
				plist_dict_pair(fd, TAG_SCP_LABEL,			"TRACE",											STR_STR ); 		//Scope_mod_label
				plist_dict_pair(fd, TAG_SCP_ORIENTA,		"orient-a",											STR_STR ); 		//Scope_orient_a
				plist_dict_pair(fd, TAG_SCP_ORIENTB,		"orient-b",											STR_STR ); 		//Scope_orient_b
				plist_dict_pair(fd, TAG_SCP_XINPUT,			"out-x",											STR_STR ); 		//Scope_x_inputname
				plist_dict_pair(fd, TAG_SCP_YINPUT,			"out-y",											STR_STR ); 		//Scope_y_inputname
				plist_dict_pair(fd, TAG_SCP_ZINPUT,			"out-z",											STR_STR ); 		//Scope_z_inputname
				
				plist_dict_pair(fd, TAG_SYS_PAUSE,			"pause",											STR_STR ); 		//System_pause
				plist_dict_pair(fd, TAG_SYS_RESET,			"reset",											STR_STR ); 		//System_reset
				plist_dict_pair(fd, TAG_SYS_STOP,			"stop",												STR_STR ); 		//System_stop
				
				sprintf( temp_string, "%d", DataPtr->build.FFT_build_size);
				plist_dict_pair(fd, TAG_FFT_BUILD_SIZE,		temp_string, 										STR_STR );		//FFT_build_size
				plist_dict_pair(fd, TAG_FFT_MOD_LABEL,		"FFT",												STR_STR ); 		//FFT_mod_label
				plist_dict_pair(fd, TAG_FFT_INPUTX,			"input-x",											STR_STR ); 		//FFT_x_in
				plist_dict_pair(fd, TAG_FFT_INPUTY,			"input-y",											STR_STR ); 		//FFT_y_in
				plist_dict_pair(fd, TAG_FFT_SYNC,			"sync",												STR_STR ); 		//FFT_sync in/out
				plist_dict_pair(fd, TAG_FFT_REALOUT,		"real",												STR_STR ); 		//FFT_real_outputname
				plist_dict_pair(fd, TAG_FFT_IMAGOUT,		"imag",												STR_STR ); 		//FFT_complex_outputname
				plist_dict_pair(fd, TAG_FFT_DELAYEDX,		"delay-x",											STR_STR ); 		//FFT_delayed_x_out
				plist_dict_pair(fd, TAG_FFT_DELAYEDY,		"delay-y",											STR_STR ); 		//FFT_delayed_y_out
				
				sprintf( temp_string, "%f", DataPtr->build.background_r);
				plist_dict_pair(fd, STR_C5,					temp_string,										STR_REAL );				// red background C5

				sprintf( temp_string, "%f", DataPtr->build.background_g);
				plist_dict_pair(fd, STR_C6,					temp_string,										STR_REAL );						// green C6

				sprintf( temp_string, "%f", DataPtr->build.background_b);
				plist_dict_pair(fd, STR_C7,					temp_string,										STR_REAL ); 						// blue C7

				plist_dict_pair(fd, STR_G10,				"1.0",												STR_REAL );		//G10
				plist_dict_pair(fd, STR_G11,				"1.0",												STR_REAL ); 		//G11
				plist_dict_pair(fd, STR_G1,					"0.0",												STR_REAL );		//G1
				plist_dict_pair(fd, STR_G2,					"0.0",												STR_REAL );		//G2
				plist_dict_pair(fd, STR_G3,					"0.5",												STR_REAL );		//G3
				plist_dict_pair(fd, STR_G4,					"12",												STR_STR ); 		//G4
				plist_dict_pair(fd, STR_G5,					"6",												STR_STR ); 		//G5
				plist_dict_pair(fd, STR_G6,					"6",												STR_STR ); 		//G6
				plist_dict_pair(fd, STR_G7,					"0",												STR_STR );		//G7
				plist_dict_pair(fd, STR_G8,					"0",												STR_STR );		//G8
				plist_dict_pair(fd, STR_G9,					"0",												STR_STR );		//G9
				plist_dict_pair(fd, STR_O1,					"0",												STR_STR );		//O1
				plist_dict_pair(fd, STR_O2,					"0",												STR_STR );		//O2

				sprintf( temp_string, "%d", DataPtr->build.Origin_X);
				plist_dict_pair(fd, STR_ORIGIN_X,			temp_string, 										STR_STR );		//Origin_X

				sprintf( temp_string, "%d", DataPtr->build.Origin_Y);
				plist_dict_pair(fd, STR_ORIGIN_Y,			temp_string, 										STR_STR );		//Origin_Y

				plist_dict_pair(fd, STR_L5,					"0",												STR_STR );		//L5
				plist_dict_pair(fd, STR_L6,					"0",												STR_STR );		//L6	

				sprintf( temp_string, "%d", DataPtr->build.WIdth);
				plist_dict_pair(fd, STR_DIMENSION_X,		temp_string, 										STR_STR );		//Width_X

				sprintf( temp_string, "%d", DataPtr->build.Height);
				plist_dict_pair(fd, STR_DIMENSION_Y,		temp_string, 										STR_STR );		//Width_Y

				plist_dict_pair(fd, STR_TITLE,				"PERTECS Scope",									STR_STR ); 		//Title
				plist_dict_pair(fd, STR_G_Origin,			"",													STR_FALSE ); 			//Origin_Position
				plist_dict_pair(fd, STR_G_Labels,			"",													STR_FALSE );		//Axis_Labels
				
				plist_dict_pair(fd, TAG_AUDIO_BUILD_SIZE,	"",													STR_FALSE ); 		//Audio_build_size
				plist_dict_pair(fd, TAG_AUDIO_INL,			"in-left",											STR_STR ); 											//Audio_in_L
				plist_dict_pair(fd, TAG_AUDIO_INR,			"in-right",											STR_STR ); 											//Audio_in_R
				plist_dict_pair(fd, TAG_AUDIO_OUT_OPTN,		"out-optn",											STR_STR ); 											//Audio_out_optn
				plist_dict_pair(fd, TAG_AUDIO_MOD_LABEL,	"ANALOGUE",											STR_STR ); 											//Audio_mod_label
				plist_dict_pair(fd, TAG_AUDIO_OUTL,			"out-left",											STR_STR ); 											//Audio_out_L
				plist_dict_pair(fd, TAG_AUDIO_OUTR,			"out-right",										STR_STR ); 											//Audio_out_R
				plist_dict_pair(fd, TAG_AUDIO_OUTA,			"out-a",											STR_STR ); 											//Audio_out_A
				plist_dict_pair(fd, TAG_AUDIO_OUTB,			"out-b",											STR_STR ); 											//Audio_out_B
				plist_dict_pair(fd, TAG_AUDIO_OUTC,			"out-c",											STR_STR ); 											//Audio_out_C
				plist_dict_pair(fd, TAG_AUDIO_OUTD,			"out-d",											STR_STR ); 											//Audio_out_D
				
				sprintf( temp_string, "%d", DataPtr->build.Connection_build_size);
				plist_dict_pair(fd, TAG_CNXN_BUILD_SIZE,	temp_string, 										STR_STR );	//Connection_build_size
				plist_dict_pair(fd, TAG_CNXN_LUG,			"conx",												STR_STR ); 												//Connection_lug
				plist_dict_pair(fd, TAG_CNXN_LABEL,			"LUG",												STR_STR );											//Connection_mod_label
// lock switches
				plist_dict_pair(fd, TAG_AXIS_LIGHT_LOCK,	"",													STR_FALSE ); 										//Axis_Light_Lock
				plist_dict_pair(fd, TAG_AXIS_LOCK,			"",													STR_FALSE ); 											//Axis_Lock
				plist_dict_pair(fd, TAG_MKRS_LIGHT_LOCK,	"",													STR_FALSE ); 										//	Markers_Lock
				plist_dict_pair(fd, TAG_SFCE_LIGHT_LOCK,	"",													STR_FALSE );										// Surface_Lock

				fprintf(fd, "\t</dict>\n");
				print_pclose(fd);
				
				sprintf(Filename,"open %s.%s.pertMd", DataPtr->config_name, DataPtr->build.ext_build);
				system(Filename);
				 
			} else fprintf(stderr,"Problems creating build %s\n", Filename); fflush(stderr);
			return 1;
		}
		else if ((!fd) && (DataPtr->buildflg == FALSE)) {
			fprintf(stderr,"Could not find build configuration %s ... exiting\n", Filename); fflush(stderr);
			return 1;
		}
		
	}
 	parser = XML_ParserCreate(NULL);


  	DataPtr->depth = 0;
	XML_SetUserData(parser, DataPtr);
	

	XML_SetElementHandler(parser, startElement, end_Build_Element); 
	XML_SetCharacterDataHandler(parser, charhndl) ;


	do {
		int len = (int)fread(buf, 1, sizeof(buf), fd);
		done = len < sizeof(buf);
		
		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
		  fprintf(stderr,
				  "%s at line %" XML_FMT_INT_MOD "u\n",
				  XML_ErrorString(XML_GetErrorCode(parser)),
				  XML_GetCurrentLineNumber(parser));
		  return 1;
		}
	
	} while (!done);

	if (fd != stdin) fclose(fd);
	XML_ParserFree(parser);


	DataPtr->build.build_size = DataPtr->build.Delay_build_size
									+ DataPtr->build.ID_build_size  
									+ DataPtr->build.Ent_build_size 
									+ DataPtr->build.Comp_build_size 
									+ DataPtr->build.Noise_channel_size 
									+ DataPtr->build.FFT_build_size 
									+ DataPtr->build.Filter_build_size 
									+ DataPtr->build.Plr_build_size
									+ DataPtr->build.Math_build_size  
									+ DataPtr->build.Extern_build_size 
									+ DataPtr->build.Audio_build_size 
									+ DataPtr->build.Trace_build_size 
									+ DataPtr->build.Pot_channel_no 
									+ 2 ; // allowing for SCOPE and SYSTEM virtual modules
									
	if (DataPtr->System_Rate >= 0) DataPtr->build.System_Sample_Rate = DataPtr->System_Rate; //overwritig the panel set rate to the command line entered rate ... allow "zero" rate to conversly imply "infinte rate" ... let it rip
																							// setting rate to zero... run /hold may now be timed independently to assess the relative computational times in circuit-switch versus modules

	if ((DataPtr->build.build_size <=2)  && (DataPtr->build.Output_channel_no == 0) && (DataPtr->build.Input_channel_no == 0)) { // no modules && no i/o 
		fprintf(stderr,"Empty build ... exiting ... no files created...\n"); fflush(stderr);
		return 1;
	}
	else if ((DataPtr->build.build_size <=2) && (DataPtr->build.Input_channel_no == 0 )) {
		fprintf(stderr,"Build requires a data source; modules and/or standard input ... exiting ... no files created...\n"); fflush(stderr);
		return 1;
	}
	else if ((DataPtr->build.Trace_build_size == 0) && (DataPtr->build.Output_channel_no == 0) && (DataPtr->build.Extern_build_size == 0) && (DataPtr->build.Audio_build_size == 0)) { // need output at least
		fprintf(stderr,"Build requires at least one of standard output, trace or extern-OSC module ... exiting ... no files created...\n"); fflush(stderr);
		return 1;
	
	}
	else if ( DataPtr->build.Trace_build_size >= NUM_GRAPH) {
		fprintf(stderr,"no of Traces may not exceed %d  ... exiting ... no files created...\n", NUM_GRAPH); fflush(stderr);
		return 1;
	
	}
	else {
//		fprintf ( stderr, "Build complete\n"); fflush(stderr); // don't need this..
		return 0;
	}
}




/********************************************************************************************************************/
/*																													*/
/*	modules configuration																							*/
/*																													*/
/********************************************************************************************************************/

static void XMLCALL
end_Preset_Element(void *userData, const char *name)
{
	struct xml_data *dataPtr = (struct xml_data *) userData;
	int *depthPtr = (int *) &dataPtr->depth;
	float	tempfloat;
	int i;
	
	if (*depthPtr == 4) {
		if (!strcmp("key", dataPtr->tag)) {

			if (!strcmp(TAG_MOD_ID, dataPtr->content)) 	 			{dataPtr->key = MOD_ID;  dataPtr->content[0] = 0;}
			else if (!strcmp(TAG_MOD_TYPE, dataPtr->content)) 	 	{dataPtr->key = MOD_TYPE; dataPtr->content[0] = 0;}
			else if (!strcmp(TAG_MOD_LABEL, dataPtr->content)) 	 	{dataPtr->key = MOD_LABEL; dataPtr->content[0] = 0;}
	
			else if (!strcmp(TAG_MOD_PAR1_LABEL, dataPtr->content)) {dataPtr->key = MOD_PAR1_LABEL; dataPtr->content[0] = 0;}
			else if (!strcmp(TAG_MOD_PAR1_FLOAT, dataPtr->content)) {dataPtr->key = MOD_PAR1_FLOAT; dataPtr->content[0] = 0;}
	
			else if (!strcmp(TAG_MOD_PAR2_LABEL, dataPtr->content)) {dataPtr->key = MOD_PAR2_LABEL; dataPtr->content[0] = 0;}
			else if (!strcmp(TAG_MOD_PAR2_FLOAT, dataPtr->content)) {dataPtr->key = MOD_PAR2_FLOAT; dataPtr->content[0] = 0;}
	
			else if (!strcmp(TAG_MOD_PAR3_LABEL, dataPtr->content)) {dataPtr->key = MOD_PAR3_LABEL; dataPtr->content[0] = 0;}
			else if (!strcmp(TAG_MOD_PAR3_FLOAT, dataPtr->content)) {dataPtr->key = MOD_PAR3_FLOAT; dataPtr->content[0] = 0;}
	
			else if (!strcmp(TAG_MOD_PAR4_LABEL, dataPtr->content)) {dataPtr->key = MOD_PAR4_LABEL; dataPtr->content[0] = 0;}
			else if (!strcmp(TAG_MOD_PAR4_FLOAT, dataPtr->content)) {dataPtr->key = MOD_PAR4_FLOAT; dataPtr->content[0] = 0;}
	
			else if (!strcmp(TAG_MOD_PAR5_LABEL, dataPtr->content)) {dataPtr->key = MOD_PAR5_LABEL; dataPtr->content[0] = 0;}
			else if (!strcmp(TAG_MOD_PAR5_FLOAT, dataPtr->content)) {dataPtr->key = MOD_PAR5_FLOAT; dataPtr->content[0] = 0;}
	
			else if (!strcmp(TAG_MOD_PAR6_LABEL, dataPtr->content)) {dataPtr->key = MOD_PAR6_LABEL; dataPtr->content[0] = 0;}
			else if (!strcmp(TAG_MOD_PAR6_FLOAT, dataPtr->content)) {dataPtr->key = MOD_PAR6_FLOAT; dataPtr->content[0] = 0;}
	
			else if (!strcmp(TAG_MOD_BUFF_SZ,dataPtr->content)) 	{dataPtr->key = MOD_BUFF_SZ;  dataPtr->content[0] = 0;}
			else { dataPtr->key = -1; }

		}
		else if (!strcmp("string",dataPtr->tag)) {			
		
			switch (dataPtr->key) {

				case MOD_ID:			sscanf(dataPtr->content,"%d", &dataPtr->currentmodule.module_no);
//debug										fprintf (stderr, "MOD_ID  %d \n", dataPtr->currentmodule.module_no); fflush (stderr);
					break;
				
				case MOD_TYPE:
					if (!strcmp(dataPtr->build.ID_mod_label,dataPtr->content))  dataPtr->currentmodule.type = DIFFINT; 
					else if (!strcmp(dataPtr->build.Pot_label,dataPtr->content))  dataPtr->currentmodule.type = PRESETS; 
					else if (!strcmp(dataPtr->build.Filter_mod_label,dataPtr->content))  dataPtr->currentmodule.type = FILTER; 
					else if (!strcmp(dataPtr->build.Comp_mod_label,dataPtr->content))  dataPtr->currentmodule.type = COMPAR; 
					else if (!strcmp(dataPtr->build.Noise_mod_label,dataPtr->content))  dataPtr->currentmodule.type = NOISE; 
					else if (!strcmp(dataPtr->build.Delay_mod_label,dataPtr->content))  dataPtr->currentmodule.type = DELINE; 
					else if (!strcmp(dataPtr->build.Plr_mod_label,dataPtr->content))  dataPtr->currentmodule.type = POLAR; 
					else if (!strcmp(dataPtr->build.Math_mod_label,dataPtr->content))  dataPtr->currentmodule.type = MATHMOD; 
					else if (!strcmp(dataPtr->build.Ent_mod_label,dataPtr->content))  dataPtr->currentmodule.type = QTC; 
					else if (!strcmp(dataPtr->build.FFT_mod_label,dataPtr->content))  dataPtr->currentmodule.type = FFT; 
					else if (!strcmp(dataPtr->build.Extern_mod_label,dataPtr->content))  dataPtr->currentmodule.type = EXTERN; 
					else if (!strcmp(dataPtr->build.Audio_mod_label,dataPtr->content))  dataPtr->currentmodule.type = AUDIO; 
					else if (!strcmp(dataPtr->build.Trace_mod_label,dataPtr->content))  dataPtr->currentmodule.type = TRACE; 
					else if (!strcmp(dataPtr->build.Scope_mod_label,dataPtr->content))  dataPtr->currentmodule.type = SCOPE; 
					else if (!strcmp(dataPtr->build.System_mod_label,dataPtr->content))  dataPtr->currentmodule.type = SYSTEM; 
					else dataPtr->currentmodule.type = -1; // unrecognisable
//debug				fprintf (stderr, "MOD_TYPE  %d \n", dataPtr->currentmodule.type); fflush (stderr);
					break;
					
				case MOD_LABEL:			strncpy(dataPtr->currentmodule.name, dataPtr->content, DEFAULT_NAME_SIZE);
//debug				fprintf (stderr, "MOD_LABEL  %s \n", dataPtr->currentmodule.name); fflush (stderr);
						break;

				case MOD_PAR1_LABEL:	strncpy(dataPtr->currentmodule.ParamPtr[0]->name, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case MOD_PAR2_LABEL:	strncpy(dataPtr->currentmodule.ParamPtr[1]->name, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case MOD_PAR3_LABEL:	strncpy(dataPtr->currentmodule.ParamPtr[2]->name, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case MOD_PAR4_LABEL:	strncpy(dataPtr->currentmodule.ParamPtr[3]->name, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case MOD_PAR5_LABEL:	strncpy(dataPtr->currentmodule.ParamPtr[4]->name, dataPtr->content, DEFAULT_NAME_SIZE);				break;
				case MOD_PAR6_LABEL:	strncpy(dataPtr->currentmodule.ParamPtr[5]->name, dataPtr->content, DEFAULT_NAME_SIZE);				break;

				case MOD_PAR1_FLOAT:	if (1 == sscanf(dataPtr->content,"%f", &tempfloat)) 
											dataPtr->currentmodule.ParamPtr[0]->value = (double) tempfloat;
										strncpy (dataPtr->currentmodule.ParamPtr[0]->label , dataPtr->content, MAX_LABEL_LENG);	 // also capture content in label space									
													break;
													
				case MOD_PAR2_FLOAT:	if (1 == sscanf(dataPtr->content,"%f", &tempfloat))
											dataPtr->currentmodule.ParamPtr[1]->value = (double) tempfloat;
										strncpy (dataPtr->currentmodule.ParamPtr[1]->label , dataPtr->content, MAX_LABEL_LENG);  // capture content in label space

													break;
													
				case MOD_PAR3_FLOAT:	if (1 == sscanf(dataPtr->content,"%f", &tempfloat))
											dataPtr->currentmodule.ParamPtr[2]->value = (double) tempfloat;
										strncpy (dataPtr->currentmodule.ParamPtr[2]->label , dataPtr->content, MAX_LABEL_LENG);  // capture content in label space
													break;
													
				case MOD_PAR4_FLOAT:	if (1 == sscanf(dataPtr->content,"%f", &tempfloat))
											dataPtr->currentmodule.ParamPtr[3]->value = (double) tempfloat;
										strncpy (dataPtr->currentmodule.ParamPtr[3]->label , dataPtr->content, MAX_LABEL_LENG);  // capture content in label space
													break;
													
				case MOD_PAR5_FLOAT:	if (1 == sscanf(dataPtr->content,"%f", &tempfloat))
											dataPtr->currentmodule.ParamPtr[4]->value = (double) tempfloat;
										strncpy (dataPtr->currentmodule.ParamPtr[4]->label , dataPtr->content, MAX_LABEL_LENG);  // capture content in label space
													break;
													
				case MOD_PAR6_FLOAT:	if (1 == sscanf(dataPtr->content,"%f", &tempfloat))
											dataPtr->currentmodule.ParamPtr[5]->value = (double) tempfloat;
										strncpy (dataPtr->currentmodule.ParamPtr[5]->label , dataPtr->content, MAX_LABEL_LENG);  // capture content in label space
													break;

				case MOD_BUFF_SZ:		sscanf(dataPtr->content,"%d", &dataPtr->currentmodule.buff_sz);					
													break;

				default:
					break;			
			}
		}
		else if (!strcmp("real",dataPtr->tag)){
			fprintf (stderr, "Not expecting real content in plist\n");
		}	
	}
// now transfer the details into the module array but check validity of entries as one goes
	else if ( (*depthPtr == 3) && (!strcmp("dict",name))) {

		if (dataPtr->currentmodule.module_no != dataPtr->entry_counter) {
				fprintf (stderr, "...Moving module %d to %d in module presets list\n", dataPtr->currentmodule.module_no, dataPtr->entry_counter ); fflush (stderr);
				dataPtr->currentmodule.module_no = dataPtr->entry_counter;				
		}
		if (dataPtr->currentmodule.type != dataPtr->Modules[dataPtr->entry_counter]->type) {
			// two possibilites... may wish to look ahead to see if the type matches an upcoming module
			i = dataPtr->entry_counter ;
			while ((i < dataPtr->build.build_size) && (dataPtr->currentmodule.type != dataPtr->Modules[i]->type)) i++;
			if ( i != dataPtr->build.build_size) {  
				fprintf (stderr, "Moving module %d to %d in module presets list\n", dataPtr->currentmodule.module_no, i ); fflush (stderr);
				dataPtr->currentmodule.module_no = dataPtr->entry_counter = i;	// update the module number
				if (!strlen(dataPtr->currentmodule.name) ) strncpy (dataPtr->currentmodule.name , dataPtr->Modules[dataPtr->entry_counter]->name, MAX_LABEL_LENG); 
				CopyModulePresets( dataPtr ) ; 
				dataPtr->entry_counter += 1 ;		// increment the counter for next time
			}
			else { // drop the module entry... do nothing, don't increment the entry counter either the module is already 
				fprintf (stderr, "Droping module %d from module presets list\n", dataPtr->currentmodule.module_no ); fflush (stderr);
			}
		}
		else {
			if (!strlen(dataPtr->currentmodule.name) ) strncpy (dataPtr->currentmodule.name , dataPtr->Modules[dataPtr->entry_counter]->name, MAX_LABEL_LENG);  
			CopyModulePresets( dataPtr ) ; 
			dataPtr->entry_counter += 1 ;		// increment the counter for next time
		}
		Init_presets( &dataPtr->currentmodule);  // init build details
	} 
	

	*depthPtr -= 1;
}


//***************************************************************************************
void Write_Mods_Config(FILE *fd, char Filename[], struct xml_data *DataPtr) {	
	
	int 	i, j;
	char temp_string[100];

					
	fd = fopen (Filename,"w+");
	if (!fd) {
		fprintf(stderr,"Problem creating file: %s\n", Filename); fflush(stderr);
	} 
	else {
		fprintf(stderr,"Creating file: %s\n", Filename); fflush(stderr);
		print_pheader(fd);
		int totalsize = DataPtr->build.build_size;
		for (i=0; i < totalsize ; i++) {
			fprintf(fd, "<dict>\n");
			sprintf( temp_string, "%d", DataPtr->Modules[i]->module_no);
			plist_dict_pair(fd, TAG_MOD_ID,		temp_string,											STR_STR );
			
			switch (DataPtr->Modules[i]->type) {
				case  PRESETS:  plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.Pot_label,				STR_STR );		break;
				case  DELINE:	plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.Delay_mod_label,			STR_STR );		break;
				case  DIFFINT:	plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.ID_mod_label,			STR_STR );		break;
				case  QTC:		plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.Ent_mod_label,			STR_STR );		break;
				case  COMPAR:	plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.Comp_mod_label,			STR_STR );		break;
				case  NOISE:	plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.Noise_mod_label,			STR_STR );		break;
				case  FFT: 		plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.FFT_mod_label,			STR_STR );		break;
				case  FILTER:	plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.Filter_mod_label,		STR_STR );		break;
				case  POLAR:	plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.Plr_mod_label,			STR_STR );		break;
				case  MATHMOD:	plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.Math_mod_label,			STR_STR );		break;
				case  EXTERN:	plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.Extern_mod_label,		STR_STR );		break;
				case  AUDIO:	plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.Audio_mod_label,			STR_STR );		break;
				case  TRACE:	plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.Trace_mod_label,			STR_STR );		break;
				case  SCOPE:	plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.Scope_mod_label,			STR_STR );		break;
				case  SYSTEM: 	plist_dict_pair(fd, TAG_MOD_TYPE,	DataPtr->build.System_mod_label,		STR_STR );		break;
			}
				
			plist_dict_pair(fd, TAG_MOD_LABEL,	DataPtr->Modules[i]->name,			STR_STR );	
			
		
			if ((DataPtr->Modules[i]->type == QTC) 
					|| (DataPtr->Modules[i]->type == FFT) 
					|| (DataPtr->Modules[i]->type == FILTER)) {
				sprintf( temp_string, "%d", DataPtr->Modules[i]->buff_sz/BUFFERMULTIPLIER);
				plist_dict_pair(fd, TAG_MOD_BUFF_SZ,	temp_string,			STR_STR );	// adjusted if necessary
			}
			else if (DataPtr->Modules[i]->type == DIFFINT) {
				sprintf( temp_string, "%d",(DataPtr->Modules[i]->buff_sz - 1 )/ BUFFERSPLIT);
				plist_dict_pair(fd, TAG_MOD_BUFF_SZ,	temp_string,			STR_STR );	// adjusted if necessary
			}
			else if (DataPtr->Modules[i]->buff_sz > 0) {
				sprintf( temp_string, "%d",DataPtr->Modules[i]->buff_sz);
				plist_dict_pair(fd, TAG_MOD_BUFF_SZ,	temp_string,			STR_STR );	// adjusted if necessary
			}

	// to be improved later by tying into the pertecs.h definitions
			for (j=0; j < MAX_MODULE_PARAMETERS_SIZE;  j++) {
				if ( DataPtr->Modules[i]->ParamPtr[j]->module_no != -1) {
					if (strlen(DataPtr->Modules[i]->ParamPtr[j]->name)) { 
						fprintf(fd, "\t\t<key>P_Label %d</key>\n\t\t<string>%s</string>\n", j+1, DataPtr->Modules[i]->ParamPtr[j]->name); // output the name
						
						if (((DataPtr->Modules[i]->type == TRACE) || (DataPtr->Modules[i]->type == PRESETS) || (DataPtr->Modules[i]->type == DELINE)) && (strlen(DataPtr->Modules[i]->ParamPtr[j]->label))) { // to be treated differently
							fprintf(fd, "\t\t<key>P_float %d</key>\n\t\t<string>%s</string>\n", j+1, DataPtr->Modules[i]->ParamPtr[j]->label); // then output the string from the label
						}
						else fprintf(fd, "\t\t<key>P_float %d</key>\n\t\t<string>%g</string>\n", j+1, (float) DataPtr->Modules[i]->ParamPtr[j]->value); // then output the value
					}
				}	
			}
			fprintf(fd, "\t</dict>\n");
		}
		print_pclose(fd);
		
	}
}


void cpy_Build_debug(struct xml_data *DataPtr) {
char Command[MAXCOMMANDSZ];
	fprintf(stderr, "Creating file: %s~.build.pertMd\n", DataPtr->config_name); fflush(stderr);
	snprintf(Command, MAXCOMMANDSZ, "cp %s.build.pertMd %s~.build.pertMd", DataPtr->config_name, DataPtr->config_name); 
	system(Command);
};


void Mod_Names_debug(struct xml_data *DataPtr) {	
	
	FILE *fd;
	char Filename[FILE_NAME_SIZE];	
	 
	sprintf(Filename,"%s~.%s.pertPr", DataPtr->config_name, DataPtr->build.ext_prests); 
	Write_Mods_Config (fd, &Filename[0], DataPtr);	
}



// modules stuff
int presets( struct xml_data *DataPtr ) {

	char Filename[FILE_NAME_SIZE];	
	char buf[BUFSIZ];
	
	XML_Parser parser;
	int done, i, j, len;

	FILE *fd;
 
 
/* lets get presets */
	if (DataPtr->config_name == NULL) fd = stdin;
	else {
		sprintf(Filename,"%s.%s.pertPr", DataPtr->config_name, DataPtr->build.ext_prests); 
		fd = fopen (&Filename[0],"r+");
	
		// if presets do not exist create a preformated default for the current build 
		if (!fd) {	// write out new file or propagate changes through from old if a rebuild is performed.
			Write_Mods_Config ( fd, &Filename[0], DataPtr);

			sprintf(Filename,"open %s.%s.pertPr", DataPtr->config_name, DataPtr->build.ext_prests); 
			system(Filename);		// open the interface

			return 1;
		}
	}	
	// otherwise read the presents file

	
		// set up current-module buffer space
	DataPtr->currentmodule.name = (char *) malloc( DEFAULT_NAME_SIZE * sizeof(char)) ;

	for (i=0; i < MAX_MODULE_PARAMETERS_SIZE; i++) {
		DataPtr->currentmodule.ParamPtr[i] = (struct field *) malloc( sizeof(struct field)) ;
		DataPtr->currentmodule.ParamPtr[i]->name = (char *) malloc( DEFAULT_NAME_SIZE * sizeof(char)) ;
		DataPtr->currentmodule.ParamPtr[i]->label = (char *) malloc( MAX_LABEL_LENG * sizeof(char)) ;
	}

	Init_presets( &DataPtr->currentmodule);  // init presets
 	DataPtr->entry_counter = 0;		// init count ... count  entries as we go... compare to the module number to detect a change of build

 
  	parser = XML_ParserCreate(NULL);
  	
 	DataPtr->depth = 0;
	XML_SetUserData(parser, DataPtr);
	
	XML_SetElementHandler(parser, startElement, end_Preset_Element);
	XML_SetCharacterDataHandler(parser, charhndl) ;
	
	do {
		len = (int)fread(buf, 1, sizeof(buf), fd);
		done = len < sizeof(buf);
		
		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
		  fprintf(stderr,
				  "%s at line %" XML_FMT_INT_MOD "u\n",
				  XML_ErrorString(XML_GetErrorCode(parser)),
				  XML_GetCurrentLineNumber(parser));
		  return 1;
		}	
	} while (!done);
//	fprintf ( stderr, "read file '%s'\n",Filename); fflush(stderr);
	if (fd != stdin) fclose(fd);
	

	XML_ParserFree(parser);
	
	if (DataPtr->currentmodule.name != NULL) { free(DataPtr->currentmodule.name); DataPtr->currentmodule.name = NULL; }
	for (i=0; i < MAX_MODULE_PARAMETERS_SIZE; i++) {
		if (DataPtr->currentmodule.ParamPtr[i]->label != NULL)	{ free(DataPtr->currentmodule.ParamPtr[i]->label);	DataPtr->currentmodule.ParamPtr[i]->label = NULL; }
		if  (DataPtr->currentmodule.ParamPtr[i]->name != NULL)	{ free(DataPtr->currentmodule.ParamPtr[i]->name); 	DataPtr->currentmodule.ParamPtr[i]->name = NULL; }
		if (DataPtr->currentmodule.ParamPtr[i] != NULL) 		{ free(DataPtr->currentmodule.ParamPtr[i]); 		DataPtr->currentmodule.ParamPtr[i] = NULL; }
	}


	return 0;
}


/********************************************************************************************************************/
/*					*/
/*	ionames			*/
/*					*/
/********************************************************************************************************************/
// will continue use the current module struct to temporarily store the data.

static void XMLCALL
end_ionames_Element(void *userData, const char *name)
{
	int mod_no; 
	struct xml_data *dataPtr = (struct xml_data *) userData;
	int *depthPtr = (int *) &dataPtr->depth;
	
		

	if (*depthPtr == 4) {
		if (!strcmp("key", dataPtr->tag)) {

			if (!strcmp(TAG_IO_ID, dataPtr->content)) 	 			dataPtr->key = IO_ID; 			// "io_No" [0-9]*  module number 
			else if (!strcmp(TAG_IO_TYPE, dataPtr->content)) 	 	dataPtr->key = IO_TYPE;			// "Channel_class" [OUTPUT][INPUT][LUG] i/o type
			else if (!strcmp(TAG_IO_MOD_NAME, dataPtr->content)) 	dataPtr->key = IO_MODULE;		// "Module_label"  defaults to: [OUTPUT][INPUT][LUG]_[io #] module label
			else if (!strcmp(TAG_IO_LABEL, dataPtr->content)) 	 	dataPtr->key = IO_LABEL;		// "Signal_label" 
			else if (!strcmp(TAG_IO_META, dataPtr->content)) 	 	dataPtr->key = IO_META;		    // "Meta_info" is optional text... 
	
		}
		else if (!strcmp("string",dataPtr->tag)) {
			switch (dataPtr->key) {

				case IO_ID:			sscanf(dataPtr->content,"%d", &dataPtr->currentio_module.module_no);					//  module no 
																										break;
				
				case IO_TYPE:		
//debug						fprintf (stderr, "list types  %s %s %s \n", dataPtr->build.Input_label, dataPtr->build.Output_label, dataPtr->build.Connection_label); fflush (stderr);
					if (!strcmp(dataPtr->build.Input_label,dataPtr->content))  {
						dataPtr->currentio_module.type = INPUT;
						strncpy(  dataPtr->currentio_module.put.In->name, dataPtr->build.Input_out_name, DEFAULT_NAME_SIZE);
					}
					else if (!strcmp(dataPtr->build.Output_label,dataPtr->content)) {
						dataPtr->currentio_module.type = OUTPUT;
						strncpy(  dataPtr->currentio_module.put.Out->name, dataPtr->build.Output_in_name, DEFAULT_NAME_SIZE);
					}
					else if (!strcmp(dataPtr->build.Connection_label,dataPtr->content)) {
				 		dataPtr->currentio_module.type = LUG; 
						strncpy(  dataPtr->currentio_module.put.Out->name, dataPtr->build.Connection_lug_name, DEFAULT_NAME_SIZE);
				 	}
					else {
						dataPtr->currentio_module.type = -1; // unrecognisable
					
					}
					
//debug						fprintf (stderr, "list types  %s %s %s \n", dataPtr->build.Input_out_name, dataPtr->build.Output_in_name, dataPtr->build.Connection_lug_name); fflush (stderr);
					
//debug						fprintf (stderr, "IO_TYPE  %d %s\n", dataPtr->currentio_module.type, dataPtr->currentio_module.put.Out->name); fflush (stderr);

					break; 
				
				case IO_MODULE:				strncpy(dataPtr->currentio_module.name, dataPtr->content, DEFAULT_NAME_SIZE);
//debug						fprintf (stderr, "MOD_LABEL  %s \n", dataPtr->currentio_module.name); fflush (stderr);
					break; // name will have module label in it
					
				case IO_LABEL:				strncpy(dataPtr->currentio_module.put.In->label, dataPtr->content, DEFAULT_NAME_SIZE);
//						fprintf (stderr, "SIG_NAME  %s \n", dataPtr->currentio_module.put.In->label); fflush (stderr);
					break; // label will have signal name in it

				case IO_META:				strncpy(dataPtr->currentio_module.put.In->metaref, dataPtr->content, DEFAULT_NAME_SIZE);		
//					fprintf (stderr, "IO_META  %s \n", dataPtr->currentio_module.put.In->metaref); fflush (stderr);
					break; // meta data or reference string

				default:
					break;			
			}
		}
		else if (!strcmp("real",dataPtr->tag)){
			fprintf (stderr, "not expecting real content in plist\n");
		}	
	}

// now transfer the details into the module array but check validity of entries as one goes
	else if ( (*depthPtr == 3) && (!strcmp("dict",name))) {
		if (dataPtr->currentio_module.module_no != dataPtr->entry_counter) {
				fprintf (stderr, "...Moving i/o module %d to %d in input/output list\n", dataPtr->currentio_module.module_no, dataPtr->entry_counter ); fflush (stderr);
				dataPtr->currentio_module.module_no = dataPtr->entry_counter;				
		}
		mod_no  = dataPtr->currentio_module.module_no - dataPtr->build.build_size; // get a count relative to the io modules

		if (dataPtr->currentio_module.type != INPUT ) { // TBD need to check if the i/o module matches expectation


		}

		if ((dataPtr->build.Input_channel_no > 0) && (mod_no >= 0 ) && (mod_no <= dataPtr->build.Input_channel_no )) { 
			if (dataPtr->currentio_module.type != INPUT ) { // expecting it to be Input... 
				// need to decide what to do here
				fprintf (stderr, "Droping i/o module %d from input/output  list\n", dataPtr->currentio_module.module_no ); fflush (stderr);

			}
			else {
				dataPtr->IO_Modules[mod_no]->put.In->module_no = dataPtr->currentio_module.module_no;
				strncpy( dataPtr->IO_Modules[mod_no]->name, dataPtr->currentio_module.name, DEFAULT_NAME_SIZE); 	// module name
				strncpy( dataPtr->IO_Modules[mod_no]->put.In->name, dataPtr->currentio_module.put.In->name, DEFAULT_NAME_SIZE); 	// signal label
				strncpy( dataPtr->IO_Modules[mod_no]->put.In->label, dataPtr->currentio_module.put.In->label, DEFAULT_NAME_SIZE); 	// signal label
				strncpy( dataPtr->IO_Modules[mod_no]->put.In->metaref, dataPtr->currentio_module.put.In->metaref, DEFAULT_NAME_SIZE); 	// signal meta ref
				mod_no = -1;
				dataPtr->entry_counter += 1;
			}
		}
		else if (dataPtr->build.Input_channel_no > 0) mod_no = mod_no - dataPtr->build.Input_channel_no - 1;
		

		if ((dataPtr->build.Output_channel_no > 0) && (mod_no >= 0 ) &&  (mod_no <= dataPtr->build.Output_channel_no )) {
			if (dataPtr->currentio_module.type != OUTPUT ) {
				// need to decide what to do here
				fprintf (stderr, "Droping i/o module %d from input/output  list\n", dataPtr->currentio_module.module_no ); fflush (stderr);

			}
			else {
				dataPtr->Output_Modules[mod_no]->put.Out->module_no = dataPtr->currentio_module.module_no;
				strncpy(  dataPtr->Output_Modules[mod_no]->name, dataPtr->currentio_module.name, DEFAULT_NAME_SIZE); // module name
				strncpy(  dataPtr->Output_Modules[mod_no]->put.Out->name, dataPtr->currentio_module.put.In->name, DEFAULT_NAME_SIZE);
				strncpy(  dataPtr->Output_Modules[mod_no]->put.Out->label, dataPtr->currentio_module.put.In->label, DEFAULT_NAME_SIZE);
				strncpy( dataPtr->Output_Modules[mod_no]->put.Out->metaref, dataPtr->currentio_module.put.In->metaref, DEFAULT_NAME_SIZE); 	// signal meta ref
				mod_no = -1;
				dataPtr->entry_counter += 1;
			}
		}
		else if (dataPtr->build.Output_channel_no > 0) mod_no = mod_no - dataPtr->build.Output_channel_no - 1;  //  allow for the frame counter ... line not now required
		

		if ((dataPtr->build.Connection_build_size > 0) && (mod_no >= 0 ) &&  (mod_no < dataPtr->build.Connection_build_size )) {
			if (dataPtr->currentio_module.type != LUG ) {
				fprintf (stderr, "Droping terminal lug module %d from list\n", dataPtr->currentio_module.module_no ); fflush (stderr);
			}
			else {
				dataPtr->lug_Modules[mod_no]->put.In->module_no = dataPtr->currentio_module.module_no;
				strncpy(  dataPtr->lug_Modules[mod_no]->name, dataPtr->currentio_module.name, DEFAULT_NAME_SIZE); // module name
				strncpy(  dataPtr->lug_Modules[mod_no]->put.In->name, dataPtr->currentio_module.put.In->name, DEFAULT_NAME_SIZE);
				strncpy(  dataPtr->lug_Modules[mod_no]->put.In->label, dataPtr->currentio_module.put.In->label, DEFAULT_NAME_SIZE);
				strncpy( dataPtr->lug_Modules[mod_no]->put.In->metaref, dataPtr->currentio_module.put.In->metaref, DEFAULT_NAME_SIZE); 	// signal meta ref
				mod_no = -1;
				dataPtr->entry_counter += 1;
			}
		}



	//debug		fprintf(stderr," %d, %d, %s, %s\n",dataPtr->currentio_module.module_no, dataPtr->currentio_module.type, dataPtr->currentio_module.name, dataPtr->currentio_module.label);

		Init_ionames (&dataPtr->currentio_module);  // reset currentmodule/field values to 0;

	} 
	
	*depthPtr -= 1;
}



/********************************************************************************************************************/

void Write_IO_Config(FILE *fd, char Filename[], struct xml_data *DataPtr) {	
	
	int 	i, j;
	char temp_string[100];
					
	fd = fopen (Filename,"w+");
	if (!fd) {
		fprintf(stderr,"Problem creating file: %s\n", Filename); fflush(stderr);
	} 
	else {
		fprintf(stderr,"Creating file: %s\n", Filename); fflush(stderr);
	
		print_pheader(fd);
		
		if (DataPtr->build.Input_channel_no) {
			for (i=0; i <= DataPtr->build.Input_channel_no ; i++) {
				fprintf(fd, "\t<dict>\n");
				sprintf( temp_string, "%d", DataPtr->IO_Modules[i]->put.In->module_no);
				plist_dict_pair(fd, TAG_IO_ID,			temp_string,														STR_STR );	
				plist_dict_pair(fd, TAG_IO_TYPE,		DataPtr->build.Input_label,											STR_STR );	

				if (strlen(DataPtr->IO_Modules[i]->name))  plist_dict_pair(fd, TAG_IO_MOD_NAME,		DataPtr->IO_Modules[i]->name,			STR_STR );
				if (strlen(DataPtr->IO_Modules[i]->put.In->label)) plist_dict_pair(fd, TAG_IO_LABEL,		DataPtr->IO_Modules[i]->put.In->label,			STR_STR );
				if (strlen(DataPtr->IO_Modules[i]->put.In->metaref)) plist_dict_pair(fd, TAG_IO_META,		DataPtr->IO_Modules[i]->put.In->metaref,			STR_STR ); // pass on the meta data/ref
				fprintf(fd,"\t</dict>\n");
			}
		}

		if (DataPtr->build.Output_channel_no) {
			for (i=0; i <= DataPtr->build.Output_channel_no ; i++) {
				fprintf(fd, "\t<dict>\n");
				sprintf( temp_string, "%d", DataPtr->Output_Modules[i]->put.Out->module_no);
				plist_dict_pair(fd, TAG_IO_ID,			temp_string,											STR_STR );	
				plist_dict_pair(fd, TAG_IO_TYPE,		DataPtr->build.Output_label,											STR_STR );	

				if (strlen(DataPtr->Output_Modules[i]->name))  plist_dict_pair(fd, TAG_IO_MOD_NAME,		DataPtr->Output_Modules[i]->name,			STR_STR );
				if (strlen(DataPtr->Output_Modules[i]->put.Out->label)) plist_dict_pair(fd, TAG_IO_LABEL,		DataPtr->Output_Modules[i]->put.Out->label,			STR_STR );
				if (strlen(DataPtr->Output_Modules[i]->put.Out->metaref)) plist_dict_pair(fd, TAG_IO_META,		DataPtr->Output_Modules[i]->put.Out->metaref,			STR_STR ); // pass on the meta data/ref

				fprintf(fd,"\t</dict>\n");
			}
		}

		if (DataPtr->build.Connection_build_size) {
			for (i=0; i < DataPtr->build.Connection_build_size ; i++) {
				fprintf(fd, "\t<dict>\n");
				sprintf( temp_string, "%d", DataPtr->lug_Modules[i]->put.In->module_no);
				plist_dict_pair(fd, TAG_IO_ID,			temp_string,											STR_STR );	
				plist_dict_pair(fd, TAG_IO_TYPE,		DataPtr->build.Connection_label,											STR_STR );	

				if (strlen(DataPtr->lug_Modules[i]->name))  plist_dict_pair(fd, TAG_IO_MOD_NAME,		DataPtr->lug_Modules[i]->name,			STR_STR );
				if (strlen(DataPtr->lug_Modules[i]->put.In->label)) plist_dict_pair(fd, TAG_IO_LABEL,		DataPtr->lug_Modules[i]->put.In->label,			STR_STR );
				if (strlen(DataPtr->lug_Modules[i]->put.In->metaref)) plist_dict_pair(fd, TAG_IO_META,		DataPtr->lug_Modules[i]->put.In->metaref,			STR_STR ); // pass on the meta data/ref

				fprintf(fd,"\t</dict>\n");
			}
		}

		print_pclose(fd);

	}
}


void IO_Names_debug(struct xml_data *DataPtr) {	
	
	FILE *fd;
	char Filename[FILE_NAME_SIZE];	
	 
	sprintf(Filename,"%s~.%s.pertNm", DataPtr->config_name, DataPtr->build.ext_ionms); 
	Write_IO_Config(fd, &Filename[0], DataPtr);	
}


/********************************************************************************************************************/
/********** inputout *******/
int ionames( struct xml_data *DataPtr ) {

	char Filename[FILE_NAME_SIZE];	
	char buf[BUFSIZ];
	
	XML_Parser parser;
	int done, i, j;

	FILE *fd;
 
 			
 
/* lets get ionames */
	if (DataPtr->config_name == NULL) fd = stdin;
	else {
																			// then read existing name assignements for purposes of resolving the circuit
		sprintf(Filename,"%s.%s.pertNm", DataPtr->config_name, DataPtr->build.ext_ionms); 
		
		fd = fopen (&Filename[0],"r+");	
		// if ionames do not exist create a preformated default for the current build 
		if (!fd) {
					// write out new file and propagate build details through.
			Write_IO_Config(fd, &Filename[0], DataPtr);
			
			sprintf(Filename,"open %s.%s.pertNm", DataPtr->config_name, DataPtr->build.ext_ionms); 
			system(Filename);
			
			return 1;
		}

	}
	// otherwise read the io file
	
	// set up temporary current-currentio_module buffer space
	
	DataPtr->currentio_module.name = (char *) malloc( DEFAULT_NAME_SIZE * sizeof(char)) ;
	DataPtr->currentio_module.put.In = (struct field *) calloc( 1, sizeof(struct field)) ;
	
	DataPtr->currentio_module.put.In->name = (char *) malloc( DEFAULT_NAME_SIZE * sizeof(char)) ;
	DataPtr->currentio_module.put.In->label = (char *) malloc( DEFAULT_NAME_SIZE * sizeof(char)) ;
	DataPtr->currentio_module.put.In->metaref = (char *) malloc( DEFAULT_NAME_SIZE * sizeof(char)) ;

	Init_ionames( &DataPtr->currentio_module);  // init io module	
	DataPtr->entry_counter = DataPtr->build.build_size;		// init count ... count  entries as we go... compare to the module number to detect a change of build


	parser = XML_ParserCreate(NULL);
	
	DataPtr->depth = 0;
	XML_SetUserData(parser, DataPtr);
	
	XML_SetElementHandler(parser, startElement, end_ionames_Element);
	XML_SetCharacterDataHandler(parser, charhndl) ;
	
	do {
		int len = (int)fread(buf, 1, sizeof(buf), fd);
		done = len < sizeof(buf);
		
		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
		  fprintf(stderr,
				  "%s at line %" XML_FMT_INT_MOD "u\n",
				  XML_ErrorString(XML_GetErrorCode(parser)),
				  XML_GetCurrentLineNumber(parser));
		  return 1;
		}
	} while (!done);
	
	if (fd != stdin) fclose(fd);
	XML_ParserFree(parser);

	
		// free up temporary  buffer space
	if (DataPtr->currentio_module.name != NULL) { free(DataPtr->currentio_module.name); DataPtr->currentio_module.name = NULL; }

	if (DataPtr->currentio_module.put.In->name != NULL) { free(DataPtr->currentio_module.put.In->name); DataPtr->currentio_module.put.In->name = NULL; }
	if (DataPtr->currentio_module.put.In->label != NULL) { free(DataPtr->currentio_module.put.In->label); DataPtr->currentio_module.put.In->label = NULL; }
	if (DataPtr->currentio_module.put.In->metaref != NULL) { free(DataPtr->currentio_module.put.In->metaref); DataPtr->currentio_module.put.In->metaref = NULL; }
	if (DataPtr->currentio_module.put.In != NULL) { free(DataPtr->currentio_module.put.In); DataPtr->currentio_module.put.In = NULL; }


	return 0;
	
}

/********************************************************************************************************************/
/*																													*/
/*	pots module 																									*/
/*																													*/
/********************************************************************************************************************/

static void XMLCALL
end_Pots_Element(void *userData, const char *name)
{
	struct xml_data *dataPtr = (struct xml_data *) userData;
	int *depthPtr = (int *) &dataPtr->depth;
	float	tempfloat;
	int j = 0;

	
	if (*depthPtr == 4) {
		if (!strcmp("key", dataPtr->tag)) {

			if (!strcmp(TAG_POT_BANK_NAME, dataPtr->content)) 	 			dataPtr->key = POT_BANK_NAME; 
	
			else if (!strcmp(TAG_POT_INPUT1, dataPtr->content)) 			dataPtr->key = POT_INPUT1; 
			else if (!strcmp(TAG_POT_INPUT2, dataPtr->content)) 			dataPtr->key = POT_INPUT2;
			else if (!strcmp(TAG_POT_INPUT3, dataPtr->content)) 			dataPtr->key = POT_INPUT3;

			else if (!strcmp(TAG_POT_SELECT1, dataPtr->content)) 			dataPtr->key = POT_SELECT1;	
			else if (!strcmp(TAG_POT_SELECT2, dataPtr->content)) 			dataPtr->key = POT_SELECT2;
			else if (!strcmp(TAG_POT_SELECT3, dataPtr->content)) 			dataPtr->key = POT_SELECT3;
	
			else dataPtr->key = -1;

		}
		else if (!strcmp("string",dataPtr->tag)) {
		
			switch (dataPtr->key) {

				case POT_BANK_NAME:
					j = 0;
					// search through modules for the rignt module number and copy all the values through to the current
					while ((j < dataPtr->build.build_size) && (strcmp(dataPtr->Modules[j]->name,  dataPtr->content) )) j++;
					if (!strcmp(dataPtr->Modules[j]->name, dataPtr->content)) {
						Init_presets( &dataPtr->currentmodule);  // init build details
						CopyToCurrent(dataPtr->Modules[j],  &dataPtr->currentmodule ) ;
//						fprintf(stderr,"got here %d, %s\n", j, dataPtr->content, DEFAULT_NAME_SIZE); fflush(stderr);
					}
					break;
				default:
					break;			
			}

		}
		else if (!strcmp("real",dataPtr->tag)){

			if (1 == sscanf(dataPtr->content,"%f", &tempfloat)) {
				switch (dataPtr->key) {
				
					case POT_INPUT1:	dataPtr->currentmodule.ParamPtr[0]->value = (double) tempfloat /1000; break; 
					case POT_INPUT2: 	dataPtr->currentmodule.ParamPtr[2]->value = (double) tempfloat /1000; break; 
					case POT_INPUT3:	dataPtr->currentmodule.ParamPtr[4]->value = (double) tempfloat /1000; break; 
				default:
					break;			
				}
			}

		}
		else if (!strcmp("true",dataPtr->tag)){

			switch (dataPtr->key) {
				case POT_SELECT1:
					strncpy (dataPtr->currentmodule.ParamPtr[1]->label , "enable", MAX_LABEL_LENG);	 
					dataPtr->currentmodule.ParamPtr[1]->value = 1.0 ;
					break;
				case POT_SELECT2:
			 		strncpy (dataPtr->currentmodule.ParamPtr[3]->label , "enable", MAX_LABEL_LENG);	
					dataPtr->currentmodule.ParamPtr[3]->value = 1.0 ;
					 break;
				case POT_SELECT3:
					strncpy (dataPtr->currentmodule.ParamPtr[5]->label , "enable", MAX_LABEL_LENG);	 
					dataPtr->currentmodule.ParamPtr[5]->value = 1.0 ;
					break;
			 	default: break;
			}

		}
		else if (!strcmp("false",dataPtr->tag)){

			switch (dataPtr->key) {
				case POT_SELECT1:
					strncpy (dataPtr->currentmodule.ParamPtr[1]->label , "disable", MAX_LABEL_LENG);
					dataPtr->currentmodule.ParamPtr[1]->value = 0.0 ;
					break;
				case POT_SELECT2:
			 		strncpy (dataPtr->currentmodule.ParamPtr[3]->label , "disable", MAX_LABEL_LENG);	 
					dataPtr->currentmodule.ParamPtr[3]->value = 0.0 ;
			 		break;
				case POT_SELECT3:
					strncpy (dataPtr->currentmodule.ParamPtr[5]->label , "disable", MAX_LABEL_LENG);	 
					dataPtr->currentmodule.ParamPtr[5]->value = 0.0 ;
					break;
			 	default: break;
			}
			
		}

	}
// now transfer the details back the module array but check validity of entries as one goes
	else if ( (*depthPtr == 3) && (!strcmp("dict",name))) {
		CopyModulePresets( dataPtr ); 
		Init_presets( &dataPtr->currentmodule);  // init build details
	} 
	
	*depthPtr -= 1;
}



void write_pots_file(struct xml_data *DataPtr, int	debugflag) {

	int 	j;
	
	char Filename[FILE_NAME_SIZE];	
	FILE *fd;


	if (DataPtr->build.Pot_channel_no) {
		if (!debugflag) sprintf(Filename,"%s.pertPts", DataPtr->config_name ); 
		else sprintf(Filename,"%s~.pertPts", DataPtr->config_name ); 
		// execute a brute force resave
		fd = fopen (&Filename[0],"w+");
		if (!fd) {
			fprintf(stderr,"Problem creating file: %s\n", Filename); fflush(stderr);
		} 
		else {
			fprintf(stderr,"Creating file: %s\n", Filename); fflush(stderr);		
			print_pheader(fd);
			for ( j = 0; j < DataPtr->build.build_size; j++) {
				
				if ( DataPtr->Modules[j]->type == PRESETS) {
					fprintf(fd, "\t<dict>\n\t\t<key>Number</key>\n\t\t<string>%s</string>\n", DataPtr->Modules[j]->name ); 
					
					if (DataPtr->Modules[j]->ParamPtr[0]->value <  0.0 ) fprintf(fd, "\t\t<key>Potentiometer1</key>\n\t\t<real>0</real>\n");
					else if (DataPtr->Modules[j]->ParamPtr[0]->value > 1) fprintf(fd, "\t\t<key>Potentiometer1</key>\n\t\t<real>1000</real>\n");
					else fprintf(fd, "\t\t<key>Potentiometer1</key>\n\t\t<real>%g</real>\n", DataPtr->Modules[j]->ParamPtr[0]->value * 1000.0 );

					if (DataPtr->Modules[j]->ParamPtr[2]->value <  0.0 ) fprintf(fd, "\t\t<key>Potentiometer2</key>\n\t\t<real>0</real>\n");
					else if (DataPtr->Modules[j]->ParamPtr[2]->value > 1) fprintf(fd, "\t\t<key>Potentiometer2</key>\n\t\t<real>1000</real>\n");
					else fprintf(fd, "\t\t<key>Potentiometer2</key>\n\t\t<real>%g</real>\n", DataPtr->Modules[j]->ParamPtr[2]->value * 1000.0 );

					if (DataPtr->Modules[j]->ParamPtr[4]->value <  0.0 ) fprintf(fd, "\t\t<key>Potentiometer3</key>\n\t\t<real>0</real>\n");
					else if (DataPtr->Modules[j]->ParamPtr[4]->value > 1) fprintf(fd, "\t\t<key>Potentiometer3</key>\n\t\t<real>1000</real>\n");
					else fprintf(fd, "\t\t<key>Potentiometer3</key>\n\t\t<real>%g</real>\n", DataPtr->Modules[j]->ParamPtr[4]->value * 1000.0 );
				
					
					if ( DataPtr->Modules[j]->ParamPtr[1]->label[0] == 'e' ) fprintf(fd, "\t\t<key>Select1</key>\n\t\t<true/>\n");
					else fprintf(fd, "\t\t<key>Select1</key>\n\t\t<false/>\n");

					if ( DataPtr->Modules[j]->ParamPtr[3]->label[0] == 'e' ) fprintf(fd, "\t\t<key>Select2</key>\n\t\t<true/>\n");
					else fprintf(fd, "\t\t<key>Select2</key>\n\t\t<false/>\n");

					if ( DataPtr->Modules[j]->ParamPtr[5]->label[0] == 'e' ) fprintf(fd, "\t\t<key>Select3</key>\n\t\t<true/>\n");
					else fprintf(fd, "\t\t<key>Select3</key>\n\t\t<false/>\n");
					fprintf(fd, "\t</dict>\n") ;				
				}
			}
			print_pclose(fd);
			fclose(fd);
		}
	}	
}





// potentiometers stuff
int potentiometers(struct xml_data *DataPtr ) {

	char Filename[FILE_NAME_SIZE];	
	char buf[BUFSIZ];
	
	XML_Parser parser;
	int done, i, j;

	FILE *fd;
 
 			
 
/* lets get presets */
	if (DataPtr->config_name == NULL) return 1;


	if (DataPtr->build.Pot_channel_no) {
		
		sprintf(Filename,"%s.pertPts", DataPtr->config_name); 
		fd = fopen (&Filename[0],"r+");
	
		if (!fd) { // file doesn't exist so create it
				write_pots_file( DataPtr, FALSE);
	
				sprintf(Filename,"open %s.pertPts", DataPtr->config_name); 
				system(Filename);
			
			return 1;
		}
		
		// otherwise read the present file
		
		// set up current-module buffer space (same as in mods, except that we only deal with the potentiomenrs
		DataPtr->currentmodule.name = (char *) malloc( DEFAULT_NAME_SIZE * sizeof(char)) ;
	
		for (i=0; i < MAX_MODULE_PARAMETERS_SIZE; i++) {
			DataPtr->currentmodule.ParamPtr[i] = (struct field *) malloc( sizeof(struct field)) ;
			DataPtr->currentmodule.ParamPtr[i]->name = (char *) malloc( DEFAULT_NAME_SIZE * sizeof(char)) ;
			DataPtr->currentmodule.ParamPtr[i]->label = (char *) malloc( MAX_LABEL_LENG * sizeof(char)) ;
		}
	
		Init_presets( &DataPtr->currentmodule);  // init current module
	
	 
		parser = XML_ParserCreate(NULL);
		
		DataPtr->depth = 0;
		XML_SetUserData(parser, DataPtr);
		
		XML_SetElementHandler(parser, startElement, end_Pots_Element);
		XML_SetCharacterDataHandler(parser, charhndl) ;
		
		do {
		int len = (int)fread(buf, 1, sizeof(buf), fd);
		done = len < sizeof(buf);
		
		if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
		  fprintf(stderr,
				  "%s at line %" XML_FMT_INT_MOD "u\n",
				  XML_ErrorString(XML_GetErrorCode(parser)),
				  XML_GetCurrentLineNumber(parser));
		  return 1;
		}
		
		} while (!done);
	//	fprintf ( stderr, "read file '%s'\n",Filename); fflush(stderr);
		fclose(fd);
		
	/*	if (DataPtr->debugflg == TRUE) {
			sprintf(Filename,"%s~.pertPts", DataPtr->config_name); 
			fd = fopen (&Filename[0],"w+");
			if (!fd) {
				fprintf(stderr,"Problem creating debug file: %s\n", Filename); fflush(stderr);
			} 
			else {
				fprintf(stderr,"Creating debug pots file: %s\n", Filename); fflush(stderr);
	//			write_mods_file(fd, DataPtr);
	
			}
		}
	*/
		XML_ParserFree(parser);
		
		if (DataPtr->currentmodule.name != NULL) { free(DataPtr->currentmodule.name); DataPtr->currentmodule.name = NULL; }
		for (i=0; i < MAX_MODULE_PARAMETERS_SIZE; i++) {
			if (DataPtr->currentmodule.ParamPtr[i]->label != NULL)	{ free(DataPtr->currentmodule.ParamPtr[i]->label);	DataPtr->currentmodule.ParamPtr[i]->label = NULL; }
			if (DataPtr->currentmodule.ParamPtr[i]->name != NULL)	{ free(DataPtr->currentmodule.ParamPtr[i]->name); 	DataPtr->currentmodule.ParamPtr[i]->name = NULL; }
			if (DataPtr->currentmodule.ParamPtr[i] != NULL) 		{ free(DataPtr->currentmodule.ParamPtr[i]); 		DataPtr->currentmodule.ParamPtr[i] = NULL; }
		}
	}

	return 0;
}
