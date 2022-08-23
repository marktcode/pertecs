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
#include <termios.h> /* POSIX terminal control definitions */     


#define MAXCOMMANDSZ 128

//**************  miscel defines ***********************
#define	TRUE	1
#define	FALSE	0
#define LARGE_FINITE_NUMBER		(1.0e300)

#define FILE_NAME_SIZE		(40)

#define MAX_LABEL_LENG 		(40)
#define DEFAULT_NAME_SIZE	(MAX_LABEL_LENG)
#define XML_STR_SIZE		(6 * MAX_LABEL_LENG)
#define XML_CONTENT_SIZE		(40 * MAX_LABEL_LENG)

#define MAX_BUILD_SIZE (2000) // number of modules
#define MAX_CHANNELS	(500) // ambitious ?!
#define MAX_POTS		(99) // too many?
#define MAX_NOISE		(200) // 
#define MAX_STACK_SIZE (16) // push stack determines the number of simultaneous connections in forming products... limited to six by interface
#define MAX_FRAME_SIZE (500) // determines the number of inputs and outputs max

#define NUM_SURF 		(10)
#define NUM_GRAPH 		(500)
#define NUM_BALLS 		(200)

#define DEFAULTRATE 	(1000) // 1KHz sample rate by default
#define NO_ZEROS		(5)		   // default no of zeros in gaussian weighted sinc interpolation filter

#define NORMAL			(0)
#define	DOWNSAMPLE		(1)
#define	STROBE			(2)			// output strobed with channel one = 1 w/text frame
#define	STROBEOSC		(3)			// output strobed with channel one = 1 w/OSC /ddata host address at 127.0.0.1 12000
#define	UP_OUTSAMPLE	(-1)
#define	UP_INSAMPLE		(-2)
#define	UP_INSAMPLE_INTERPOLATE	(-3)


//wire guage sets resolution of engine
#define guage	double

#define	DELIM	'_'

//**************  control state defines ***********************
#define QUIT	-3
#define STOP	-2
#define RESET	-1
#define PAUSE	0 // runs for any positive value
#define RUN		-4
#define TOGGLE	1	// between the pause and run possibly not used


// thread control file
#define STOP_MODE 		0
#define START_MODE 		1
#define CONTINUE_MODE 	2



// **** xml content type  *****
#define STRING_CONTENT 		0
#define REAL_CONTENT 		1



// ***************** define module types  **********************
#define PRESETS	1		// potentiometers
#define DELINE	2
#define DIFFINT 3
#define QTC		4
#define	COMPAR 	5
#define NOISE	6		// random
#define FFT		7
#define FILTER	8
#define	POLAR	9
#define	MATHMOD 10
#define EXTERN	11
#define TRACE	12
#define SCOPE	13		//pseudo module
#define SYSTEM	14		//pseudo module
#define INPUT	15		
#define OUTPUT	16		
#define LUG		17		
#define AUDIO	18		



// ***************** module inputs  **********************
#define MAX_MODULE_INPUTS_SIZE (3) // includes standard, special, delayed inputs
// ***************** generic inputs and outputs of modules **********************
#define STANDARD	0
#define SPECIAL 	1
#define	SETHOLD		2		// integratior set or hold ... set = 1, normal = 0, hold -1
#define	SPECIAL2	2

#define REALFFTIN 	0
#define IMAGFFTIN 	1
#define SYNCIN 		2


// ***************** module outputs  **********************
#define MAX_MODULE_OUTPUTS_SIZE (6)
// ***************** generic *******************************
#define	DELAYED		5 // coincides with the delayed sampling point of the buffer midpoint

// ***************** Filter outputs ******************
#define FILTEROUT 	1
// ***************** POTS  outputs ******************
#define POTINX 		0
#define POTINY 		1
#define POTINZ 		2
#define SELINX 		3
#define SELINY 		4
#define SELINZ 		5

// ***************** COMPARATOR outputs ******************
#define POS 		4
#define ZERO 		3
#define NONPOS 		2
#define	NEG			1
#define	NONNEG		0
// ***************** NOISE output ******************
#define NOISEOUT 	0
// *****************  DIFFINT outputs  **********************
#define INTEGRATED 	4
#define SMOOTHED 	3
#define FST_DERIV 	2
#define	SCND_DERIV 	1
#define THRD_DERIV 	0
// ***************** POLAR outputs ******************
#define DELAYED2 	4
#define DELAYED3 	3
#define THETA 		2	// phi	atan(z,(sqrt(x^2 + y^2))
#define PHI 		1	// theta atan2(y,x)
#define NORM 		0	// sqrt (x^2 + y^2 + z^2)
// ***************** FFT outputs ******************
#define REALFFT 	0
#define IMAGFFT 	1
#define DELAYREALFFT 	2
#define DELAYIMAGFFT 	3
// ***************** MATH outputs ******************
#define SQRT 		4
#define LOG 		3
#define EXP 		2
#define	SIN 		1
#define RECIPROCAL 	0

// ***************** Analog outputs ******************
#define LEFTOUT 	0
#define RIGHTOUT 	1
#define AOUT 		2
#define BOUT 		3
#define	COUT 		4
#define DOUT	 	5

// ***************** EXTERN outputs ******************
#define ARG1 		0
#define ARG2 		1
#define ARG3 		2
#define ARG4 		3
#define	ARG5 		4
#define ARG6	 	5


// ***************** QTP outputs ******************
#define RAST_ENTROPY 0
#define MAX_ENTROPY 0 	// make no distinction at present


#define DIFFSET		1		// set Integrator to initial value
#define DIFFHOLD	-1		// hold integration to current value



// ***************** module parameters  **********************
#define MAX_MODULE_PARAMETERS_SIZE (6)
#define MAX_PRODUCT MAX_MODULE_PARAMETERS_SIZE

// ***************** Parameter assignments for modules **********************
// ***************** GENERAL **********************
#define INITIALISATION 0
#define PRM_INITIALISATION	"initial value"
#define CMP_INITIALISATION	"initialise cmprtr"
#define INITIALISATION2 1
#define CMP_INITIALISATION2	"initialise delay"

// ***************** POST **********************
#define POTSXINIT 			0
#define POTSXINIT_LBL		"init pot-x [0,1]"
#define POTSYINIT 			2
#define POTSYINIT_LBL		"init pot-y [0,1]"
#define POTSZINIT 			4
#define POTSZINIT_LBL		"init pot-z [0.1]"
#define POTSXENABLE 		1
#define POTSXENABLE_LBL		"[en][dis]able"
#define POTSYENABLE 		3
#define POTSYENABLE_LBL		"[en][dis]able"
#define POTSZENABLE 		5
#define POTSZENABLE_LBL		"[en][dis]able"

// ***************** FILTER **********************
#define FILTERTYPE 			0
#define FILTERTYPE_LBL		"IIR type [Ch Be Bu]"
#define PASSBAND 			1
#define PASSBAND_LBL		"band [Lp Bp Hp]"
#define RIPPLE 				2
#define RIPPLE_LBL			"Ch-ripple (-dB)"
#define CORNERFREQ1 		3
#define CORNERFREQ1_LBL		"corner1 [0-1]"
#define CORNERFREQ2 		4
#define CORNERFREQ2_LBL		"corner2 [0-1]"
#define GAIN 				5
#define GAIN_LBL			"gain"

// ***************** EXTRENAL PORTS **********************
#define EXTERNINADDR	 	0
#define EXTERNINADDR_LBL	"Dest IP"
#define EXTERNINPORT 		1
#define EXTERNINPORT_LBL	"Dest Port"
#define EXTERNOUTADDR 		2
#define EXTERNOUTADDR_LBL	"OSC address"
#define EXTERNOUTIP 		3
#define EXTERNOUTIP_LBL		"Opt. text arg_3"
#define EXTERNOUTPORT 		4
#define EXTERNOUTPORT_LBL	"Opt. text arg_4"
#define EXTERNTEXTARG 		5
#define EXTERNTEXTARG_LBL	"Opt. text arg_5"

// ***************** FUNCTION/DELAY **********************
#define DELAYMODE 			0
#define DELAYMODE_LBL		"[delay][func][FIR]"
#define DELFUNCFILE			1
#define DELFUNCFILE_LBL		"File extension"			// allows a file to be loaded with a precalculated function

// ***************** TRACE **********************
#define TRACETIMER 0
#define TRACETIMER_LBL		"time-axis [xyz][-]"
#define TRACECOLOUR 1
#define TRACECOLOUR_LBL		"colour [rgbcmyWB]"
#define TRACEVISIBLE 2
#define TRACEVISIBLE_LBL	"[en][dis]able"
#define TRACESTATIC 3
#define TRACESTATIC_LBL		"[static][dynamic]"
#define TRACELINETYP 4
#define TRACELINETYP_LBL	"line type [-_.|tT]"
#define TRACELINEWGHT 5
#define TRACELINEWGHT_LBL	"line weight"

// ***************** Paramters for the entropy calculation *******************
#define WINDOW			0		// window width
#define WINDOW_SHIFT	1		// window step size
#define START_PART 		2		// start of partition steps
#define STOP_PART 		3		// end of partition steps
#define COUNTER 		4		// counting off the steps

#define PRM_WINDOW			"window width"
#define PRM_WINDOW_SHIFT	"window step"
#define PRM_START_PART		"partition bgn"
#define PRM_STOP_PART		"partition end"
#define PRM_COUNTER 		"step counter"

// display format for wired connections
#define	ID		0
#define	NAME	1
#define	VALUE	2
#define	CCODE	3



#define RATESTEP (0.01)



// ******************  entropy structures and definitions ********************

#define MAX_STEPSAMPLE_INTERVAL 65536  // The maximum interval (2^16) for step information sample

typedef struct CODEWORDLIST  // match lists links same codewords based on IDs. string list links all codewords
{  
    unsigned long CodeWordID;
    struct CODEWORDLIST *NextCodeWordInMatchList;  
    struct CODEWORDLIST *PreviousCodeWordInMatchList;    
    struct CODEWORDLIST *NextCodeWordInStringList;    
    struct CODEWORDLIST *PreviousCodeWordInStringList;  
    
}CODEWORD; 

typedef struct MATCHLISTENTRY_DEF  // entry list stores heads of all match lists. These are dummy heads, no real data is stored here
{  
    unsigned long CodeWordID;
    struct CODEWORDLIST *NextCodeWordInMatchList;   
    struct CODEWORDLIST *PreviousCodeWordInMatchList;
    
}MATCHLISTENTRY;

#define EMPTY -1

#define MAXIT 100
#define EULER 0.5772156649
#define FPMIN 5.0e-308
#define EPS 5.0e-16






/*********************************************/
//build tags
#define TAG_COMMENT			"Comment"

#define TAG_INPT_LABEL		"Input_label"  				// class label
#define TAG_INPT_CHAN_NO 	"Input_channel_no"
#define TAG_INPT_NAME 		"Input_outputname"

#define TAG_POT_LABEL		"Pot_label"
#define TAG_POT_CHAN_NO		"Pot_channel_no"
#define TAG_POT_OUTX		"Pot_out_x"
#define TAG_POT_OUTY		"Pot_out_y"
#define TAG_POT_OUTZ		"Pot_out_z"
#define TAG_POT_SELX		"Pot_sel_x"
#define TAG_POT_SELY		"Pot_sel_y"
#define TAG_POT_SELZ		"Pot_sel_z"

#define TAG_RND_LABEL		"Rand_mod_label"
#define TAG_RND_BUILD_SIZE	"Random_build_size"
#define TAG_RND_OUTPUT		"Random_outputname"

#define TAG_OUTPT_LABEL		"Output_label" 				// class label
#define TAG_OUTPT_CHNL_NO	"Output_channel_no"
#define TAG_OUTPT_NAME		"Output_inputname"

#define TAG_CNXN_LABEL		"Connection_mod_label"  	// class label
#define TAG_CNXN_BUILD_SIZE "Connection_build_size"
#define TAG_CNXN_LUG 		"Connection_lug"


#define TAG_SCP_LABEL		"Scope_mod_label"
#define TAG_SCP_CHAN_NO		"Scope_channel_no"
#define TAG_SCP_XINPUT		"Scope_x_inputname"
#define TAG_SCP_YINPUT		"Scope_y_inputname"
#define TAG_SCP_ZINPUT		"Scope_z_inputname"

#define TAG_SCP_ORIENTA		"Scope_orient_a"
#define TAG_SCP_ORIENTB		"Scope_orient_b"

#define TAG_SYS_PAUSE		"System_pause"	
#define TAG_SYS_RESET		"System_reset"		
#define TAG_SYS_STOP		"System_stop"		

#define TAG_DLY_MOD_LABEL	"Delay_mod_label"
#define TAG_DLY_BUILD_SIZE	"Delay_build_size"
#define TAG_DLY_INPUT		"Delay_inputname"
#define TAG_DLY_OUTPUT		"Delay_outputname"
#define TAG_DLY_IN_SH		"Delay_reset_hold"

#define TAG_ID_MOD_LABEL	"ID_mod_label"  			// class label
#define TAG_ID_BUILD_SIZE	"ID_build_size"
#define TAG_ID_OUT_A		"a_out"
#define TAG_ID_OUT_B		"b_out"
#define TAG_ID_OUT_C		"c_out"
#define TAG_ID_OUT_D		"d_out"
#define TAG_ID_OUT_E		"e_out"
#define TAG_ID_OUT_F		"f_out"
#define TAG_ID_IN_I			"label_i"
#define TAG_ID_IN_J			"label_j"
#define TAG_ID_IN_SH		"ID_reset_hold"

#define TAG_ENT_BUILD_SIZE	"Ent_build_size"
#define TAG_ENT_MOD_LABEL	"Ent_mod_label" 			// class label
#define TAG_ENT_OUT_DELAYED	"Ent_delayed"
#define TAG_ENT_OUT_ENTROPY	"Ent_out"
#define TAG_ENT_INPUT		"Ent_in"

#define TAG_COMP_BUILD_SIZE	"Comparator_build_size" 	// comparator build size
#define TAG_COMP_MOD_LABEL	"Comp_mod_label" 			// class label
#define TAG_COMP_INPUT		"Comp_in"					// comparator in
#define TAG_COMP_POS		"Comparator_out"			// comparator pos
#define TAG_COMP_ZERO		"Comparator_zero"			// comparator zero
#define TAG_COMP_NONPOS		"Comparator_nonpos"			// comparator nonpos
#define TAG_COMP_NEG		"Comparator_neg"			// comparator neg
#define TAG_COMP_NONNEG		"Comparator_nonneg"			// comparator nonneg
#define TAG_COMP_DELAYED	"Comparator_delayed" 		// delayed 

#define TAG_FFT_MOD_LABEL	"FFT_mod_label" 			// class label
#define TAG_FFT_BUILD_SIZE	"fft_build_size"
#define TAG_FFT_INPUTX		"FFT_x_in"
#define TAG_FFT_INPUTY		"FFT_y_in"
#define TAG_FFT_SYNC		"FFT_sync"
#define TAG_FFT_REALOUT		"fft_real_outputname"
#define TAG_FFT_IMAGOUT		"fft_complex_outputname"
#define TAG_FFT_DELAYEDX	"fft_delayed_x_out"
#define TAG_FFT_DELAYEDY	"fft_delayed_y_out"

#define TAG_FIR_MOD_LABEL	"FIR_mod_label" 			// class label
#define TAG_FIR_BUILD_SIZE	"Filter_build_size"	
#define TAG_FIR_OUT_DELAYED	"filter_delayed"
#define TAG_FIR_OUT			"filter_out"
#define TAG_FIR_INPUT		"Fir_in"

#define TAG_PLR_MOD_LABEL	"Polar_mod_label"
#define TAG_PLR_BUILD_SIZE	"Polar_build_size"
#define TAG_PLR_XINPUT		"Polar_x_inputname"
#define TAG_PLR_YINPUT		"Polar_y_inputname"
#define TAG_PLR_ZINPUT		"Polar_z_inputname"
#define TAG_PLR_THETA		"Polar_theta_outputname"		//theta
#define TAG_PLR_PHI			"Polar_phi_outputname"		//phi
#define TAG_PLR_NORM		"Polar_norm_outputname"
#define TAG_PLR_XDELAY		"Polar_delayed_x"
#define TAG_PLR_YDELAY		"Polar_delayed_y"
#define TAG_PLR_ZDELAY		"Polar_delayed_z"

#define TAG_AUDIO_BUILD_SIZE		"Audio_build_size" 			// audio build size
#define TAG_AUDIO_MOD_LABEL	"Audio_mod_label" 			// audio module name
#define TAG_AUDIO_INL		"Audio_in_L" 				// audio left in
#define TAG_AUDIO_INR		"Audio_in_R"				// audio right in
#define TAG_AUDIO_OUT_OPTN	"Audio_out-optn"			// audio output options
#define TAG_AUDIO_OUTL		"Audio_out_L" 				// audio left out
#define TAG_AUDIO_OUTR		"Audio_out_R" 				// audio right out
#define TAG_AUDIO_OUTA		"Audio_out_A" 				// audio a out
#define TAG_AUDIO_OUTB		"Audio_out_B" 				// audio b out
#define TAG_AUDIO_OUTC		"Audio_out_C" 				// audio c out
#define TAG_AUDIO_OUTD		"Audio_out_D" 				// audio d out

#define TAG_MATH_BUILD_SIZE	"Math_build_size" 			// math build size
#define TAG_MATH_MOD_LABEL	"Math_mod_label" 			// class label
#define TAG_MATH_INPUT		"Math_in"					// Math in
#define TAG_MATH_SQRT		"Math_sqrt_out"				// Math sqrt
#define TAG_MATH_LOG		"Math_log_out"				// Math log
#define TAG_MATH_EXP		"Math_exp_out"				// Math exp
#define TAG_MATH_SIN		"Math_sin_out"				// Math sin
#define TAG_MATH_RECIP		"Math_recip_out"			// Math recip
#define TAG_MATH_DELAYED	"Math_delayed_out" 			// delayed output

#define TAG_EXTRN_BUILD_SIZE "EXT_build_size"
#define TAG_EXTRN_MOD_LABEL	"EXT_mod_label" 			// class label
#define TAG_EXTRN_ARGX		"Ext_arg_x" 				// first input
#define TAG_EXTRN_ARGY		"Ext_arg_y"					// second input
#define TAG_EXTRN_PUSHZ		"Ext_push_z"				// third input
#define TAG_EXTRN_ARG1		"Ext_arg_1"					// first output
#define TAG_EXTRN_ARG2		"Ext_arg_2"					// second output
#define TAG_EXTRN_ARG3		"Ext_arg_3"					// third output
#define TAG_EXTRN_ARG4		"Ext_arg_4"					// fourth output
#define TAG_EXTRN_ARG5		"Ext_arg_5"					// fifth output
#define TAG_EXTRN_ARG6		"Ext_arg_6" 				// sixth output


#define TAG_EXT_BUILD		"Extension_Build" 			// build extension
#define TAG_EXT_INOT		"Extension_IO" 				// Input/output extension
#define TAG_EXT_PREST		"Extension_Presets" 		// presets extension
#define TAG_EXT_INIT		"Extension_Initialisation"	// initialisation extension
#define TAG_EXT_CIRC		"Extension_Circuitry" 		// circuitry extension
#define TAG_EXT_FILT		"Extension_Filter" 			// filter file extension
#define TAG_EXT_DISP		"Extension_Display" 			// display file extension
#define TAG_EXT_VIEW		"Extension_Views" 			// view file extension

#define TAG_RATE			"Rate"						// set sample rate

#define TAG_AXIS_LIGHT_LOCK	"Axis_Light_Lock" 			// Graticule Axis Light Lock
#define TAG_AXIS_LOCK		"Axis_Lock" 				// Axis Orientation  Lock
#define TAG_MKRS_LIGHT_LOCK	"Markers_Lock" 				// Markers Light Lock
#define TAG_SFCE_LIGHT_LOCK	"Surface_Lock" 				// Surface Light Lock



#define	STR_C1			"C1" 
#define	STR_C2			"C2"
#define	STR_C3			"C3"
#define	STR_C4			"C4"
#define	STR_C5			"C5"
#define	STR_C6			"C6"
#define	STR_C7			"C7"
			
#define	STR_G1			"G1"							// Graticule Illuminiation YZ
#define STR_G2			"G2"							// Graticule Illuminiation XZ
#define STR_G3			"G3"							// Graticule Illuminiation XY
#define STR_G4			"G4"							// Graticule Span X
#define STR_G5			"G5"							// Graticule Span Y
#define STR_G6			"G6"							// Graticule Span Z
#define STR_G7			"G7"							// Graticule Offset X
#define STR_G8			"G8"							// Graticule Offset Y
#define STR_G9			"G9"							// Graticule Offset Z
#define STR_G10			"G10"							// Graticule Colour Red
#define STR_G11			"G11"							// Graticule Colour Green
#define STR_G12			"G12"							// Graticule Colour Blue
#define STR_G13			"G13"							// Graticule linlog X
#define STR_G14			"G14"							// Graticule linlog Y
#define STR_G15			"G15"							// Graticule linlog Z


#define STR_G_Origin	"Origin_Position" 				// Graticule left grid origin left/centre
#define STR_G_Labels	"Axis_Labels" 					// Graticule axis labels

/*
#define STR_I1			"I1"
#define STR_I2			"I2"
#define STR_I3			"I3"
#define STR_I4			"I4"
#define STR_I5			"I5"
*/
		
#define STR_L1			"L1"							// Surface Light angle 1
#define STR_L2			"L2"							// Surface Light angle 2
#define STR_L3			"L3"							// Markers Light angle 1
#define STR_L4			"L4"							// Markers Light angle 2
#define STR_L5			"L5"							// Graticule Light angle 1
#define STR_L6			"L6"							// Graticule Light angle 2

/*
#define STR_M1			"M1"
#define STR_M2			"M2"
#define STR_M3			"M3"
#define STR_M4			"M4"
*/

#define STR_O1			"O1"							// Axis Orientation angle 1
#define STR_O2			"O2"							// Axis Orientation angle 2

/*
#define STR_P1			"P1"
#define STR_P2			"P2"
#define	STR_P3			"P3"
#define STR_P4			"P4"

#define STR_VIEWNUMBER	"number"

#define	STR_SELECT_ENBLE "o_enable"
#define	STR_OG1			"og1"
#define STR_OG2			"og2"
#define STR_OG3			"og3"
#define STR_OG4			"og4"
#define STR_OG5			"og5"
#define STR_OG6			"og6"
#define STR_OG7			"og7"
#define STR_OG8			"og8"
#define	STR_OG9			"og9"
#define STR_OG10		"og10"

#define STR_OS1			"os1"
#define	STR_OS2			"os2"
#define STR_OS3			"os3"
#define STR_OS4			"os4"
#define STR_OS5			"os5"
#define STR_OS6			"os6"
#define STR_OS7			"os7"
#define STR_OS8			"os8"
#define STR_OS9			"os9"
#define STR_OS10		"os10"

#define STR_DELAY		"Delay" 
#define STR_DURATION	"Duration"
*/
#define STR_ORIGIN_X	"Origin_X"
#define STR_ORIGIN_Y	"Origin_Y"
#define STR_DIMENSION_X	"Width_X"
#define STR_DIMENSION_Y	"Width_Y"
#define STR_TITLE		"Title"
/*
#define STR_STRMNME 	"Stream_filename"
#define STR_VIEWNME		"Views_filename"
*/

#define STR_SURFACE_RED		"surface_red"
#define STR_SURFACE_GREEN	"surface_green"
#define STR_SURFACE_BLUE	"surface_blue"
#define STR_SURFACE_INTSTY	"surf_intensity"
#define STR_SURFACE_SHADOW	"surf_shadow"




// build assigns
// starts from 0
#define INPUT_LABEL			0
#define INPUT_CHNL_NO		1
#define INPUT_NAME			2


#define RND_LABEL			5
#define RND_CHNL_NO			6
#define RND_NAME			7

#define	OUTPUT_LABEL		10
#define OUTPUT_CHNL_NO		11
#define OUTPUT_NAME			12

#define CNXN_LABEL			15
#define CNXN_BUILD_SIZE		16
#define CNXN_LUG			17

#define POT_LABEL			20
#define POT_CHNL_NO			21
#define POT_OUTPUTX			22
#define POT_OUTPUTY			23	
#define POT_OUTPUTZ			24
#define POT_SELECTX			25
#define POT_SELECTY			26	
#define POT_SELECTZ			27

#define SCP_LABEL			30
#define SCP_CHAN_NO			31
#define SCP_XINPUT			32
#define SCP_YINPUT			33
#define SCP_ZINPUT			34
#define SCP_ORIENTA			35
#define SCP_ORIENTB			36

#define DLY_MOD_LABEL		40
#define DLY_BUILD_SIZE		41
#define DLY_INPUT			42
#define DLY_OUTPUT			43
#define DLY_IN_SH			44


#define SYSTEM_STOP			45
#define SYSTEM_PAUSE		46
#define SYSTEM_RESET		47

#define SYSTEM_RATE			48



#define ID_BUILD_SIZE		50
#define ID_MOD_LABEL		51
#define ID_OUT_A			52 // third deriv
#define ID_OUT_B			53 // second deriv
#define ID_OUT_C			54 // first deriv
#define ID_OUT_D			55 // smoothed
#define ID_OUT_E			56 // delayed
#define ID_OUT_F			57 // integrated
#define ID_IN_I				58
#define ID_IN_J				59
#define ID_IN_SH			60

#define ENT_BUILD_SIZE		65
#define ENT_MOD_LABEL		66
#define ENT_OUT_DELAYED		67
#define ENT_INPUT			68
#define ENT_OUT_ENTROPY		69

#define COMP_BUILD_SIZE		70
#define COMP_MOD_LABEL		71
#define COMP_DELAYED		72
#define COMP_POS			73
#define COMP_NONPOS			74
#define COMP_ZERO			75
#define COMP_NEG			76
#define COMP_NONNEG			77
#define COMP_INPUT			78

#define FFT_MOD_LABEL		80			
#define FFT_BUILD_SIZE		81
#define FFT_INPUTX			82
#define FFT_INPUTY			83
#define FFT_SYNC			84
#define FFT_REALOUT			85
#define FFT_IMAGOUT			86
#define FFT_DELAYEDX		87
#define FFT_DELAYEDY		88

#define FIR_BUILD_SIZE		90
#define FIR_MOD_LABEL		91
#define FIR_OUT_DELAYED		92
#define FIR_OUT				93
#define FIR_INPUT			94

#define PLR_MOD_LABEL		100
#define PLR_BUILD_SIZE		101
#define PLR_XINPUT			102
#define PLR_YINPUT			103
#define PLR_ZINPUT			104
#define PLR_THETA			105
#define PLR_PHI				106
#define PLR_NORM			107
#define PLR_XDELAY			108
#define PLR_YDELAY			109
#define PLR_ZDELAY			110

#define AUDIO_BUILD_SIZE 	119
#define AUDIO_MOD_LABEL		120
#define AUDIO_INPUTL		121
#define AUDIO_INPUTR		122
#define AUDIO_OUT_OPT		123
#define AUDIO_OUTPUTL		124
#define AUDIO_OUTPUTR		125
#define AUDIO_OUTPUTA		126
#define AUDIO_OUTPUTB		127
#define AUDIO_OUTPUTC		128
#define AUDIO_OUTPUTD		129

#define MATH_BUILD_SIZE		130
#define MATH_MOD_LABEL		131
#define MATH_DELAYED		132
#define MATH_SQRT			133
#define MATH_LOG			134
#define MATH_EXP			135
#define MATH_SIN			136
#define MATH_RECIP			137
#define MATH_INPUT			138

#define EXTRN_MOD_LABEL		140
#define EXTRN_BUILD_SIZE	141
#define EXTRN_ARGX			142
#define EXTRN_ARGY			143
#define EXTRN_PUSHZ			144
#define EXTRN_ARG1			145
#define EXTRN_ARG2			146
#define EXTRN_ARG3			147
#define EXTRN_ARG4			148
#define EXTRN_ARG5			149
#define EXTRN_ARG6			150

#define EXT_BILD			160
#define EXT_INOT			161
#define EXT_PREST			162
#define EXT_INIT			163
#define EXT_CIRC			164
#define EXT_FILT			165
#define EXT_DISP			166
#define EXT_VIEW			167

#define		ORIGIN_X		170
#define		ORIGIN_Y		171
#define		DIMENSION_X		172
#define		DIMENSION_Y		173
#define		TITLE			174

#define		C1				180
#define		C2				181
#define		C3				182
#define		C4				183
#define		C5				184
#define		C6				185
#define		C7				186
#define		G1				187
#define		G2				188
#define		G3				189
#define		G4				190
#define		G5				191
#define		G6				192
#define		G7				193
#define		G8				194
#define		G9				195
#define		G10				196
#define		G11				197
#define		G12				198

#define		G13				199
#define		G14				200
#define		G15				201


#define		G_Origin		202		// center origin
#define		G_Labels		203		// axis labels
#define		L1				204
#define		L2				205
#define		L3				206
#define		L4				207
#define		L5				208
#define		L6				209
#define		O1				210
#define		O2				211

#define		LOCK_AXIS		212
#define		LOCK_AXIS_LIGHT	213
#define		LOCK_SFCE_LIGHT	214
#define		LOCK_MKRS_LIGHT	215

#define		SURFACE_RED		220
#define		SURFACE_GREEN	221
#define		SURFACE_BLUE	222
#define		SURFACE_INTSTY	223
#define		SURFACE_SHADOW	224


/*********************************************/
//  modules tags

#define	TAG_MOD_ID			"mod_No"
#define	TAG_MOD_TYPE		"Type"
#define	TAG_MOD_LABEL		"Label"
  
#define	TAG_MOD_PAR1_ID		"P_ID 1"
#define	TAG_MOD_PAR1_LABEL	"P_Label 1"
#define	TAG_MOD_PAR1_INT	"P_int 1"
#define	TAG_MOD_PAR1_FLOAT	"P_float 1"

#define	TAG_MOD_PAR2_ID		"P_ID 2"
#define	TAG_MOD_PAR2_LABEL	"P_Label 2"
#define	TAG_MOD_PAR2_INT	"P_int 2"
#define	TAG_MOD_PAR2_FLOAT	"P_float 2"

#define	TAG_MOD_PAR3_ID		"P_ID 3"
#define	TAG_MOD_PAR3_LABEL	"P_Label 3"
#define	TAG_MOD_PAR3_INT	"P_int 3"
#define	TAG_MOD_PAR3_FLOAT	"P_float 3"

#define	TAG_MOD_PAR4_ID		"P_ID 4"
#define	TAG_MOD_PAR4_LABEL	"P_Label 4"
#define	TAG_MOD_PAR4_INT	"P_int 4"
#define	TAG_MOD_PAR4_FLOAT	"P_float 4"

#define	TAG_MOD_PAR5_ID		"P_ID 5"
#define	TAG_MOD_PAR5_LABEL	"P_Label 5"
#define	TAG_MOD_PAR5_INT	"P_int 5"
#define	TAG_MOD_PAR5_FLOAT	"P_float 5"

#define	TAG_MOD_PAR6_ID		"P_ID 6"
#define	TAG_MOD_PAR6_LABEL	"P_Label 6"
#define	TAG_MOD_PAR6_INT	"P_int 6"
#define	TAG_MOD_PAR6_FLOAT	"P_float 6"

#define	TAG_MOD_BUFF_SZ		"Buffer_size"


// presets settings
#define MOD_START		(LOCK_MKRS_LIGHT + 1)
#define MOD_ID			(0 + MOD_START)
#define MOD_TYPE		(1 + MOD_START)
#define MOD_LABEL		(2 + MOD_START)

#define MOD_PAR1_ID		(3 + MOD_START)
#define MOD_PAR2_ID		(4 + MOD_START)
#define MOD_PAR3_ID		(5 + MOD_START)
#define MOD_PAR4_ID		(6 + MOD_START)
#define MOD_PAR5_ID		(7 + MOD_START)
#define MOD_PAR6_ID		(8 + MOD_START)

#define MOD_PAR1_LABEL	(9 + MOD_START)
#define MOD_PAR2_LABEL	(10 + MOD_START)
#define MOD_PAR3_LABEL	(11 + MOD_START)
#define MOD_PAR4_LABEL	(12 + MOD_START)
#define MOD_PAR5_LABEL	(13 + MOD_START)
#define	MOD_PAR6_LABEL	(14 + MOD_START)

#define MOD_PAR1_INT	(15 + MOD_START)
#define MOD_PAR2_INT	(16 + MOD_START)
#define MOD_PAR3_INT	(17 + MOD_START)
#define MOD_PAR4_INT	(18 + MOD_START)
#define MOD_PAR5_INT	(19 + MOD_START)
#define MOD_PAR6_INT	(20 + MOD_START)

#define MOD_PAR1_FLOAT	(21 + MOD_START)
#define MOD_PAR2_FLOAT	(22 + MOD_START)
#define MOD_PAR3_FLOAT	(23 + MOD_START)
#define MOD_PAR4_FLOAT	(24 + MOD_START)
#define MOD_PAR5_FLOAT	(25 + MOD_START)
#define MOD_PAR6_FLOAT	(26 + MOD_START)

#define MOD_BUFF_SZ		(27 + MOD_START)



/*********************************************/
// ionames tags

#define	TAG_IO_ID		"io_No"
#define	TAG_IO_TYPE		"Channel_class" // this is the io class name INPUT, OUTPUT, LUG as set in the build panel 
#define	TAG_IO_MOD_NAME	"Module_label"	// IO module label
#define	TAG_IO_LABEL	"Signal_label"  // this carries the signal name
#define	TAG_IO_META		"Meta_info"

// ionames settings
#define IO_START		(MOD_BUFF_SZ+1)
#define IO_ID			(0 + IO_START)
#define IO_TYPE			(1 + IO_START)
#define IO_MODULE		(2 + IO_START)
#define IO_LABEL		(3 + IO_START)
#define IO_META			(4 + IO_START)

// io type
#define INPUTCHAN 		1
#define	OUTPTCHAN		2




/*********************************************/
// connection tags

#define	TAG_CON_NO				"Connection_No"
#define	TAG_CON_INPUT_NO		"Input_No"
#define	TAG_CON_INPUT_MOD_LBL	"Input_module"
#define	TAG_CON_INPUT_NAME		"Input_name"
#define	TAG_CON_INPUT_LABLE		"Input_label" // signal name

#define	TAG_CON_OFFSET			"Offset"
#define	TAG_CON_WEIGHT			"Weight"

#define	TAG_CON_OUTPUT1_NO		"Output1_No"
#define	TAG_CON_OUTPUT2_NO		"Output2_No"
#define	TAG_CON_OUTPUT3_NO		"Output3_No"
#define	TAG_CON_OUTPUT4_NO		"Output4_No"
#define	TAG_CON_OUTPUT5_NO		"Output5_No"
#define	TAG_CON_OUTPUT6_NO		"Output6_No"

#define	TAG_CON_OUTPUT1_MOD_LBL	"Output1_module"
#define	TAG_CON_OUTPUT2_MOD_LBL	"Output2_module"
#define	TAG_CON_OUTPUT3_MOD_LBL	"Output3_module"
#define	TAG_CON_OUTPUT4_MOD_LBL	"Output4_module"
#define	TAG_CON_OUTPUT5_MOD_LBL	"Output5_module"
#define	TAG_CON_OUTPUT6_MOD_LBL	"Output6_module"

#define	TAG_CON_OUTPUT1_NAME	"Output1_name"
#define	TAG_CON_OUTPUT2_NAME	"Output2_name"
#define	TAG_CON_OUTPUT3_NAME	"Output3_name"
#define	TAG_CON_OUTPUT4_NAME	"Output4_name"
#define	TAG_CON_OUTPUT5_NAME	"Output5_name"
#define	TAG_CON_OUTPUT6_NAME	"Output6_name"

#define	TAG_CON_OUTPUT1_LBL		"Output1_label"
#define	TAG_CON_OUTPUT2_LBL		"Output2_label"
#define	TAG_CON_OUTPUT3_LBL		"Output3_label"
#define	TAG_CON_OUTPUT4_LBL		"Output4_label"
#define	TAG_CON_OUTPUT5_LBL		"Output5_label"
#define	TAG_CON_OUTPUT6_LBL		"Output6_label"


// connection settings
#define	CON_NO				0 // connection number 

#define	CON_INPUT_NO		1
#define	CON_INPUT_MOD_LBL	2
#define	CON_INPUT_NAME		3
#define	CON_INPUT_LABLE		4

#define	CON_OFFSET			5
#define	CON_WEIGHT			6

#define	CON_OUTPUT1_NO		10
#define	CON_OUTPUT2_NO		11
#define	CON_OUTPUT3_NO		12
#define	CON_OUTPUT4_NO		13
#define	CON_OUTPUT5_NO		14
#define	CON_OUTPUT6_NO		15

#define	CON_OUTPUT1_MOD_LBL	20
#define	CON_OUTPUT2_MOD_LBL	21
#define	CON_OUTPUT3_MOD_LBL	22
#define	CON_OUTPUT4_MOD_LBL	23
#define	CON_OUTPUT5_MOD_LBL	24
#define	CON_OUTPUT6_MOD_LBL	25

#define	CON_OUTPUT1_NAME	30
#define	CON_OUTPUT2_NAME	31
#define	CON_OUTPUT3_NAME	32
#define	CON_OUTPUT4_NAME	33
#define	CON_OUTPUT5_NAME	34
#define	CON_OUTPUT6_NAME	35

#define	CON_OUTPUT1_LBL		40
#define	CON_OUTPUT2_LBL		41
#define	CON_OUTPUT3_LBL		42
#define	CON_OUTPUT4_LBL		43
#define	CON_OUTPUT5_LBL		44
#define	CON_OUTPUT6_LBL		45



/*********************************************/
// Potentiometer tags

#define	TAG_POT_BANK_NAME	"Number" // is really the pot bank name

#define	TAG_POT_INPUT1		"Potentiometer1"
#define	TAG_POT_INPUT2		"Potentiometer2"
#define	TAG_POT_INPUT3		"Potentiometer3"

#define	TAG_POT_SELECT1		"Select1"
#define	TAG_POT_SELECT2		"Select2"
#define	TAG_POT_SELECT3		"Select3"


// Potentiameter assignments
#define	POT_BANK_NAME		0 // is really the pot bank name

#define	POT_INPUT1			1
#define	POT_INPUT2			2
#define	POT_INPUT3			3

#define	POT_SELECT1			4
#define	POT_SELECT2			5
#define	POT_SELECT3			6

/**** trace stuff *****/

#define MAX_GRAPHS	100


#define BUFFER 			0
#define CHANNEL			1
#define GRAPH_COLOUR	2
#define GRAPH_LABEL		3
#define GRAPH_STATIC	4
#define GRAPH_VISIBLE 	5

#define LINE_SIZE		6
#define LINE_TYPE		7

#define X_SCALE			8
#define Y_SCALE			9	
#define Z_SCALE			10

#define X_OFFSET		11
#define Y_OFFSET		12
#define Z_OFFSET		13

#define	MARKER_LABEL	28

#define	MARKER_COLOUR	14
#define	MARKER_VISIBLE	15
#define	MARKER_SIZE		16
#define	MARKER_TYPE		17

#define	MK_VISIBLE		18

#define MK_X_POSITION	19
#define MK_Y_POSITION	20
#define MK_Z_POSITION	21

#define MK_X_SCALE		22
#define MK_Y_SCALE		23
#define MK_Z_SCALE		24

#define MK_X_OFFSET		25
#define MK_Y_OFFSET		26
#define MK_Z_OFFSET		27


#define STR_KEY			"key"
#define STR_STR			"string"
#define STR_REAL		"real"
#define	STR_TRUE		"true"
#define STR_FALSE		"false"





/*********************************************/

#define DEFAULT_PRESETS_SZ 		0
#define DEFAULT_DELY_BUFF_SZ 	0
#define DEFAULT_FIR_BUFF_SZ		1
#define DEFAULT_COMP_BUFF_SZ	0
#define DEFAULT_DIFINT_BUFF_SZ	1 
#define DEFAULT_MATH_BUFF_SZ	0
#define DEFAULT_ENT_BUFF_SZ		1024 // same as default fft
#define DEFAULT_FFT_BUFF_SZ		1024
#define DEFAULT_EXTRN_BUFF_SZ	0 
#define DEFAULT_TRACE_BUFF_SZ	1000 // trace buffer and number of traces will be set here
#define DEFAULT_PLR_BUFF_SZ		0
#define DEFAULT_NOISE_BUFF_SZ	0
#define DEFAULT_AUDIO_BUFF_SZ	0

#define BUFFERMULTIPLIER		(4)	// many of the devices need more buffer space than indicated by BUFF_SZ
#define BUFFERSPLIT				(2)	// 


#define BUILD_STAGE				(1)
#define PRESETS_STAGE			(2)
#define IO_STAGE				(3)
#define	CONNECTIONS_STAGE		(4)
#define	CONTROL_STAGE			(5)

/*********************************************/
// pertecs data structures

struct pertecs_build {
	int	status;

	int		build_size;				// total build size

	char	Scope_mod_label[MAX_LABEL_LENG];	// module label
	char	Scope_orienta[MAX_LABEL_LENG];	// Scope orientation
	char	Scope_orientb[MAX_LABEL_LENG];	// 

	char 	System_mod_label[MAX_LABEL_LENG]; // module label
	char	System_pause[MAX_LABEL_LENG];	// system pause causes pertecs to suspend processing
	char	System_stop[MAX_LABEL_LENG];	// system stop causes pertecs to exit
	char	System_reset[MAX_LABEL_LENG];	// system reset causes pertecs to reinitialise and continue to process
	int		System_Sample_Rate;
	float	multiplier_Rate;					// multilier for the System_Sample_Rate

	int		Input_channel_no;
	char 	Input_label[MAX_LABEL_LENG];
	char 	Input_out_name[MAX_LABEL_LENG];

	int		Noise_channel_size;
	char 	Noise_mod_label[MAX_LABEL_LENG];
	char 	Noise_out_name[MAX_LABEL_LENG];

	int		Pot_channel_no;
	char 	Pot_label[MAX_LABEL_LENG];
	char 	Pot_out_namex[MAX_LABEL_LENG];
	char 	Pot_out_namey[MAX_LABEL_LENG];
	char 	Pot_out_namez[MAX_LABEL_LENG];
	char 	Pot_sel_namex[MAX_LABEL_LENG];
	char 	Pot_sel_namey[MAX_LABEL_LENG];
	char 	Pot_sel_namez[MAX_LABEL_LENG];


	int		Output_channel_no;
	char 	Output_label[MAX_LABEL_LENG];
	char 	Output_in_name[MAX_LABEL_LENG];

	int		Connection_build_size;
	char 	Connection_label[MAX_LABEL_LENG];
	char 	Connection_lug_name[MAX_LABEL_LENG];

	int		Trace_build_size;
	char 	Trace_mod_label[MAX_LABEL_LENG];
	char 	Trace_xin_name[MAX_LABEL_LENG];
	char 	Trace_yin_name[MAX_LABEL_LENG];
	char 	Trace_zin_name[MAX_LABEL_LENG];


	int		Delay_build_size;
	char 	Delay_mod_label[MAX_LABEL_LENG];
	char 	Delay_in_name[MAX_LABEL_LENG];
	char 	Delay_in_sethold_name[MAX_LABEL_LENG];
	char 	Delay_out_name[MAX_LABEL_LENG];

	int		ID_build_size;
	char 	ID_mod_label[MAX_LABEL_LENG];
	char 	ID_in_i[MAX_LABEL_LENG];
	char 	ID_in_j[MAX_LABEL_LENG];
	char 	ID_in_sethold[MAX_LABEL_LENG];
	char 	ID_out_a[MAX_LABEL_LENG];
	char 	ID_out_b[MAX_LABEL_LENG];
	char 	ID_out_c[MAX_LABEL_LENG];
	char 	ID_out_d[MAX_LABEL_LENG];
	char 	ID_out_e[MAX_LABEL_LENG];
	char 	ID_out_f[MAX_LABEL_LENG];
	
	int		Ent_build_size;
	char 	Ent_mod_label[MAX_LABEL_LENG];
	char 	Ent_in[MAX_LABEL_LENG];
	char 	Ent_out[MAX_LABEL_LENG];
	char 	Ent_delayed[MAX_LABEL_LENG];

	int		Comp_build_size;
	char 	Comp_mod_label[MAX_LABEL_LENG];
	char 	Comp_in[MAX_LABEL_LENG];
	char 	Comp_out[MAX_LABEL_LENG];
	char 	Comp_zero[MAX_LABEL_LENG];
	char 	Comp_nonpos[MAX_LABEL_LENG];
	char 	Comp_neg[MAX_LABEL_LENG];
	char 	Comp_nonneg[MAX_LABEL_LENG];
	char 	Comp_delayed[MAX_LABEL_LENG];

	int		FFT_build_size;
	char 	FFT_mod_label[MAX_LABEL_LENG];
	char 	FFT_inx[MAX_LABEL_LENG];
	char 	FFT_iny[MAX_LABEL_LENG];
	char 	FFT_sync[MAX_LABEL_LENG];
	char 	FFT_realout[MAX_LABEL_LENG];
	char 	FFT_imagout[MAX_LABEL_LENG];
	char 	FFT_delayedx[MAX_LABEL_LENG];
	char 	FFT_delayedy[MAX_LABEL_LENG];

	int		Filter_build_size;
	char 	Filter_mod_label[MAX_LABEL_LENG];
	char 	Filter_in[MAX_LABEL_LENG];
	char 	Filter_delayed[MAX_LABEL_LENG];
	char 	Filter_out[MAX_LABEL_LENG];

	int		Plr_build_size;
	char 	Plr_mod_label[MAX_LABEL_LENG];
	char 	Plr_xin[MAX_LABEL_LENG];
	char 	Plr_yin[MAX_LABEL_LENG];
	char 	Plr_zin[MAX_LABEL_LENG];
	char 	Plr_theta[MAX_LABEL_LENG];
	char 	Plr_phi[MAX_LABEL_LENG];
	char 	Plr_norm[MAX_LABEL_LENG];
	char 	Plr_xdelayed[MAX_LABEL_LENG];
	char 	Plr_ydelayed[MAX_LABEL_LENG];
	char 	Plr_zdelayed[MAX_LABEL_LENG];

	int		Audio_build_size;
	char 	Audio_mod_label[MAX_LABEL_LENG];
	char 	Audio_leftin[MAX_LABEL_LENG];
	char 	Audio_rightin[MAX_LABEL_LENG];
	char 	Audio_out_optn[MAX_LABEL_LENG];
	char 	Audio_leftout[MAX_LABEL_LENG];
	char 	Audio_rightout[MAX_LABEL_LENG];
	char 	Audio_aout[MAX_LABEL_LENG];
	char 	Audio_bout[MAX_LABEL_LENG];
	char 	Audio_cout[MAX_LABEL_LENG];
	char 	Audio_dout[MAX_LABEL_LENG];

	int		Math_build_size;
	char 	Math_mod_label[MAX_LABEL_LENG];
	char 	Math_in[MAX_LABEL_LENG];
	char 	Math_delayed[MAX_LABEL_LENG];
	char 	Math_sqrt[MAX_LABEL_LENG];
	char 	Math_log[MAX_LABEL_LENG];
	char 	Math_exp[MAX_LABEL_LENG];
	char 	Math_sin[MAX_LABEL_LENG];
	char 	Math_recip[MAX_LABEL_LENG];

	int		Extern_build_size;
	char 	Extern_mod_label[MAX_LABEL_LENG];
	char 	Extern_argx_label[MAX_LABEL_LENG];
	char 	Extern_argy_label[MAX_LABEL_LENG];
	char 	Extern_pushz_label[MAX_LABEL_LENG];
	char 	Extern_arg1_label[MAX_LABEL_LENG];
	char 	Extern_arg2_label[MAX_LABEL_LENG];
	char 	Extern_arg3_label[MAX_LABEL_LENG];
	char 	Extern_arg4_label[MAX_LABEL_LENG];
	char 	Extern_arg5_label[MAX_LABEL_LENG];
	char 	Extern_arg6_label[MAX_LABEL_LENG];

	char 	ext_build[MAX_LABEL_LENG];
	char 	ext_ionms[MAX_LABEL_LENG];
	char 	ext_prests[MAX_LABEL_LENG];
	char 	ext_init[MAX_LABEL_LENG];
	char 	ext_circ[MAX_LABEL_LENG];
	char 	ext_filter[MAX_LABEL_LENG];
	char 	ext_display[MAX_LABEL_LENG];
	char 	ext_views[MAX_LABEL_LENG];

// scope build
	float 					background_r			; // move the scope parameters into the pertecs machine stricture
	float					background_g			;
	float					background_b			;
	float					complement				; // complement the background settings

	int						Origin_X				;
	int						Origin_Y				;
	int						WIdth					;
	int						Height					;

// graphics
	float					xscale			;
	float					yscale			;
	float					zscale			;
	float					xoffset			;
	float					yoffset			;
	float					zoffset			;
	int						origin			;
	int						axis			;

	float					yzintensity		;
	float					xyintensity		;
	float					xzintensity		;
	float					rcolour			;
	float					gcolour			;
	float					bcolour			;
	float					langle1			;
	float					langle2			;

/*
// surface
	float					surface_red			;
	float					surface_green		;
	float					surface_blue		;
	float					surface_intsty		;
*/


};

struct module {
	int					module_no;										// each module is numbered, and the default name is formed from M#
	int					type;											// module class
	char				*name;											// module label as used in the build panel

	int					no_inputs;										// number of inputs
	struct field		*Input[MAX_MODULE_INPUTS_SIZE];					// node labels in each field

	int					no_outputs;										// number of outputs								
	struct field		*Output[MAX_MODULE_OUTPUTS_SIZE]; 				//

	int					no_parameters;
	struct field 		*ParamPtr[MAX_MODULE_PARAMETERS_SIZE];			// visible paramters personalised for each type of module 

	int					In_Ptr;											// buffer pointer
	int					buff_sz;										// buffer size
	double				*input_buffPtr;									// input data buffer pointer
	double				spare;											// try this for the integrators
																			
};


struct io_module { // simplified modules have the input and outputr as a single connection point

	int					module_no;										// each module is numbered, and the default name is formed from M#
	int					type;											// module class
	char				*name;											// connection label as used in the build panel

    union {
		struct field		*In;
		struct field		*Out;
	} put;																												
};


// need to create a module field for the i/o ? 
///  module number : id
// module node : integer
// defined a field for every nodule node.
struct field
{
	int		module_no;													// module_no
	int		module_node;												// specify module connection node
	int		type;														// module class (optional for use when field is in the i/o)
	char	*name;														// internal node in the field for identifying input / output or in case of i/o class

	char	*label;														//  signal label which may ultimately be used instead of the named connection port/pin.
	char	*metaref;													// optional meta data (could be a string reference to the metadata
	double	value;														// or a double value  ... the double value is the default for all module processing
	long	intvalue;													//  express the double as an integer value ... saves repetitive conversion 
};

// holds the maximum information for a single connection as a stack of fields...
// the last is the input
// the remainder are the outputs
struct stack
{
	int		pointer;
	double	offset, constant; // global
	struct	field	*Field[MAX_STACK_SIZE];								// array of pointers to a stack fields: input_module, node, offset, weight and up to six output_modules and nodes
};


/* structure to store a connection */
struct Node
{
	int					key;											// connection number (sort on this)
	struct field		*Input;											// input of device or output channel 
	double 				offset;											// input offset 
	double 				constant;										// input weight 
	char				*notes;											// pointer to save the notes associated with connection
	struct field		*Output[MAX_PRODUCT];							// may include a product of devices  // this is to be much smaller only 6
	struct Node			*PreviousNodePtr;
	struct Node			*leftPtr, *rightPtr;							// left and right child nodes
};


struct connection {
	int				Last_No;											// handle relative addressing with respect to last
	int				Last_Input_No;										// handle relative addressing with respect to last

	double			Last_offset;
	double			Last_weight;
	
	int				No;													// connection number
	int				input_No;											// input module number
	char			input_module[MAX_LABEL_LENG];
	char			input_label[MAX_LABEL_LENG];
	char			input_name[MAX_LABEL_LENG];
	
	struct	field	*InputPtr;
		
	int				output1_No;
	int				output2_No;
	int				output3_No;
	int				output4_No;
	int				output5_No;
	int				output6_No;

	char			output1_module[MAX_LABEL_LENG];
	char			output2_module[MAX_LABEL_LENG];
	char			output3_module[MAX_LABEL_LENG];
	char			output4_module[MAX_LABEL_LENG];
	char			output5_module[MAX_LABEL_LENG];
	char			output6_module[MAX_LABEL_LENG];

	char			output1_label[MAX_LABEL_LENG];
	char			output2_label[MAX_LABEL_LENG];
	char			output3_label[MAX_LABEL_LENG];
	char			output4_label[MAX_LABEL_LENG];
	char			output5_label[MAX_LABEL_LENG];
	char			output6_label[MAX_LABEL_LENG];

	char			output1_name[MAX_LABEL_LENG];
	char			output2_name[MAX_LABEL_LENG];
	char			output3_name[MAX_LABEL_LENG];
	char			output4_name[MAX_LABEL_LENG];
	char			output5_name[MAX_LABEL_LENG];
	char			output6_name[MAX_LABEL_LENG];

	struct	field	*OutputPtr;
	
	double			weight;
	double 			offset;
};






struct window {
	int		status							;
	char	Title[FILE_NAME_SIZE]			; // more space than for a label
	char	ViewFileName[FILE_NAME_SIZE]	; // more space than for a label
	char	StreamFileName[FILE_NAME_SIZE]	; // more space than for a label
	float	Delay							;
	float	Duration						;
	int		Position_x						;
	int		Position_y						;
	int		Dimension_x						;
	int		Dimension_y						;
};


struct envir {
	char	FileName[FILE_NAME_SIZE]		; // more space than for a label
	int		status							;
	int		ViewNo							;
	char	viewlabel[MAX_LABEL_LENG]		;
	float	Orientation[2]					;
	float	Lighting[6]						;
	float	Grid[12]						;
	float	Colour[7]						;
	float	Partition[5]					;
	int		Mode[3]							;
	float	Mode_4							;
	float	Index[5]						;
	int		obj_enable						;
	int		surf_obj[10]					;
	int		graph_obj[10]					;
};



// this is the core memory structure for the whole machine

struct xml_data {
	FILE	*				input_fid				; //Telemetry input id
	FILE	*				output_fid				; //Telemetry output id


	int						runtime_controls_flg	;
	int						openflg					;
	int						opencontrolsflg			;
	int						OSC_flg					;

// i/o 
	int						System_Rate				; //if non-zero... over-rides the build rate.
	int						rawinflg				; //set if raw data in
	int						rawoutflg				; //set if raw data out
	int						out_sampleflg			; //indicates upsampling and downsampling
	int						in_sampleflg			; //indicates upsampling and downsampling
	int 					out_resample			; //down sampling count
	int 					in_resample				; //up sampling count
	int						terminate_on_EOF		; // terminate if input =EOF
	
	int						buildflg				; //set if build is true
	int						debugflg				; //set if in debug mode
	int						suppressnamesflag		; //set if XML inputs to ignore the assigned names... retun the defualts in the debug files
	int						codeflag				; //set if coding output required
	
// xml parameters
	int						depth					;	// depth in the xml parsing hierarchy
	int						entry_counter			;   // keep track of the dict entires...
	int						status					;
	char					tag[XML_STR_SIZE]		;
	int						key						;
	char					content[XML_STR_SIZE]	; // this needs to be very large??? perhaps or make this a separate data structure... with large temporary buffer
	int						content_index			;
	int						channels				;

	
// build parameters
	struct pertecs_build 	build					; // build
	char					*config_name			; 

// temporary spaces	
	struct module			currentmodule			; // used as a temporary space in the collection of 
	struct io_module		currentio_module		; // used as a temporary space in the collection of 
	struct connection		currentconnection		;
	struct Node				*currenttree			;
	

// module spaces
	struct module 			*Modules[MAX_BUILD_SIZE];
	
// new i/o module space ... these are a reduced module
	struct io_module		*IO_Modules[2 * MAX_CHANNELS];			// here we try to separate out the label and name confusion to replace the IOframe
	struct io_module 		**Output_Modules;						// offset point into the previous array
	struct io_module 		**lug_Modules;							// offset point into the previous array
	float	 				mirror_IO_Modules[MAX_CHANNELS];
	float 					*mirror_Output_Modules;					// pointing to an array of float values
	float					*mirror_Lug_Modules;						// pointer to array of lug values
	
	
// i/o spaces
	struct field 			*Pot_Frame[ MAX_POTS]; 					// these are in triples allowing for 32 banks
	float					*frameoutPtr			; 				//points to telemetry output frame 
	
	
	float	 				*interpfilterPtr[MAX_CHANNELS]  ;		// enough to cover the input array 
	int						interp_size 			;
	float					*weightsPtr 			;
	int 					farray_index 			;

//d	float	 				mirror_IO_Frame[MAX_CHANNELS];
//d	float 					*mirror_Output_Frame;					// pointing to an array of float values
//d	float					*mirror_Lugs_Frame;						// pointer to array of lug values
	float		 			mirror_Pot_Frame[MAX_POTS]; 			//  pointer to array of pot values

// wiring and sampling
	struct Node 			*CircuitTree 			;				//  root node for circuit tree.
	struct Node 			*SamplingTree 			;				//  root node for output telemetry sampling. 	
	struct stack 			*mystackPtr				;
	
	int						configure_stage			;				
	int						run_mode				;				// START, STOP, CONTINUE
	int						thread_status			;
	int						stepincr				;
};





// ****************** external structures ********************

//extern void Init_External(struct xml_data *dataPtr);

extern	float *init_gsinc( int upsample, int no_zeros);
// ****************** pertecs structures ********************

extern int build(struct xml_data *DataPtr );
extern int ionames(struct xml_data *DataPtr);
extern int presets(struct xml_data *DataPtr);
extern int load_connections(struct xml_data *DataPtr );
extern void write_pots_file(struct xml_data *DataPtr, int flag);
extern	int potentiometers(struct xml_data *DataPtr );
extern void print_pheader(FILE *fd);
extern void print_pclose(FILE *fd);
/*********************************************/
//main
extern	struct xml_data	pertecs_data;
extern	int threadcount ;
extern long  iframe_counter, oframe_counter;

/*********************************************/
//init
extern FILE *Init_Input_Telemetry ();
extern FILE *Init_Output_Telemetry ();
extern void InitInputOutput(struct xml_data *DataPtr);
extern void InitInputOutputNames(struct xml_data *DataPtr);


/*********************************************/
//pertecs_trees
extern void	create( struct Node **curNodePtrPtr );
extern void	pushto(struct stack *stackPtr, struct field *Field);
extern struct field *pop(struct stack *stackPtr);

extern void set_offset(double number, struct stack *stackPtr);
extern void set_constant(double number, struct stack *stackPtr);

extern void	add_connection( struct Node *nodePtr, int key, struct stack *stackPtr );
extern void	Reconnection( struct Node *nodePtr, int key, struct stack *stackPtr );

extern void Disconnection( struct Node *nodePtr, int key);
extern void	Sampling(struct Node *nodePtr );

extern	void	freeTree(struct Node *nodePtr );

/*********************************************/
//pertecs_modules
extern	int Process_Module( struct module *Module[], struct pertecs_build *buildPtr, long time, int state);
extern	void Create_Module_Buffers(struct xml_data *dataPtr);
extern	void CopyModulePresets(struct xml_data *dataPtr);
extern void CopyToCurrent(struct module *modulePtr, struct module *currentmodule);
extern	void Create_Module_Space(struct xml_data *DataPtr);

extern double ftdSelAugment ( char *buffer, long bufflen, MATCHLISTENTRY *EntryList, CODEWORD *PrimeList);
extern double invlogint(double x);



extern void *Control(void *threadarg);
extern void mkfilter(struct xml_data pertecs_data);

//extern	void *NDD_monitor(void *threadarg);

#include "spod.h"


/*********************************************/
//trace


extern	SURF *surfacePtr[] ; // maximum number of surfaces
extern	LINE *curvePtr[] ; // maximum number of graphs
extern	BALL *ballPtr[] ; // maximum no of balls
extern	TXTLINE *textPtr[] ;  // maximum no of text lines
extern	JUMP *jumpPtr[];  // number of views

extern int helpmenu, textdisp;
extern int	xtextstart, ytextstart;


/*********************************************/
extern void ddataframe(char *address, char *hostDestIP, int port, int numarg, struct io_module **Output_Modules );
extern int portrecnumber;		/* port to receive parameter updates from */
extern int portsndnumber; // port to send parameter updates to
extern int portdoscnumber; // default for DD digital duisplay

