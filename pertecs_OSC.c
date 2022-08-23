/*
Adapted from code written by Matt Wright and Adrian Freed

 now adapting this to provide a user interface to PERTECS 

	compile:

cc -o pertecs_OSC pertecs_OSC.c htmsocket.c OSC_client.c OSC_timeTag.c

*/



#include "OSC_client.h"
#include "htmsocket.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <rpc/rpc.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/times.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pwd.h>
#include <signal.h>
#include <grp.h>
#include <sys/file.h>
#include <GLUT/glut.h>

#ifdef NEED_SCHEDCTL_AND_LOCK
#include <sys/schedctl.h>
#include <sys/lock.h>
#endif

#if defined(__APPLE__) && defined(__GNUC__)
#define OSX
#endif


char *htm_error_string;
typedef int Boolean;
typedef void *OBJ;



typedef struct ClientAddressStruct {
        struct sockaddr_in  cl_addr;
        int clilen;
        int sockfd;
} *ClientAddr;

typedef struct TypeArgStruct {
    enum {INT, FLOAT, STRING} type;
    union {
        int i;
        float f;
        char *s;
    } datum;
} typedArg;


#include "pertecs.h"



// default labels for controlabellist

char *controldevlabels[] = {
"X",
"Y",
"Z",
};


char *controllabellist[] = {
"/labelx[0]",
"/labely[0]",
"/labelz[0]",
"/labelx[1]",
"/labely[1]",
"/labelz[1]",
"/labelx[2]",
"/labely[2]",
"/labelz[2]",
"/labelx[3]",
"/labely[3]",
"/labelz[3]",
"/labelx[4]",
"/labely[4]",
"/labelz[4]",
"/labelx[5]",
"/labely[5]",
"/labelz[5]",
"/labelx[6]",
"/labely[6]",
"/labelz[6]",
"/labelx[7]",
"/labely[7]",
"/labelz[7]",
"/labelx[8]",
"/labely[8]",
"/labelz[8]",
"/labelx[9]",
"/labely[9]",
"/labelz[9]",
"/labelx[10]",
"/labely[10]",
"/labelz[10]",
"/labelx[11]",
"/labely[11]",
"/labelz[11]",
"/labelx[12]",
"/labely[12]",
"/labelz[12]",
"/labelx[13]",
"/labely[13]",
"/labelz[13]",
"/labelx[14]",
"/labely[14]",
"/labelz[14]",
"/labelx[15]",
"/labely[15]",
"/labelz[15]",
"/labelx[16]",
"/labely[16]",
"/labelz[16]",
"/labelx[17]",
"/labely[17]",
"/labelz[17]",
"/labelx[18]",
"/labely[18]",
"/labelz[18]",
"/labelx[19]",
"/labely[19]",
"/labelz[19]",
"/labelx[20]",
"/labely[20]",
"/labelz[20]",
"/labelx[21]",
"/labely[21]",
"/labelz[21]",
"/labelx[22]",
"/labely[22]",
"/labelz[22]",
"/labelx[23]",
"/labely[23]",
"/labelz[23]"
};

int N_label = sizeof controllabellist / sizeof *controllabellist;

char *controldevlist[] = {
"/rotaryx[0]", // strict order preserved
"/rotaryy[0]",
"/rotaryz[0]",
"/rotaryx[1]",
"/rotaryy[1]",
"/rotaryz[1]", 
"/rotaryx[2]", 
"/rotaryy[2]",
"/rotaryz[2]",
"/rotaryx[3]",
"/rotaryy[3]",
"/rotaryz[3]", 
"/rotaryx[4]", 
"/rotaryy[4]",
"/rotaryz[4]",
"/rotaryx[5]",
"/rotaryy[5]",
"/rotaryz[5]", 
"/rotaryx[6]", 
"/rotaryy[6]",
"/rotaryz[6]",
"/rotaryx[7]",
"/rotaryy[7]",
"/rotaryz[7]", 
"/rotaryx[8]", // strict order preserved
"/rotaryy[8]",
"/rotaryz[8]",
"/rotaryx[9]",
"/rotaryy[9]",
"/rotaryz[9]", 
"/rotaryx[10]", 
"/rotaryy[10]",
"/rotaryz[10]",
"/rotaryx[11]",
"/rotaryy[11]",
"/rotaryz[11]", 
"/rotaryx[12]", 
"/rotaryy[12]",
"/rotaryz[12]",
"/rotaryx[13]",
"/rotaryy[13]",
"/rotaryz[13]", 
"/rotaryx[14]", 
"/rotaryy[14]",
"/rotaryz[14]",
"/rotaryx[15]",
"/rotaryy[15]",
"/rotaryz[15]", 
"/rotaryx[16]", // strict order preserved
"/rotaryy[16]",
"/rotaryz[16]",
"/rotaryx[17]",
"/rotaryy[17]",
"/rotaryz[17]", 
"/rotaryx[18]", 
"/rotaryy[18]",
"/rotaryz[18]",
"/rotaryx[19]",
"/rotaryy[19]",
"/rotaryz[19]", 
"/rotaryx[20]", 
"/rotaryy[20]",
"/rotaryz[20]",
"/rotaryx[21]",
"/rotaryy[21]",
"/rotaryz[21]", 
"/rotaryx[22]", 
"/rotaryy[22]",
"/rotaryz[22]",
"/rotaryx[23]",
"/rotaryy[23]",
"/rotaryz[23]", 

"/togglex[0]", 
"/toggley[0]",
"/togglez[0]",
"/togglex[1]",
"/toggley[1]",
"/togglez[1]",
"/togglex[2]", 
"/toggley[2]",
"/togglez[2]",
"/togglex[3]",
"/toggley[3]",
"/togglez[3]",
"/togglex[4]", 
"/toggley[4]",
"/togglez[4]",
"/togglex[5]",
"/toggley[5]",
"/togglez[5]",
"/togglex[6]", 
"/toggley[6]",
"/togglez[6]",
"/togglex[7]",
"/toggley[7]",
"/togglez[7]",
"/togglex[8]", 
"/toggley[8]",
"/togglez[8]",
"/togglex[9]",
"/toggley[9]",
"/togglez[9]",
"/togglex[10]", 
"/toggley[10]",
"/togglez[10]",
"/togglex[11]",
"/toggley[11]",
"/togglez[11]",
"/togglex[12]", 
"/toggley[12]",
"/togglez[12]",
"/togglex[13]",
"/toggley[13]",
"/togglez[13]",
"/togglex[14]", 
"/toggley[14]",
"/togglez[14]",
"/togglex[15]",
"/toggley[15]",
"/togglez[15]",
"/togglex[16]", 
"/toggley[16]",
"/togglez[16]",
"/togglex[17]",
"/toggley[17]",
"/togglez[17]",
"/togglex[18]", 
"/toggley[18]",
"/togglez[18]",
"/togglex[19]",
"/toggley[19]",
"/togglez[19]",
"/togglex[20]", 
"/toggley[20]",
"/togglez[20]",
"/togglex[21]",
"/toggley[21]",
"/togglez[21]",
"/togglex[22]", 
"/toggley[22]",
"/togglez[22]",
"/togglex[23]",
"/toggley[23]",
"/togglez[23]",
};

/*
"/trace[0]",	// specials + 10
"/trace[1]",
"/trace[2]",
"/trace[3]",
"/trace[4]",
"/trace[5]",
"/trace[6]",
"/trace[7]",
"/trace[8]",
"/trace[9]",

"/run",			// specials + 33
"/step",
"/hold",
"/set",	
"/reset",
"/esc",
"/resetview",
"/setview",
"/up",
"/holdview",
"/left",
"/down",
"/right",
"/xyup",
"/xydown",
"/xzup",
"/xzdown",
"/yzup",
"/yzdown",
"/textdisp",	// text display  on or off
"/xtextstart",	// text move left right
"/ytextstart",	// text move up down
"/debug",		// dump a connections list
"/conxn", 		// extract all connection details
"/weight",		// get and set weight
"/offset",		// get and set offset 
"/meter",		// meter a module no, input/output [0/1],nodeindex[,"/label"] 
"/conxin",   	// get input mdule in given connection
"/conxout",   	// get output module(s)
"/find",		//find all conections containing string arg
"/porto",
"/modname",		//set/get module name
"/extern",		//allows for OSC input to the external modulle
"/help"
*/


float frame[sizeof controldevlist / sizeof *controldevlist + 1]; // so we have a floating value for every item in the control list

int N = sizeof controldevlist / sizeof *controldevlist;



/* Declarations */
static int initudp(int chan);
static void closeudp(int sockfd);
Boolean ClientReply(int packetsize, void *packet, int socketfd, 
	void *clientaddresspointer, int clientaddressbufferlength);
void sgi_CleanExit(void);
Boolean sgi_HaveToQuit(void);
int RegisterPollingDevice(int fd, void (*callbackfunction)(int , void *), void *dummy);
static void catch_sigint();
static int Synthmessage(char *m, int n, void *clientdesc, int clientdesclength, int fd, struct xml_data *dataPtr) ;
void ParseOSCPacket(char *buf, int n, ClientAddr returnAddr, struct xml_data *dataPtr);
static void Smessage(char *address, void *v, int n, ClientAddr returnAddr, struct xml_data *dataPtr);
int PrintTypeTaggedArgs(void *v, int n);
static void PrintHeuristicallyTypeGuessedArgs(void *v, int n, int skipComma);
char *DataAfterAlignedString(char *string, char *boundary) ;
Boolean IsNiceString(char *string, char *boundary) ;
void complain(char *s, ...);
//PrintArgs(int size, struct TypeArgStruct *typesPtr);

extern int errno;

OSCTimeTag ParseTimeTag(char *s);
void ParseInteractiveLine(OSCbuf *buf, char *mesg);
typedArg ParseToken(char *token);
int WriteMessage(OSCbuf *buf, char *messageName, int numArgs, typedArg *args);
void SendBuffer(void *htmsocket, OSCbuf *buf);
void SendData(void *htmsocket, int size, char *data);
void fatal_error(char *s);

static int useTypeTags = 1;
struct TypeArgStruct mytypesPtr[20]; // total of seventeen argements for a fully populated connection ... minium of four

  



    int portrecnumber = 9001;		/* port to receive parameter updates from */
    int portsndnumber = 9000; // port to send parameter updates to
    int portdoscnumber = 9100; // default for DD digital duisplay
    char hostname[100] ;
    int updateflag = FALSE;
    OSCbuf buf[1];
    OSCbuf buff[1];
    typedArg arg;
    void *htmsocket, *htmsockettwo;


//****************
#define MAX_ARGS 2000
#define SC_BUFFER_SIZE 32000
static char bufferForOSCbuf[SC_BUFFER_SIZE];


//****************
// Output ddata output frame
void ddataframe(char *address, char *hostDestIP, int portdoscnumber, int numarg, struct io_module **Output_Modules ) {
	int i;
	typedArg arg[20];

	OSC_initBuffer(buff, SC_BUFFER_SIZE, bufferForOSCbuf);
// no bundle
//	if (OSC_openBundle(buff, OSCTT_Immediately())) {
//		complain("Problem opening bundle: %s\n", OSC_errorMessage);
//		return;
//	}
	htmsockettwo = OpenHTMSocket(hostDestIP, portdoscnumber);
	if (!htmsockettwo) {
		perror("Couldn't open socket: ");
		exit(3);
	}
	for (i=0; i < numarg; i++)  {
		arg[i].type =  FLOAT;
		arg[i].datum.f = (float) ( Output_Modules[i]->put.Out->value);
	}
	WriteMessage(buff, address, numarg, &arg[0]); 
// no bundle
//	if (OSC_closeBundle(buff)) {
//		complain("Problem closing bundle: %s\n", OSC_errorMessage);
//		return;
//	}
	SendBuffer(htmsockettwo, buff);
	CloseHTMSocket(htmsockettwo);
}


void external(struct module *Module, struct pertecs_build *buildPtr, long time)
{
	struct io_module *Output_Modules[2];
	struct io_module  Mod[2];
	struct field	in[2];
	char address[160]; // set up the address
	int numarg = 0;
	
	if (Module->Input[SPECIAL2]->value == 1.0) {// push 
		Output_Modules[0] = &Mod[0];
		Output_Modules[1] = &Mod[1];
		
		Mod[0].put.In = &in[0];
		Mod[1].put.In = &in[1];
	
		in[0].value = Module->Input[STANDARD]->value;
		in[1].value = Module->Input[SPECIAL]->value;

		if (Module->buff_sz > 0) numarg = Module->buff_sz;
		if (numarg > 2) numarg = 2;
		if (numarg) {
			if (strlen(Module->ParamPtr[5]->label)) sprintf(address,"%s %s %s %s",Module->ParamPtr[2]->label, Module->ParamPtr[3]->label, Module->ParamPtr[4]->label, Module->ParamPtr[5]->label);
			else if (strlen(Module->ParamPtr[4]->label)) sprintf(address,"%s %s %s",Module->ParamPtr[2]->label, Module->ParamPtr[3]->label, Module->ParamPtr[4]->label);
			else if (strlen(Module->ParamPtr[3]->label)) sprintf(address,"%s %s",Module->ParamPtr[2]->label, Module->ParamPtr[3]->label);
			else sprintf(address,"%s",Module->ParamPtr[2]->label);
			ddataframe(address, Module->ParamPtr[0]->label, atoi(Module->ParamPtr[1]->label), numarg, &Output_Modules[0] ); // send OSC
		}
	
	}
}


//****************
//#define NNUMBPOTBANKS	24			// max number of potbanks 24 is N_label/3
//#define BANKOFFSET	(3*NNUMBPOTBANKS) // same as N_label

void Updatepots(struct xml_data *dataPtr) {
	
	int	j = 0, i = 0, max;

	if ( dataPtr->build.Pot_channel_no <= N_label/3) max = dataPtr->build.Pot_channel_no;
	else max = N_label/3;

// initialise the pot data frame
	while (i < N_label/3) { // only up to 24 banks of pots (the complete set)
		if ( dataPtr->build.Pot_channel_no > i) { 	// count through the banks... if bank is outside the range then.... ??	
				sscanf(dataPtr->Modules[i]->ParamPtr[0]->label, "%f", &frame[j] ) ; 
				if (!strncmp( dataPtr->Modules[i]->ParamPtr[1]->label, "en", 2)) frame[N_label+j++] = 1 ;
				else frame[N_label+j++] = 0 ;
				sscanf(dataPtr->Modules[i]->ParamPtr[2]->label, "%f", &frame[j] ) ; 
				if (!strncmp( dataPtr->Modules[i]->ParamPtr[3]->label, "en", 2)) frame[N_label+j++] = 1 ;
				else frame[N_label+j++] = 0 ;
				sscanf(dataPtr->Modules[i]->ParamPtr[4]->label, "%f", &frame[j] ) ; 
				if (!strncmp( dataPtr->Modules[i]->ParamPtr[5]->label, "en", 2)) frame[N_label+j++] = 1 ;
				else frame[N_label+j++] = 0 ;
	
		}
		else {
			frame[j] = 0.0; frame[N_label + j++] = 0.0; 
			frame[j] = 0.0; frame[N_label + j++] = 0.0;
			frame[j] = 0.0; frame[N_label + j++] = 0.0;
		}
		i++;
	}


	if (dataPtr->build.Pot_channel_no >= 1) {			//we have at least one pot
	

		// ****************  update pots values and select switches **************************
// was//		for (i = 0; i < N ; i++) { 
		for (i = 0; i < max * 3; i++) {
	
			arg.type =  FLOAT;
			arg.datum.f = frame[i];
	
			WriteMessage(buf, controldevlist[i], 1, &arg);
			
		}

		
		// ****************  update default pot labels **************************
// was//		for (i = 0; i < N_label; i++) {
		for (i = 0; i < max * 3; i++) {
	
			arg.type =  STRING;
			arg.datum.s = controldevlabels[i % 3]; // it mod 3
	
			WriteMessage(buf, controllabellist[i], 1, &arg);
			
		}
	
		arg.type =  STRING;
		j = 0;
		for (i = 0; i < max; i++) {
			arg.datum.s = &dataPtr->Modules[i]->ParamPtr[0]->name[0];
			WriteMessage(buf, controllabellist[j++], 1, &arg);
			arg.datum.s = &dataPtr->Modules[i]->ParamPtr[2]->name[0];
			WriteMessage(buf, controllabellist[j++], 1, &arg);
			arg.datum.s = &dataPtr->Modules[i]->ParamPtr[4]->name[0];
			WriteMessage(buf, controllabellist[j++], 1, &arg);
		}
    } // end of pots updates
}



void updateinterface(char *hostname, int portsndnumber, struct xml_data *dataPtr) {
    char *messageName;
    char *token;
    int i,j, numArgs;

    OSC_initBuffer(buf, SC_BUFFER_SIZE, bufferForOSCbuf);

    if (N > 1) {
	if (OSC_openBundle(buf, OSCTT_Immediately())) {
	    complain("Problem opening bundle: %s\n", OSC_errorMessage);
	    return;
	}
    }

    htmsocket = OpenHTMSocket(hostname, portsndnumber);
    if (!htmsocket) {
        perror("Couldn't open socket: ");
        exit(3);
    }

    //***************** disable buttons **************************
	if (lock_axis_flag == TRUE) {
            	arg.type =  STRING;
				arg.datum.s = ""; 									// blank out the button labels
				WriteMessage(buf, "/leftlabel", 1, &arg); 
				WriteMessage(buf, "/rightlabel", 1, &arg); 
				WriteMessage(buf, "/uplabel", 1, &arg); 
				WriteMessage(buf, "/downlabel", 1, &arg); 	
				WriteMessage(buf, "/resetviewlabel", 1, &arg); 	
				WriteMessage(buf, "/holdviewlabel", 1, &arg); 	
    }
    else { // refresh the button labels for the Rotate-Tumble View buttons
           		arg.type =  STRING;
    			arg.datum.s = "UP";
				WriteMessage(buf, "/uplabel", 1, &arg); 
    			arg.datum.s = "DOWN";
 				WriteMessage(buf, "/downlabel", 1, &arg); 	
   				arg.datum.s = "LEFT";
				WriteMessage(buf, "/leftlabel", 1, &arg); 
    			arg.datum.s = "RIGHT";
				WriteMessage(buf, "/rightlabel", 1, &arg); 
    			arg.datum.s = "RESET";
				WriteMessage(buf, "/resetviewlabel", 1, &arg); 
    			arg.datum.s = "HOLD";
				WriteMessage(buf, "/holdviewlabel", 1, &arg); 
    }

	// **************** update pots and switch settings  *********

	Updatepots(dataPtr);

	// **************** update page and controls *****************
        WriteMessage(buf, "/1", 0, NULL); // switch to page 1

            arg.type =  FLOAT;
            arg.datum.f = 1.0;
        WriteMessage(buf, "/hold", 1, &arg); 
            arg.datum.f = 0.0;
        WriteMessage(buf, "/run", 1, &arg); 


    if (N > 1) {
	if (OSC_closeBundle(buf)) {
	    complain("Problem closing bundle: %s\n", OSC_errorMessage);
	    return;
	}
    }

    SendBuffer(htmsocket, buf);
}





OSCTimeTag ParseTimeTag(char *s) {
    char *p, *newline;
    typedArg arg;

    p = s;
    while (isspace(*p)) p++;
    if (*p == '\0') return OSCTT_Immediately();

    if (*p == '+') {
	/* Time tag is for some time in the future.  It should be a
           number of seconds as an int or float */

	newline = strchr(s, '\n');
	if (newline != NULL) *newline = '\0';

	p++; /* Skip '+' */
	while (isspace(*p)) p++;

	arg = ParseToken(p);
	if (arg.type == STRING) {
	    complain("warning: inscrutable time tag request: %s\n", s);
	    return OSCTT_Immediately();
	} else if (arg.type == INT) {
	    return OSCTT_PlusSeconds(OSCTT_CurrentTime(),
				     (float) arg.datum.i);
	} else if (arg.type == FLOAT) {
	    return OSCTT_PlusSeconds(OSCTT_CurrentTime(), arg.datum.f);
	} else {
	    fatal_error("This can't happen!");
	}
    }

    if (isdigit(*p) || (*p >= 'a' && *p <='f') || (*p >= 'A' && *p <='F')) {
	/* They specified the 8-byte tag in hex */
	OSCTimeTag tt;
	if (sscanf(p, "%llx", (long long unsigned int *) &tt) != 1) {
	    complain("warning: couldn't parse time tag %s\n", s);
	    return OSCTT_Immediately();
	}
#ifndef	HAS8BYTEINT
	if (ntohl(1) != 1) {
	    /* tt is a struct of seconds and fractional part,
	       and this machine is little-endian, so sscanf
	       wrote each half of the time tag in the wrong half
	       of the struct. */
	    uint32 temp;
	    temp = tt.seconds;
	    tt.seconds = tt.fraction ;
	    tt.fraction = temp;
	}
#endif
	return tt;
    }

    complain("warning: invalid time tag: %s\n", s);
    return OSCTT_Immediately();
}
	    

void ParseInteractiveLine(OSCbuf *buf, char *mesg) {
    char *messageName, *token, *p;
    typedArg args[MAX_ARGS];
    int thisArg;

    p = mesg;
    while (isspace(*p)) p++;
    if (*p == '\0') return;

    messageName = p;

    if (strcmp(messageName, "play\n") == 0) {
	/* Special kludge feature to save typing */
	typedArg arg;

	if (OSC_openBundle(buf, OSCTT_Immediately())) {
	    complain("Problem opening bundle: %s\n", OSC_errorMessage);
	    return;
	}

	arg.type = INT;
	arg.datum.i = 0;
	WriteMessage(buf, "/voices/0/tp/timbre_index", 1, &arg);

	arg.type = FLOAT;
	arg.datum.i = 0.0f;
	WriteMessage(buf, "/voices/0/tm/goto", 1, &arg);

	if (OSC_closeBundle(buf)) {
	    complain("Problem closing bundle: %s\n", OSC_errorMessage);
	}

	return;
    }

    while (!isspace(*p) && *p != '\0') p++;
    if (isspace(*p)) {
        *p = '\0';
        p++;
    }

    thisArg = 0;
    while (*p != '\0') {
        /* flush leading whitespace */
        while (isspace(*p)) p++;
        if (*p == '\0') break;

        if (*p == '"') {
            /* A string argument: scan for close quotes */
            p++;
            args[thisArg].type = STRING;
            args[thisArg].datum.s = p;

            while (*p != '"') {
                if (*p == '\0') {
                    complain("Unterminated quote mark: ignoring line\n");
                    return;
                }
                p++;
            }
            *p = '\0';
            p++;
        } else {
            token = p;
            while (!isspace(*p) && (*p != '\0')) p++;
            if (isspace(*p)) {
                *p = '\0';
                p++;
            }
            args[thisArg] = ParseToken(token);
        }
        thisArg++;
	if (thisArg >= MAX_ARGS) {
	  complain("Sorry, your message has more than MAX_ARGS (%d) arguments; ignoring the rest.\n",
		   MAX_ARGS);
	  break;
	}
    }

    if (WriteMessage(buf, messageName, thisArg, args) != 0)  {
	complain("Problem sending message: %s\n", OSC_errorMessage);
    }
}

typedArg ParseToken(char *token) {
    char *p = token;
    typedArg returnVal;

    /* It might be an int, a float, or a string */

    if (*p == '-') p++;

    if (isdigit(*p) || *p == '.') {
        while (isdigit(*p)) p++;
        if (*p == '\0') {
            returnVal.type = INT;
            returnVal.datum.i = atoi(token);
            return returnVal;
        }
        if (*p == '.') {
            p++;
            while (isdigit(*p)) p++;
            if (*p == '\0') {
                returnVal.type = FLOAT;
                returnVal.datum.f = atof(token);
                return returnVal;
            }
        }
    }

    returnVal.type = STRING;
    returnVal.datum.s = token;
    return returnVal;
}

//******************************************************************************
int WriteMessage(OSCbuf *buf, char *messageName, int numArgs, typedArg *args) {
    int j, returnVal;

    returnVal = 0;

#ifdef DEBUG
    printf("WriteMessage: %s ", messageName);

     for (j = 0; j < numArgs; j++) {
        switch (args[j].type) {
            case INT:
	    printf("%d ", args[j].datum.i);
            break;

            case FLOAT:
	    printf("%f ", args[j].datum.f);
            break;

            case STRING:
	    printf("%s ", args[j].datum.s);
            break;

            default:
            fatal_error("Unrecognized arg type");
            exit(5);
        }
    }
    printf("\n");
#endif

    if (!useTypeTags) {
	returnVal = OSC_writeAddress(buf, messageName);
	if (returnVal) {
	    complain("Problem writing address: %s\n", OSC_errorMessage);
	}
    } else {
	/* First figure out the type tags */
	char typeTags[MAX_ARGS+2];
	int i;

	typeTags[0] = ',';

	for (i = 0; i < numArgs; ++i) {
	    switch (args[i].type) {
		case INT:
		typeTags[i+1] = 'i';
		break;

		case FLOAT:
		typeTags[i+1] = 'f';
		break;

		case STRING:
		typeTags[i+1] = 's';
		break;

		default:
		fatal_error("Unrecognized arg type");
		exit(5);
	    }
	}
	typeTags[i+1] = '\0';
	    
	returnVal = OSC_writeAddressAndTypes(buf, messageName, typeTags);
	if (returnVal) {
	    complain("Problem writing address: %s\n", OSC_errorMessage);
	}
    }

     for (j = 0; j < numArgs; j++) {
        switch (args[j].type) {
            case INT:
            if ((returnVal = OSC_writeIntArg(buf, args[j].datum.i)) != 0) {
		return returnVal;
	    }
            break;

            case FLOAT:
            if ((returnVal = OSC_writeFloatArg(buf, args[j].datum.f)) != 0) {
		return returnVal;
	    }
            break;

            case STRING:
            if ((returnVal = OSC_writeStringArg(buf, args[j].datum.s)) != 0) {
		return returnVal;
	    }
            break;

            default:
            fatal_error("Unrecognized arg type");
            exit(5);
        }
    }

    return returnVal;
}

//******************************************************************************
void SendBuffer(void *htmsocket, OSCbuf *buf) {

#ifdef DEBUG
    fprintf(stderr, "Sending buffer...\n"); fflush{stderr}
#endif
    if (OSC_isBufferEmpty(buf)) return;
    if (!OSC_isBufferDone(buf)) {
	fatal_error("SendBuffer() called but buffer not ready!");
	exit(5);
    }
    SendData(htmsocket, OSC_packetSize(buf), OSC_getPacket(buf));
}

//******************************************************************************
void SendData(void *htmsocket, int size, char *data) {
    if (!SendHTMSocket(htmsocket, size, data)) {
        perror("Couldn't send out socket: ");
        CloseHTMSocket(htmsocket);
        exit(3);
    }
}

void fatal_error(char *s) {
    fprintf(stderr, "%s\n", s);
    exit(4);
}

#include <stdarg.h>
void complain(char *s, ...) {
    va_list ap;
    va_start(ap, s);
    vfprintf(stderr, s, ap);
    va_end(ap);
}


#ifdef COMPUTE_MESSAGE_SIZE
    /* Unused code to find the size of a message */

    /* Compute size */
    size = SynthControl_effectiveStringLength(messageName);

    for (j = 0; j < numArgs; j++) {
        switch (args[j].type) {
            case INT: case FLOAT:
            size += 4;
            break;

            case STRING:
            size += SynthControl_effectiveStringLength(args[j].datum.s);
            break;

            default:
            fatal_error("Unrecognized token type");
            exit(4);
        }
    }

    if (!SynthControl_willMessageFit(buf, size)) {
        complain("Message \"%s\" won't fit in buffer: dropping.", messageName);
        return;
    }
#endif
//**********************




#define UNIXDG_PATH "/tmp/htm"


static int initudp(int chan)
{
	struct sockaddr_in serv_addr;
	int  sockfd;
	
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
			return sockfd;
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(chan);
	
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("unable to  bind\n");
		return -1;
	}

	fcntl(sockfd, F_SETFL, FNDELAY); 
	return sockfd;
}

static void closeudp(int sockfd) {
		close(sockfd);
}

static Boolean catchupflag=FALSE;
Boolean ClientReply(int packetsize, void *packet, int socketfd, 
	void *clientaddresspointer, int clientaddressbufferlength)
{
	if(!clientaddresspointer) return FALSE;
	catchupflag= TRUE;
	return packetsize==sendto(socketfd, packet, packetsize, 0, clientaddresspointer, clientaddressbufferlength);
}

static Boolean exitflag= FALSE;
void sgi_CleanExit(void) {
	exitflag = TRUE;
}

Boolean sgi_HaveToQuit(void) {
	return exitflag;
}


/* file descriptor poll table */
static int npolldevs =0;
typedef struct polldev
{
	int fd;
	void (*callbackfunction)(int , void *);
	void *dummy;
} polldev;
#define TABMAX 8
static polldev polldevs[TABMAX];


/*      Register a device (referred to by a file descriptor that the caller
	should have already successfully obtained from a system call) to be
	polled as real-time constraints allowed.
        
        When a select(2) call indicates activity on the file descriptor, the
	callback function is called with the file descripter as first
	argument and the given dummy argument (presumably a pointer to the
	instance variables associated with the device).
*/
int RegisterPollingDevice(int fd, void (*callbackfunction)(int , void *), void *dummy)
{
	if(npolldevs<TABMAX)
	{
		polldevs[npolldevs].fd = fd;
		polldevs[npolldevs].callbackfunction = callbackfunction;
		polldevs[npolldevs].dummy = dummy;
	}
	else return -1;
	return npolldevs++;
}

static int caught_sigint;

static void catch_sigint()  {
   caught_sigint = 1;
}

static int sockfd;



static int Synthmessage(char *m, int n, void *clientdesc, int clientdesclength, int fd, struct xml_data *dataPtr)  {
    struct ClientAddressStruct ras;
    ClientAddr ra = &ras;

    catchupflag= FALSE;

    ras.cl_addr = *((struct sockaddr_in *) clientdesc);
    ras.clilen = clientdesclength;
    ras.sockfd = fd;

    ParseOSCPacket(m, n, ra, dataPtr);
    return catchupflag;
}

void PrintClientAddr(ClientAddr CA) {
    unsigned long addr = CA->cl_addr.sin_addr.s_addr;
    fprintf(stderr,"Client address %p:\n", CA);
    fprintf(stderr,"  clilen %d, sockfd %d\n", CA->clilen, CA->sockfd);
    fprintf(stderr,"  sin_family %d, sin_port %d\n", CA->cl_addr.sin_family,
	   CA->cl_addr.sin_port);
    fprintf(stderr,"  address: (%lx) %s\n", addr,  inet_ntoa(CA->cl_addr.sin_addr));

    fprintf(stderr,"  sin_zero = \"%c%c%c%c%c%c%c%c\"\n", 
	   CA->cl_addr.sin_zero[0],
	   CA->cl_addr.sin_zero[1],
	   CA->cl_addr.sin_zero[2],
	   CA->cl_addr.sin_zero[3],
	   CA->cl_addr.sin_zero[4],
	   CA->cl_addr.sin_zero[5],
	   CA->cl_addr.sin_zero[6],
	   CA->cl_addr.sin_zero[7]);

    fprintf(stderr,"\n");
}


void ParseOSCPacket(char *buf, int n, ClientAddr returnAddr, struct xml_data *dataPtr) {
    int size, messageLen, i;
    char *messageName;
    char *args;

#ifdef PRINTADDRS
    PrintClientAddr(returnAddr);
#endif


    if ((n%4) != 0) {
	complain("SynthControl packet size (%d) not a multiple of 4 bytes: dropping",
		 n);
	return;
    }

    if ((n >= 8) && (strncmp(buf, "#bundle", 8) == 0)) {
	/* This is a bundle message. */

		if (n < 16) {
			complain("Bundle message too small (%d bytes) for time tag", n);
			return;
		}
		
		/* Print the time tag */
		//	fprintf(stderr,"[ %lx%08lx\n", ntohl(*((unsigned long *)(buf+8))),
		//	       ntohl(*((unsigned long *)(buf+12))));
		/* Note: if we wanted to actually use the time tag as a little-endian
		   64-bit int, we'd have to word-swap the two 32-bit halves of it */
		
		i = 16; /* Skip "#group\0" and time tag */
		while(i<n) {
			size = ntohl(*((int *) (buf + i)));
			if ((size % 4) != 0) {
			complain("Bad size count %d in bundle (not a multiple of 4)", size);
			return;
			}
			if ((size + i + 4) > n) {
			complain("Bad size count %d in bundle (only %d bytes left in entire bundle)",
				 size, n-i-4);
			return;	
			}
			
			/* Recursively handle element of bundle */
			ParseOSCPacket(buf+i+4, size, returnAddr, dataPtr);
			i += 4 + size;
		}
		if (i != n) {
			complain("This can't happen");
		}
		//	fprintf(stderr,"]\n");
    } else {
	/* This is not a bundle message */

	messageName = buf;
	args = DataAfterAlignedString(messageName, buf+n);
	if (args == 0) {
	    complain("Bad message name string: %s\nDropping entire message.\n",
		     htm_error_string);
	    return;
	}
	messageLen = args-messageName;	    
	Smessage(messageName, (void *)args, n-messageLen, returnAddr, dataPtr);					// print out data frame
    }
}

#define SMALLEST_POSITIVE_FLOAT 0.000001f

static void Smessage(char *address, void *v, int n, ClientAddr returnAddr, struct xml_data *dataPtr) {
    char *chars = v;
	int  result = 0, i = 0, j = 0;



	// lets deal with the controls

    OSC_initBuffer(buf, SC_BUFFER_SIZE, bufferForOSCbuf);


	if (OSC_openBundle(buf, OSCTT_Immediately())) {
		complain("Problem opening bundle: %s\n", OSC_errorMessage);
		return;
	}




//debug			fprintf(stderr,"address: %s\n", address); fflush(stderr);

int numarg;
	numarg = PrintTypeTaggedArgs(v, n);
		//fprintf(stderr,"numarg: %d \n", numarg); fflush(stderr);

// on toggle buttons.. action on depression
	if (numarg <= 1) { // any command with a single argument or none... 
	// ******************** run ***************************
		if (!strcmp(address, "/run"))  {
		 arg.type =  INT;
		 arg.datum.i = 0;
			WriteMessage(buf, "/hold", 1, &arg); 
		 arg.datum.i = 1;
			WriteMessage(buf, "/run", 1, &arg); 
	
			dataPtr->run_mode = RUN;
			goto send_immediate;
	
		}
	
	// ******************** hold ***************************
		else if (!strcmp(address, "/hold")) {
		 arg.type =  INT;
		 arg.datum.i = 0;
			WriteMessage(buf, "/run", 1, &arg); 
		 arg.datum.i = 1;
			WriteMessage(buf, "/hold", 1, &arg); 
	
			dataPtr->run_mode = PAUSE;
			goto send_immediate;
		}
	
	// ******************** reset ***************************
		else if (!strcmp(address, "/reset")) {
		 arg.type =  INT;
		 arg.datum.i = 0;
			WriteMessage(buf, "/run", 1, &arg); 
		 arg.datum.i = 1;
			WriteMessage(buf, "/hold", 1, &arg); 
	
			dataPtr->run_mode = RESET;
			Updatepots(dataPtr);
			goto send_immediate;
		}
	
	// ******************** set ***************************
		else if (!strcmp(address, "/set") ) {
		 arg.type =  INT;
		 arg.datum.i = 0;
			WriteMessage(buf, "/run", 1, &arg); 
		 arg.datum.i = 1;
			WriteMessage(buf, "/hold", 1, &arg); 
	
			dataPtr->run_mode = RESET;
			Updatepots(dataPtr);
			goto send_immediate;
		}
	
	// ******************** step ***************************
		else if (!strcmp(address, "/step") ) {
			if (numarg==0) {
				 arg.type =  INT;
				 arg.datum.i = 0;
					WriteMessage(buf, "/run", 1, &arg); 
				 arg.datum.i = 1;
					WriteMessage(buf, "/hold", 1, &arg); 
				dataPtr->run_mode = dataPtr->stepincr;
				goto send_immediate;
			}
			else if ((mytypesPtr[0].type == INT) && (mytypesPtr[0].datum.i > 0 )) { // could be in either tree.. 
				dataPtr->run_mode = dataPtr->stepincr = mytypesPtr[0].datum.i;
				goto skip_send;
			}
		}
	
	// ******************** esc ***************************
		else if (!strcmp(address, "/esc")) {
		  	arg.type =  INT;
			arg.datum.i = 0;
			WriteMessage(buf, "/run", 1, &arg); 
			WriteMessage(buf, "/hold", 1, &arg); 
			
			dataPtr->run_mode = STOP;
			sgi_CleanExit(); 					// initiate the clean exit of the OSC interface
			goto send_immediate;
		}

	// ******************** xyup ***************************
		else if (!strcmp(address, "/xyup") ) {
			if (zaxis <= .9) { zaxis+=.1 ; glutPostRedisplay(); }
				goto skip_send;
		}
	// ******************** xydown ***************************
		else if (!strcmp(address, "/xydown") ) {
			if (zaxis > 0.0 ) { zaxis-=.1 ; glutPostRedisplay(); }
				goto skip_send;

		}
	// ******************** xzup ***************************
		else if (!strcmp(address, "/xzup") ) {
			if (yaxis <= .9) { yaxis+=.1 ; glutPostRedisplay(); }
				goto skip_send;

		}
	// ******************** xzdown ***************************
		else if (!strcmp(address, "/xzdown") ) {
			if (yaxis > 0.0 ) { yaxis-=.1 ; glutPostRedisplay(); }
				goto skip_send;

		}
	// ******************** yzup ***************************
		else if (!strcmp(address, "/yzup") ) {
			if (xaxis <= .9 ) { xaxis +=.1 ; glutPostRedisplay(); }
				goto skip_send;

		}
	// ******************** yzdown ***************************
		else if (!strcmp(address, "/yzdown") ) {
			if (xaxis > 0.0 ) { xaxis-=.1 ; glutPostRedisplay(); }
				goto skip_send;
			
		}


		
	// ******************** left rotate ***************************
		else if (!strcmp(address, "/left") ) {
			if (lock_axis_flag == FALSE) { 
				if ((numarg == 1) && (mytypesPtr[0].type==FLOAT) && (mytypesPtr[0].datum.f < .5) && (mytypesPtr[0].datum.f >= 0.0)) trace = -mytypesPtr[0].datum.f;
				else trace -= RATESTEP; 
				glutIdleFunc(both); 
				goto skip_send;
			}
		}
	// ******************** right rotate ***************************
		else if (!strcmp(address, "/right") ) {
			if (lock_axis_flag == FALSE) {
				if ((numarg == 1) && (mytypesPtr[0].type==FLOAT) && (mytypesPtr[0].datum.f < .5) && (mytypesPtr[0].datum.f >= 0.0)) trace = mytypesPtr[0].datum.f;
				else trace += RATESTEP; 
				glutIdleFunc(both); 
				goto skip_send;
			}

		}
	// ******************** up tumble ***************************
		else if (!strcmp(address, "/up") ) {
			if (lock_axis_flag == FALSE) {
				if ((numarg == 1) && (mytypesPtr[0].type==FLOAT) && (mytypesPtr[0].datum.f < .5) && (mytypesPtr[0].datum.f >= 0.0)) tracex = -mytypesPtr[0].datum.f;
				else tracex -= RATESTEP; 
				glutIdleFunc(both); 
				goto skip_send;
			}

		}
	// ******************** down tumble ***************************
		else if (!strcmp(address, "/down") ) {
			if (lock_axis_flag == FALSE) {
				if ((numarg == 1) && (mytypesPtr[0].type==FLOAT) && (mytypesPtr[0].datum.f < .5) && (mytypesPtr[0].datum.f >= 0.0)) tracex = mytypesPtr[0].datum.f;
				else tracex += RATESTEP; 
				glutIdleFunc(both); 
				goto skip_send;
			}

		}
	// ******************** hold view ***************************
		else if (!strcmp(address, "/holdview") ) {
			if (lock_axis_flag == FALSE) {trace = tracex = 0; glutIdleFunc(both); }
				goto skip_send;

		}
	// ******************** down tumble ***************************
		else if (!strcmp(address, "/resetview") ) {
			if (lock_axis_flag == FALSE) { 
				trace = tracex = 0;
				spinysurface = 0; 
				spinxsurface = 0;
				glutIdleFunc(both); 
				goto skip_send;
			}

		}
		else if (!strcmp(address, "/debug") ) { // dump all the connections list to new circuit file with ~ added to file name
			cpy_Build_debug( dataPtr );
			IO_Names_debug ( dataPtr );			// create the debug config file;
			Mod_Names_debug ( dataPtr );		//
			Circuit_Names_debug ( dataPtr );
		}
		else if ((numarg == 1) && (mytypesPtr[0].type==INT)) {// button down have some values
			// ******************** connection ***************************
			 if (!strncmp(address, "/trace", 6) ) {
				int which = -1;
				if (!strncmp(&address[6], "[0]", 3) ) which =9; 
				else if (!strncmp(&address[6], "[1]", 3) ) which =0;
				else if (!strncmp(&address[6], "[2]", 3) ) which =1;
				else if (!strncmp(&address[6], "[3]", 3) ) which =2;
				else if (!strncmp(&address[6], "[4]", 3) ) which =3;
				else if (!strncmp(&address[6], "[5]", 3) ) which =4;
				else if (!strncmp(&address[6], "[6]", 3) ) which =5;
				else if (!strncmp(&address[6], "[7]", 3) ) which =6;
				else if (!strncmp(&address[6], "[8]", 3) ) which =7;
				else if (!strncmp(&address[6], "[9]", 3) ) which =8;
				else which = 10; // signal to process all together

				if (mytypesPtr[0].datum.i == 0) {
					if ((which >=0) && (which <= 9)) {
						if ((curvePtr[which]) && (curvePtr[which]->npolylinenodes)) curvePtr[which]->visible = GLUT_NOT_VISIBLE;
					} else if (which == 10) {
						for (which =0; which <=9; which++) {
							if ((curvePtr[which]) && (curvePtr[which]->npolylinenodes)) curvePtr[which]->visible = GLUT_NOT_VISIBLE;
						}
					}
				}
				else if (mytypesPtr[0].datum.i == 1) {
					if ((which >=0) && (which <= 9)) {
						if ((curvePtr[which]) && (curvePtr[which]->npolylinenodes)) curvePtr[which]->visible = GLUT_VISIBLE;
					} else if (which == 10) {
						for (which =0; which <=9; which++) { 
							if ((curvePtr[which]) && (curvePtr[which]->npolylinenodes)) curvePtr[which]->visible = GLUT_VISIBLE;
						}
					}
				}
				goto skip_send;

			}
			else if (!strcmp(address, "/textdisp") ) {
				if (numarg == 0) {
					helpmenu = FALSE;
					if (textdisp == FALSE) textdisp = TRUE;
					else if (textdisp == TRUE) textdisp = FALSE;
				} else if ((numarg == 1) && (mytypesPtr[0].type == INT)) {
					if (mytypesPtr[0].datum.i == 0) {
						helpmenu = FALSE;
						textdisp = FALSE;
					} else if (mytypesPtr[0].datum.i == 1) {
						helpmenu = FALSE;
						textdisp = TRUE;
					}
				}
				goto skip_send;

			}
			else if (!strcmp(address, "/xtextstart") ) {
				if ((mytypesPtr[0].type == INT) && (mytypesPtr[0].datum.i >= 0 )) {
					xtextstart = mytypesPtr[0].datum.i;
				}
				goto skip_send;
			}
			else if (!strcmp(address, "/ytextstart") ) {
				if ((mytypesPtr[0].type == INT) && (mytypesPtr[0].datum.i >= 0 )) {
					ytextstart = mytypesPtr[0].datum.i;
				}
				goto skip_send;
			}
			else if (!strcmp(address, "/conxn") ) {
				int connumb = mytypesPtr[0].datum.i;
				struct Node *searchnodePtr = NULL;				
				GetNodePtr( dataPtr->CircuitTree, connumb, &searchnodePtr);
				GetNodePtr( dataPtr->SamplingTree, connumb, &searchnodePtr);
				
				if (searchnodePtr != NULL) {
					// input
					mytypesPtr[1].type = STRING; 
					mytypesPtr[2].type = STRING; 
					
					if (searchnodePtr->Input != NULL) {
						int index = searchnodePtr->Input->module_no;
						if (index < dataPtr->build.build_size) mytypesPtr[1].datum.s = dataPtr->Modules[index]->name;
						else mytypesPtr[1].datum.s = dataPtr->IO_Modules[index-dataPtr->build.build_size]->name;
						mytypesPtr[2].datum.s = searchnodePtr->Input->name;  // node connection name
						
						
						// offset
						mytypesPtr[3].type = FLOAT; // set up for outputting a float
						mytypesPtr[3].datum.f = searchnodePtr->offset;
						// constant weight
						mytypesPtr[4].type = FLOAT; // set up for outputting a float
						mytypesPtr[4].datum.f = searchnodePtr->constant;
						// outputs
						int k = 0;
						while (searchnodePtr->Output[k] != NULL) {
							mytypesPtr[2*k + 5].type = STRING; 
							mytypesPtr[2*k + 6].type = STRING;
							int index = searchnodePtr->Output[k]->module_no;
							if (index < dataPtr->build.build_size)
								mytypesPtr[2*k+5].datum.s = dataPtr->Modules[index]->name;
							else mytypesPtr[2*k+5].datum.s = dataPtr->IO_Modules[index-dataPtr->build.build_size]->name;
							mytypesPtr[2*k+6].datum.s = searchnodePtr->Output[k]->name;  // node connection name
							k+=1;
						}
					
						WriteMessage(buf, "/conxn", 2*k + 5, mytypesPtr); 
						goto send_immediate;

					}
				}
			}
			else if (!strcmp(address, "/porto") ) {
				if ((mytypesPtr[0].datum.i >= 9000) && (mytypesPtr[0].datum.i < 50000)) {
					portsndnumber = mytypesPtr[0].datum.i;
					updateflag = FALSE;
				}
			}
		}
		if ((numarg == 0) && (!strcmp(address, "/help") )) {
			fprintf(stderr,"/run       \tenable running of pertecs\n"); fflush(stderr);
			fprintf(stderr,"/step,[n]  \tstep the process by n steps\n"); fflush(stderr);
			fprintf(stderr,"/hold      \tpause the running of pertecs\n"); fflush(stderr);
			fprintf(stderr,"/set       \tinitialise the modules and pause\n"); fflush(stderr);	
			fprintf(stderr,"/reset     \tsame as /set\n"); fflush(stderr);
			fprintf(stderr,"/esc       \tterminate pertecs\n"); fflush(stderr);
			fprintf(stderr,"/resetview \trestore orientation of scope view\n"); fflush(stderr);
			fprintf(stderr,"/setview,[n1][n2] \t set the orientation of the screen view\n"); fflush(stderr);
			fprintf(stderr,"/holdview  \tpause the roll/tumble\n"); fflush(stderr);
			fprintf(stderr,"/up        \ttumbling back\n"); fflush(stderr);
			fprintf(stderr,"/down      \ttumbling forward\n"); fflush(stderr);
			fprintf(stderr,"/left      \tspin left\n"); fflush(stderr);
			fprintf(stderr,"/right     \tspin right\n"); fflush(stderr);
			fprintf(stderr,"/xyup      \txy axis brightness increase\n"); fflush(stderr);	
			fprintf(stderr,"/xydown    \txy axis brightness decrease\n"); fflush(stderr);
			fprintf(stderr,"/xzup      \txz axis brightness increase\n"); fflush(stderr);
			fprintf(stderr,"/xzdown    \txz axis brightness increase\n"); fflush(stderr);
			fprintf(stderr,"/yzup      \tyz axis brightness increase\n"); fflush(stderr);
			fprintf(stderr,"/yzdown    \tyz axis brightness increase\n"); fflush(stderr);
			fprintf(stderr,"/textdisp  \ttoggle text display  on or off\n"); fflush(stderr);
			fprintf(stderr,"/xtextstart,x\ttext move x position\n"); fflush(stderr);
			fprintf(stderr,"/ytextstart,y\ttext move y position\n"); fflush(stderr);
			fprintf(stderr,"/debug     \toutput a new set of configuration files\n"); fflush(stderr);
			fprintf(stderr,"/conxn,n   \textract nth connection as OSC \\conxn\n"); fflush(stderr);
			fprintf(stderr,"/find,\"text\",n  \textract nth connection with \"text\" name or label, outputs \\conxn\n"); fflush(stderr);
			fprintf(stderr,"/weight,n[,f]\tget and set connection constant weight\n"); fflush(stderr);
			fprintf(stderr,"/offset,n[,f]\tget and set connection offset\n"); fflush(stderr);
			fprintf(stderr,"/meter,[modnum][0=input/1=output][pinumb]\n"); fflush(stderr);
			fprintf(stderr,"/conxin,n    \tget input module in connection n\n"); fflush(stderr);
			fprintf(stderr,"/conxout,n   \tget output module(s) in connection n\n"); fflush(stderr);
			fprintf(stderr,"/porto,n    \tset a new OSC output port number\n"); fflush(stderr);
			fprintf(stderr,"/modname,n[,newname]\tget or set module name\n"); fflush(stderr);
			fprintf(stderr,"/extern,M,f1,f2,f3,f4,f5,f6\tenters floating values into module EXTERN_M\n"); fflush(stderr);
			fprintf(stderr,"/help    \t outputs this help as stderr\n"); fflush(stderr);

			goto skip_send;
		}
	}
	if (((numarg == 1) || (numarg == 2)) && (mytypesPtr[0].type == INT)) { // connection number
		if (!strcmp(address, "/weight")) {
			int connumb = mytypesPtr[0].datum.i;
			struct Node *searchnodePtr = NULL;				
			GetNodePtr( dataPtr->CircuitTree, connumb, &searchnodePtr);
			GetNodePtr( dataPtr->SamplingTree, connumb, &searchnodePtr);
			if (searchnodePtr != NULL) {
				if (numarg == 1) { // get weight
					mytypesPtr[1].type = FLOAT; // set up for outputting a float
					mytypesPtr[1].datum.f = searchnodePtr->constant;
					WriteMessage(buf, "/weight", 2, mytypesPtr); 
					goto send_immediate;
				
				} else {  // numarg == 2 set weight
					if (mytypesPtr[1].type == INT) searchnodePtr->constant = (double) mytypesPtr[1].datum.i;
					else if (mytypesPtr[1].type == FLOAT) searchnodePtr->constant = (double) mytypesPtr[1].datum.f;
					goto skip_send;
				} 
			}
		}
		else if (!strcmp(address, "/offset") ) {
			int connumb = mytypesPtr[0].datum.i;
			struct Node *searchnodePtr = NULL;				
			GetNodePtr( dataPtr->CircuitTree, connumb, &searchnodePtr);
			GetNodePtr( dataPtr->SamplingTree, connumb, &searchnodePtr);
			if (searchnodePtr != NULL) {
				if (numarg == 1) { // get offset
					mytypesPtr[1].type = FLOAT; // set up for outputting a float
					mytypesPtr[1].datum.f = searchnodePtr->offset;
					WriteMessage(buf, "/offset", 2, mytypesPtr); 
					goto send_immediate;
				
				} else {  // numarg == 2 set weight
					if (mytypesPtr[1].type == INT) searchnodePtr->offset = (double) mytypesPtr[1].datum.i;
					else if (mytypesPtr[1].type == FLOAT) searchnodePtr->offset = (double) mytypesPtr[1].datum.f;
					goto skip_send;
				} 
			}
		}
		if (!strcmp(address, "/modname")) {
			int modnumb = mytypesPtr[0].datum.i;
			if ((modnumb >=0) && (modnumb < dataPtr->build.build_size)) { // is in processing modules
				if (numarg == 1) { // get name
					mytypesPtr[1].type = STRING; // set up for outputting a float
					mytypesPtr[1].datum.s = dataPtr->Modules[modnumb]->name;
					WriteMessage(buf, "/modname", 2, mytypesPtr); 
					goto send_immediate;
				} else {  // numarg == 2 set weight
					if (mytypesPtr[1].type == STRING) {
						strcpy(dataPtr->Modules[modnumb]->name,mytypesPtr[1].datum.s);
					}
					goto skip_send; 
				}
			}
			// IO modules
			else if (modnumb < dataPtr->build.build_size + dataPtr->build.Input_channel_no + (dataPtr->build.Input_channel_no > 0) + dataPtr->build.Output_channel_no + (dataPtr->build.Output_channel_no > 0) + dataPtr->build.Connection_build_size) {
				modnumb -= dataPtr->build.build_size;
				if (numarg == 1) { // get name
					mytypesPtr[1].type = STRING; // set up for outputting a float
					mytypesPtr[1].datum.s = dataPtr->IO_Modules[modnumb]->name;
					WriteMessage(buf, "/modname", 2, mytypesPtr); 
					goto send_immediate;
				} else {  // numarg == 2 set name
					if (mytypesPtr[1].type == STRING) {
						strcpy(dataPtr->IO_Modules[modnumb]->name,mytypesPtr[1].datum.s);
					}
					goto skip_send; 
				}
			}
		}
		else if (!strcmp(address, "/conxin") ) {
			int connumb = mytypesPtr[0].datum.i;
			struct Node *searchnodePtr = NULL;				
			GetNodePtr( dataPtr->CircuitTree, connumb, &searchnodePtr);
			GetNodePtr( dataPtr->SamplingTree, connumb, &searchnodePtr);
			if (searchnodePtr != NULL) {
				if (numarg == 1) { // get input module by name module_no
					mytypesPtr[1].type = STRING; // set up for outputting a float
					mytypesPtr[2].type = STRING; // set up for outputting a float
					int index = searchnodePtr->Input->module_no;
					if (index < dataPtr->build.build_size) {
						mytypesPtr[1].datum.s = dataPtr->Modules[index]->name;
					}
					else {
						mytypesPtr[1].datum.s = dataPtr->IO_Modules[index-dataPtr->build.build_size]->name;
					}
						mytypesPtr[2].datum.s = searchnodePtr->Input->name;  // node connection name

					WriteMessage(buf, "/conxin", 3, mytypesPtr); 
					goto send_immediate;
				}
			}
		}
		else if (!strcmp(address, "/conxout") ) {
			int connumb = mytypesPtr[0].datum.i;
			struct Node *searchnodePtr = NULL;				
			GetNodePtr( dataPtr->CircuitTree, connumb, &searchnodePtr);
			GetNodePtr( dataPtr->SamplingTree, connumb, &searchnodePtr);
			if (searchnodePtr != NULL) {
				int k = 0, index;
				if (numarg == 1) { // get input module by name module_no
					while (searchnodePtr->Output[k] != NULL) {
						mytypesPtr[2*k + 1].type = STRING; // set up for outputting a float
						mytypesPtr[2*k + 2].type = STRING; // set up for outputting a float
						index = searchnodePtr->Output[k]->module_no;
					
						//fprintf(stderr," index %d\n", index); fflush(stderr);
						if (index < dataPtr->build.build_size)
							mytypesPtr[2*k+1].datum.s = dataPtr->Modules[index]->name;
						else mytypesPtr[2*k+1].datum.s = dataPtr->IO_Modules[index-dataPtr->build.build_size]->name;
						mytypesPtr[2*k+2].datum.s = searchnodePtr->Output[k]->name;  // node connection name
						k+=1;
					}
					if (k >=1) {
						WriteMessage(buf, "/conxout", 2*k+1, mytypesPtr); 
						goto send_immediate;
					} else goto skip_send;
				}
			}
		}
	}			
	if (numarg == 2) {
		if ((mytypesPtr[0].type==INT) && (mytypesPtr[1].type==INT) && (!strcmp(address, "/setview"))) {
			if (lock_axis_flag == FALSE) { 
				trace = tracex = 0;
				if  ((mytypesPtr[0].datum.i >= 0)  && (mytypesPtr[1].datum.i >= 0)  && (mytypesPtr[0].datum.i < 360) && (mytypesPtr[1].datum.i < 360)) {
					spinysurface = mytypesPtr[1].datum.i; 
					spinxsurface = mytypesPtr[0].datum.i;
				}
				else spinysurface = spinxsurface = 0;
				glutIdleFunc(both); 
			}	
			goto skip_send;
		}
		else if ((mytypesPtr[0].type==STRING) && (mytypesPtr[1].type==INT) && (!strcmp(address, "/find"))) { // find 
			int nth = mytypesPtr[1].datum.i;
			struct Node *searchnodePtr = NULL;
						
			GetStringNodePtr( dataPtr->SamplingTree, mytypesPtr[0].datum.s, &nth, &searchnodePtr);
//			GetStringNodePtr( dataPtr->CircuitTree, mytypesPtr[0].datum.s, &nth, &searchnodePtr);
//			if (nth > 0) { 
//				fprintf(stderr,"here\n",nth); 
//				if (dataPtr->SamplingTree == NULL) fprintf(stderr,"is NULL\n");
//				fflush(stderr);
//				GetStringNodePtr( dataPtr->SamplingTree, mytypesPtr[0].datum.s, &nth, &searchnodePtr);
//			}

			if (searchnodePtr != NULL) {
				// input
				mytypesPtr[1].type = STRING; 
				mytypesPtr[2].type = STRING; 
				
				if (searchnodePtr->Input != NULL) {
					mytypesPtr[0].type=INT;
					mytypesPtr[0].datum.i = searchnodePtr->key;
					int index = searchnodePtr->Input->module_no;
					if (index < dataPtr->build.build_size) mytypesPtr[1].datum.s = dataPtr->Modules[index]->name;
					else mytypesPtr[1].datum.s = dataPtr->IO_Modules[index-dataPtr->build.build_size]->name;
					mytypesPtr[2].datum.s = searchnodePtr->Input->name;  // node connection name
					
					
					// offset
					mytypesPtr[3].type = FLOAT; // set up for outputting a float
					mytypesPtr[3].datum.f = searchnodePtr->offset;
					// constant weight
					mytypesPtr[4].type = FLOAT; // set up for outputting a float
					mytypesPtr[4].datum.f = searchnodePtr->constant;
					// outputs
					int k = 0;
					while (searchnodePtr->Output[k] != NULL) {
						mytypesPtr[2*k + 5].type = STRING; 
						mytypesPtr[2*k + 6].type = STRING;
						int index = searchnodePtr->Output[k]->module_no;
						if (index < dataPtr->build.build_size)
							mytypesPtr[2*k+5].datum.s = dataPtr->Modules[index]->name;
						else mytypesPtr[2*k+5].datum.s = dataPtr->IO_Modules[index-dataPtr->build.build_size]->name;
						mytypesPtr[2*k+6].datum.s = searchnodePtr->Output[k]->name;  // node connection name
						k+=1;
					}
				
					WriteMessage(buf, "/conxn", 2*k + 5, mytypesPtr); 
					goto send_immediate;

				}
			}				
		}
	}
	if ((numarg == 2) || (numarg == 3))  {
		if ((mytypesPtr[0].type==INT) && (mytypesPtr[1].type==INT) && (!strcmp(address, "/modparam"))) {
			int modnumb = mytypesPtr[0].datum.i;
			int modparam = mytypesPtr[1].datum.i;			
			if ((modnumb < dataPtr->build.build_size) && (modparam < dataPtr->Modules[modnumb]->no_parameters))  { // only in processing modules
				if (numarg == 2) { // get name
					mytypesPtr[2].type = STRING; // set up for outputting a float
					mytypesPtr[2].datum.s = dataPtr->Modules[modnumb]->ParamPtr[modparam]->label;
					WriteMessage(buf, "/modparam", 3, mytypesPtr); 
					goto send_immediate;
				} /*else { // enter the new string
					if (mytypesPtr[2].type == STRING) {  // should set it up to accept integer, float or string and to process to all three as well.
						strncpy(dataPtr->Modules[modnumb]->ParamPtr[modparam]->label, mytypesPtr[2].datum.s, DEFAULT_NAME_SIZE);
						float tempfloat;
						int tempint;
						if (1 == sscanf(mytypesPtr[2].datum.s,"%f", &tempfloat)) dataPtr->currentmodule.ParamPtr[2]->value = (double) tempfloat;
						else if (1 == sscanf(mytypesPtr[2].datum.s,"%d", &tempint)) dataPtr->currentmodule.ParamPtr[2]->intvalue = (double) tempint;
					} else if (mytypesPtr[2].type == FLOAT)  {
							dataPtr->currentmodule.ParamPtr[2]->value =  (double) mytypesPtr[2].datum.f;
					} else if (mytypesPtr[2].type == INT)  {
						dataPtr->currentmodule.ParamPtr[2]->intvalue =  (double) mytypesPtr[2].datum.i;
					}
				} */
			}
		}
	}
	if ((numarg == 3) || (numarg == 4))  {
		if ((mytypesPtr[0].type==INT) && (mytypesPtr[1].type==INT) && (mytypesPtr[2].type==INT) && (!strcmp(address, "/meter"))) {
				int modnumb = mytypesPtr[0].datum.i;
				int typio = mytypesPtr[1].datum.i;
				int selport = mytypesPtr[2].datum.i;
//				fprintf(stderr, "%d %d %d %d\n", modnumb, dataPtr->build.build_size, dataPtr->build.Output_channel_no, (dataPtr->build.Output_channel_no > 0)); fflush(stderr);
// module metering			
			if ( modnumb < dataPtr->build.build_size) { // are we inthe modules or i/o
				if  (modnumb == dataPtr->Modules[modnumb]->module_no) { // does the module number correspond 
					if (typio == 0) { // input
						if ((selport >= 0 ) && (selport < dataPtr->Modules[modnumb]->no_inputs )) { //index range check
							if (numarg == 3) {
								//fprintf (stderr, "%f\n", dataPtr->Modules[modnumb]->Input[selport]->value); fflush(stderr);
								mytypesPtr[0].type = FLOAT;
								mytypesPtr[0].datum.f = dataPtr->Modules[modnumb]->Input[selport]->value;
								WriteMessage(buf, "/meter", 1, mytypesPtr); 
								goto send_immediate;
							}
							else if (mytypesPtr[3].type==STRING) {
								//fprintf (stderr, "%f %s\n", dataPtr->Modules[modnumb]->Input[selport]->value, mytypesPtr[3].datum.s); fflush(stderr);
								mytypesPtr[0].type = FLOAT;
								mytypesPtr[0].datum.f = dataPtr->Modules[modnumb]->Input[selport]->value;
								WriteMessage(buf, mytypesPtr[3].datum.s, 1, mytypesPtr); 
								goto send_immediate;
							}
						}
					}
					else if (typio == 1) { // output 
						if ((selport >= 0 ) && (selport < dataPtr->Modules[modnumb]->no_outputs )) { //index range check
							if (numarg == 3) {
								//fprintf (stderr, "%f\n",  dataPtr->Modules[modnumb]->Output[selport]->value);  fflush(stderr);
								mytypesPtr[0].type = FLOAT;
								mytypesPtr[0].datum.f = dataPtr->Modules[modnumb]->Output[selport]->value;
								WriteMessage(buf, "/meter", 1, mytypesPtr); 
								goto send_immediate;
							}
							else if (mytypesPtr[3].type==STRING) {
								//fprintf (stderr, "%f %s\n",  dataPtr->Modules[modnumb]->Output[selport]->value, mytypesPtr[3].datum.s);  fflush(stderr);
								mytypesPtr[0].type = FLOAT;
								mytypesPtr[0].datum.f = dataPtr->Modules[modnumb]->Output[selport]->value;
								WriteMessage(buf, mytypesPtr[3].datum.s, 1, mytypesPtr); 
								goto send_immediate;
							}
						}
					}
				}
			}
// input metering			
			else if ( modnumb < dataPtr->build.build_size + dataPtr->build.Input_channel_no + (dataPtr->build.Input_channel_no > 0)  ) { // are we in input section
				int index = modnumb - dataPtr->build.build_size  ; 
				if (typio == 0) { // input
					if  ((modnumb == dataPtr->IO_Modules[index]->put.In->module_no) && (selport == 0)) { // does module number correspond
						if (index == 0) {
							mytypesPtr[0].type = INT;
							mytypesPtr[0].datum.i = iframe_counter;
						} else {
							mytypesPtr[0].type = FLOAT;
							mytypesPtr[0].datum.f = dataPtr->mirror_IO_Modules[index];
						}
						if (numarg == 3) WriteMessage(buf, "/meter", 1, mytypesPtr); 
						else if (mytypesPtr[3].type==STRING) WriteMessage(buf, mytypesPtr[3].datum.s, 1, mytypesPtr); 
						goto send_immediate;
					}
				}
 			}
// output metering			
			else if ( modnumb < dataPtr->build.build_size + dataPtr->build.Output_channel_no + (dataPtr->build.Output_channel_no > 0) +  dataPtr->build.Output_channel_no + (dataPtr->build.Output_channel_no > 0) ) { // are we in the  i/o allows for the framecounter
				int index = modnumb - dataPtr->build.build_size -  dataPtr->build.Input_channel_no - (dataPtr->build.Input_channel_no > 0); 
				//fprintf(stderr, "got here %d %s %s %s %s %d\n",index, dataPtr->IO_Modules[index]->name, dataPtr->IO_Modules[index]->put.In->name, dataPtr->IO_Modules[index]->put.Out->name, dataPtr->IO_Modules[index]->put.In->label, dataPtr->IO_Modules[index]->put.Out->label, dataPtr->IO_Modules[index]->put.In->module_no, dataPtr->IO_Modules[index]->put.Out->module_no );
				if (typio == 1) { // output 
					if  ((modnumb == dataPtr->IO_Modules[index]->put.In->module_no) && (selport == 0)) { // does module number correspond 
						if (index == 0) {
							mytypesPtr[0].type = INT;
							mytypesPtr[0].datum.i = oframe_counter;
						} else {
							mytypesPtr[0].type = FLOAT;
							mytypesPtr[0].datum.f = dataPtr->mirror_Output_Modules[index];
						}
						if (numarg == 3) WriteMessage(buf, "/meter", 1, mytypesPtr); 
						else if (mytypesPtr[3].type==STRING) WriteMessage(buf, mytypesPtr[3].datum.s, 1, mytypesPtr); 
						goto send_immediate;
					}
				}
			
			}
// lug metering			
			else if ( modnumb < dataPtr->build.build_size + dataPtr->build.Input_channel_no + (dataPtr->build.Input_channel_no > 0) + dataPtr->build.Output_channel_no + (dataPtr->build.Output_channel_no > 0) + dataPtr->build.Connection_build_size) { // are we in the  i/o allows for the framecounter
				int index = modnumb - dataPtr->build.build_size -  dataPtr->build.Input_channel_no - (dataPtr->build.Input_channel_no > 0) - dataPtr->build.Output_channel_no - (dataPtr->build.Output_channel_no > 0); 
				if  ((modnumb == dataPtr->IO_Modules[index]->put.In->module_no) && (selport == 0)) { // does module number correspond
					mytypesPtr[0].type = FLOAT;
					mytypesPtr[0].datum.f = dataPtr->mirror_Lug_Modules[index];
					if (numarg == 3) WriteMessage(buf, "/meter", 1, mytypesPtr); 
					else if (mytypesPtr[3].type==STRING) WriteMessage(buf, mytypesPtr[3].datum.s, 1, mytypesPtr); 
					goto send_immediate;
				}
			}
			goto skip_send;
		}

	}
	if ((numarg >= 1) && (numarg < 6))  {  // allow for a direct module address .. caters for touchOSC to be used with any new named module
		int modnumb = 0;
		while ((modnumb < dataPtr->build.build_size) && (strcmp(&address[1], dataPtr->Modules[modnumb]->name))) modnumb++; 
		if ((modnumb < dataPtr->build.build_size) && (dataPtr->Modules[modnumb]->type == EXTERN)) {
			int j;
			for (j=0;  j < numarg; j++) {
				if (mytypesPtr[j].type == INT) dataPtr->Modules[modnumb]->Output[j]->value = mytypesPtr[j].datum.i;
				else if (mytypesPtr[j].type == FLOAT) dataPtr->Modules[modnumb]->Output[j]->value = mytypesPtr[j].datum.f;
			}
		}
	} 
	goto skip_send;

	
send_immediate:
	if (OSC_closeBundle(buf)) {
		complain("Problem closing bundle: %s\n", OSC_errorMessage);
		return;
	}

    SendBuffer(htmsocket, buf);
    goto onward;
    

skip_send:
	if (OSC_closeBundle(buf)) {
		complain("Problem closing bundle: %s\n", OSC_errorMessage);
		return;
	}

onward:

// check for other entries
	if (numarg == 1) {
		while ((strcmp(address, controldevlist[i])) && (i < N  - 1)) { i++;  } // systematically find index in the pots list
	
		if (!strcmp(address, controldevlist[i]) && (i < N)) { // if we have found one 
			if (n != 0) {
				if 	(mytypesPtr[0].type == FLOAT)  frame[i] = (float) mytypesPtr[0].datum.f;
				else if (mytypesPtr[0].type == INT) frame[i] = (float) mytypesPtr[0].datum.i;

				if (i < N_label) { // this doing the pots
						if ( i/3 < dataPtr->build.Pot_channel_no) 
							dataPtr->Modules[i/3]->ParamPtr[(i % 3)*2]->value = frame[i];
				}
				else if (i < (2*N_label)) { // and here we do the toggles
					int j = i - N_label; // get the toggle index
					if (( j/3 < dataPtr->build.Pot_channel_no) && // only process those that are catered for in a build
						 (frame[i] == 0)) { strncpy( dataPtr->Modules[j/3]->ParamPtr[(j % 3)*2 + 1]->label, "disable", 7); dataPtr->Modules[j/3]->ParamPtr[(j % 3)*2+1]->value=0; }
						else { strncpy( dataPtr->Modules[j/3]->ParamPtr[(j % 3)*2+1]->label, "enable", 6); dataPtr->Modules[j/3]->ParamPtr[(j % 3)*2+1]->value=1; } 
				}
			} 
		}
		else if (i == N) {fprintf(stderr,"OSC: not defined\n"); fflush(stderr);}
	}

}


PrintArgs(int size, struct TypeArgStruct typesPtr[]) {
	int k;

	for (k=0; k < size; k++) {
		if (typesPtr[k].type == INT) 
			fprintf(stderr,"%d \n", typesPtr[k].datum.i );
		else if (typesPtr[k].type == FLOAT) 
			fprintf(stderr,"%f \n", typesPtr[k].datum.f );		
		else if (typesPtr[k].type == STRING)
			fprintf(stderr,"\"%s\" \n", typesPtr[k].datum.s );		
	}
}


// return a count of number of parameters
int PrintTypeTaggedArgs(void *v, int n) { 
    char *typeTags, *thisType ;
    char *p;
    float value;
    int	k = 0;
    typeTags = v;
	

//    if (!IsNiceString(typeTags, typeTags + n )) {
	/* No null-termination, so maybe it wasn't a type tag
	   string after all */
//	PrintHeuristicallyTypeGuessedArgs(v, n, 0);
//	return;
//    }

    p = DataAfterAlignedString(typeTags, typeTags+n);


    for (thisType = typeTags + 1; *thisType != 0; ++thisType) {
	switch (*thisType) {
	    case 'i': case 'r': case 'm': case 'c':
	    	value = (float) ntohl(*((int *) p));
	    	mytypesPtr[k].type = INT;
	    	mytypesPtr[k].datum.i = ((int) value);
	    	k++;
	    	
//	fprintf(stderr,"%d ", (int) value);
	    p += 4;
	    break;

	    case 'f': {
		int i = ntohl(*((int *) p));
		float *floatp = ((float *) (&i));
		value = *floatp;
	    	mytypesPtr[k].type = FLOAT;
	    	mytypesPtr[k].datum.f = value;
	    	k++;
//	fprintf(stderr,"%f ", value);
		p += 4;
	    }
	// return(value);
	    break;

	    case 'h': case 't':
	    fprintf(stderr,"[A 64-bit int] "); fflush(stderr);
	    p += 8;
	    break;

	    case 'd':
	    fprintf(stderr,"[A 64-bit float] "); fflush(stderr);
	    p += 8;
	    break;

	    case 's': case 'S':
	    if (!IsNiceString(p, typeTags+n)) {
	fprintf(stderr,"Type tag said this arg is a string but it's not!\n"); fflush(stderr);
		return; 
	    } else {
	    	mytypesPtr[k].type = STRING;
	    	mytypesPtr[k].datum.s = p;
	    	k++;
//	fprintf(stderr,"\"%s\" ", p);
		p = DataAfterAlignedString(p, typeTags+n);
	    }
	    break;

	    case 'T': fprintf(stderr,"[True] "); break;
	    case 'F': fprintf(stderr,"[False] "); break;
	    case 'N': fprintf(stderr,"[Nil]"); break;
	    case 'I': fprintf(stderr,"[Infinitum]"); break;

	    default:
	    fprintf(stderr,"[Unrecognized type tag %c]", *thisType);
	    //return(value);
	}
    }
	//fprintf(stderr,"\n");fflush(stderr);
	return(k);
}



static void PrintHeuristicallyTypeGuessedArgs(void *v, int n, int skipComma) {
    int i, thisi;
    float thisf;
    int *ints;
    char *chars;
    char *string, *nextString;


    /* Go through the arguments 32 bits at a time */
    ints = v;
    chars = v;

    for (i = 0; i<n/4; ) {
	string = &chars[i*4];
	thisi = ntohl(ints[i]);
	/* Reinterpret the (potentially byte-reversed) thisi as a float */
	thisf = *(((float *) (&thisi)));

	if  (thisi >= -1000 && thisi <= 1000000) {
	    fprintf(stderr,"%d ", thisi);
	    i++;
	} else if (thisf >= -1000.f && thisf <= 1000000.f &&
		   (thisf <=0.0f || thisf >= SMALLEST_POSITIVE_FLOAT)) {
	    fprintf(stderr,"%f ",  thisf);
	    i++;
	} else if (IsNiceString(string, chars+n)) {
	    nextString = DataAfterAlignedString(string, chars+n);
	    fprintf(stderr,"\"%s\" ", (i == 0 && skipComma) ? string +1 : string);
	    i += (nextString-string) / 4;
	} else {
	    fprintf(stderr,"0x%x ", ints[i]);
	    i++;
	}
    }
}


#define STRING_ALIGN_PAD 4

char *DataAfterAlignedString(char *string, char *boundary) 
{
    /* The argument is a block of data beginning with a string.  The
       string has (presumably) been padded with extra null characters
       so that the overall length is a multiple of STRING_ALIGN_PAD
       bytes.  Return a pointer to the next byte after the null
       byte(s).  The boundary argument points to the character after
       the last valid character in the buffer---if the string hasn't
       ended by there, something's wrong.

       If the data looks wrong, return 0, and set htm_error_string */

    int i;

    if ((boundary - string) %4 != 0) {
	fprintf(stderr, "Internal error: DataAfterAlignedString: bad boundary\n");
	return 0;
    }

    for (i = 0; string[i] != '\0'; i++) {
	if (string + i >= boundary) {
	    htm_error_string = "DataAfterAlignedString: Unreasonably long string";
	    return 0;
	}
    }

    /* Now string[i] is the first null character */
    i++;

    for (; (i % STRING_ALIGN_PAD) != 0; i++) {
	if (string + i >= boundary) {
	    htm_error_string = "DataAfterAlignedString: Unreasonably long string";
	    return 0;
	}
	if (string[i] != '\0') {
	    htm_error_string = "DataAfterAlignedString: Incorrectly padded string.";
	    return 0;
	}
    }

    return string+i;
}

Boolean IsNiceString(char *string, char *boundary) 
{
    /* Arguments same as DataAfterAlignedString().  Is the given "string"
       really a string?  I.e., is it a sequence of isprint() characters
       terminated with 1-4 null characters to align on a 4-byte boundary? */

    int i;

    if ((boundary - string) %4 != 0) {
	fprintf(stderr, "Internal error: IsNiceString: bad boundary\n");
	return 0;
    }

    for (i = 0; string[i] != '\0'; i++) {
	if (!isprint(string[i])) return FALSE;
	if (string + i >= boundary) return FALSE;
    }

    /* If we made it this far, it's a null-terminated sequence of printing characters 
       in the given boundary.  Now we just make sure it's null padded... */

    /* Now string[i] is the first null character */
    i++;
    for (; (i % STRING_ALIGN_PAD) != 0; i++) {
	if (string[i] != '\0') return FALSE;
    }

    return TRUE;
}



#define MAXMESG2 32768
static char mbuf[MAXMESG2];




//#define max(a,b) (((a) > (b)) ? (a) : (b))

//int main(int argc, char **argv) {

void monitorUDPio( struct xml_data *dataPtr) {
    
    int i;
		
    struct sockaddr_in cl_addr;

    int clilen,maxclilen=sizeof(cl_addr);

    int n;
	
    clilen = maxclilen;

trace = 0.0;
tracex = 0.0;

//	n = recvfrom(0, mbuf, MAXMESG2, 0, (struct sockaddr *) &cl_addr, (socklen_t *) &clilen);
//	if(n>0)
//	{	
//		sockfd = 0;
//		portrecnumber = -1;
//		Synthmessage(mbuf, n, &cl_addr, clilen,sockfd, dataPtr) ;
//	}
//	else
//	{	

//		sockfd=initudp(portrecnumber);
//	}

	sockfd=initudp(portrecnumber);
    if (sockfd < 0) {
      perror("initudp");
    } else {
		fd_set read_fds, write_fds;
		int nfds;
		nfds = sockfd+ 1;
		{
			int j;
			for(j=0;j<npolldevs;++j)
				if(polldevs[j].fd>=nfds)
				{
						nfds = polldevs[j].fd+1;
				}
		}

		caught_sigint = 0;

		/* Set signal handler */
		signal(SIGINT, catch_sigint);
	
		while(!caught_sigint)
		{
			
			int r;
 	
		back:	

			FD_ZERO(&read_fds);                /* clear read_fds        */
			FD_ZERO(&write_fds);               /* clear write_fds        */
			FD_SET(sockfd, &read_fds); 
		       	{
		        	int j;
		        	
		        	for(j=0;j<npolldevs;++j)
		        		FD_SET(polldevs[j].fd, &read_fds);
		        }
 	      
		        r = select(nfds, &read_fds, &write_fds, (fd_set *)0, 
		                        (struct timeval *)0);
		        if (r < 0)  /* select reported an error */
			  return;
		        {
			    int j;
			    
			    for(j=0;j<npolldevs;++j)
				    if(FD_ISSET(polldevs[j].fd, &read_fds))
				    (*(polldevs[j].callbackfunction))(polldevs[j].fd,polldevs[j].dummy );
		        }


		        if(FD_ISSET(sockfd, &read_fds))
			{
				clilen = maxclilen;
				while( (n = recvfrom(sockfd, mbuf, MAXMESG2, 0, (struct sockaddr *) &cl_addr, (socklen_t *) &clilen)) >0) 
				{
					       
					 if (updateflag == FALSE) { 										// only enters this the very first key press
					   //printf("got packet from %s\n\n\n", inet_ntoa(cl_addr.sin_addr));
					   snprintf(&hostname[0], 100, "%s", inet_ntoa(cl_addr.sin_addr));
						updateinterface(hostname, portsndnumber, dataPtr); updateflag = TRUE;			// now initialise the controls
					}
					else {
	
						  int r;
						  r = Synthmessage(mbuf, n, &cl_addr, clilen, sockfd, dataPtr) ; //  passing in xml data structure
		
						  if( sgi_HaveToQuit()) return;
						  if(r>0) goto back;
						  clilen = maxclilen;
					}
				}
			}



		} /* End of while(!caught_sigint) */
    }

    return ;
}


