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


/************************************************************************************/
/*	PERTECS																			*/
/*	Programmable environment for Real-Time Emulation of Continuous Systems			*/
/*																					*/
/************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pertecs.h"

/************************************************************************************/
// output module info
/************************************************************************************/
void first (FILE *file, struct Node *nodePtr, struct xml_data *dataPtr, int mod_no, char *module, char *label, char *name, int index ) {

	if ((name != NULL) && (dataPtr->Modules[mod_no]->name != NULL)) fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", module, dataPtr->Modules[mod_no]->name  ); // we want the module label here
	if ( dataPtr->suppressnamesflag == FALSE) 
		if ((name != NULL) && (nodePtr->Output[index]->label != NULL)) fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", label,  nodePtr->Output[index]->label);  // signal name is here
	if ((name != NULL) && (nodePtr->Output[index]->name != NULL)) fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", name, nodePtr->Output[index]->name ); // we want output designator
}

// output i/o info
// should finish off the checking of the pointers before working with them.... 
void second (FILE *file, struct xml_data *dataPtr, int mod_no, char *module, char *label, char *name) {
	mod_no -= dataPtr->build.build_size ;
	if ((dataPtr->build.Input_channel_no > 0) && (mod_no >= 0 ) && (mod_no <= dataPtr->build.Input_channel_no )) {
		fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", module, dataPtr->IO_Modules[mod_no]->name  ); //  module label here
			fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", label, dataPtr->IO_Modules[mod_no]->put.In->label ); // signal-name  here 
			fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", name, dataPtr->build.Input_out_name ); //  output name
		mod_no = -1;

	}
	else if (dataPtr->build.Input_channel_no > 0) mod_no = mod_no - dataPtr->build.Input_channel_no - 1;
	
	if ((dataPtr->build.Output_channel_no > 0) && (mod_no >= 0 ) &&  (mod_no <= dataPtr->build.Output_channel_no )) {
			fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", module, dataPtr->Output_Modules[mod_no]->name  ); // we want the module label here
			fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", label, dataPtr->Output_Modules[mod_no]->put.Out->label  ); // signal name is here
			fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", name, dataPtr->build.Output_in_name); // input name for the module
		mod_no = -1;
	}
	else if (dataPtr->build.Output_channel_no > 0) mod_no = mod_no - dataPtr->build.Output_channel_no - 1;  //  allow for the frame counter
	
	if ((dataPtr->build.Connection_build_size > 0) && (mod_no >= 0 ) &&  (mod_no < dataPtr->build.Connection_build_size )) {
			fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", module, dataPtr->lug_Modules[mod_no]->name  ); // we want the module label here
			fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", label, dataPtr->lug_Modules[mod_no]->put.In->label  ); // signal name is here
			fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", name, dataPtr->build.Connection_lug_name); // input name for the module
		mod_no = -1;
	}
}


/************************************************************************************/
// input/output
/************************************************************************************/
void second_code (FILE *file, struct xml_data *dataPtr, int mod_no, char *module, char *label, char *name) {
	mod_no -= dataPtr->build.build_size ;
	if ((dataPtr->build.Input_channel_no > 0) && (mod_no >= 0 ) && (mod_no <= dataPtr->build.Input_channel_no )) {
			fprintf (file, "\tDataPtr->Input_Frame[%d]->value", mod_no); // signal-name  here 
		mod_no = -1;
	}
	else if (dataPtr->build.Input_channel_no > 0) mod_no = mod_no - dataPtr->build.Input_channel_no - 1;
	
//	if ((dataPtr->build.Pot_channel_no > 0) && (mod_no >= 0 ) && (mod_no < dataPtr->build.Pot_channel_no )) {
//			fprintf (file, "\tDataPtr->Pot_Frame[%d]->value", mod_no); // signal-name  here 
//		mod_no = -1;
//	}
//	else mod_no -= dataPtr->build.Pot_channel_no;  //  

	if ((dataPtr->build.Output_channel_no > 0) && (mod_no >= 0 ) &&  (mod_no <= dataPtr->build.Output_channel_no )) {
			fprintf (file, "\tDataPtr->Output_Frame[%d]->value", mod_no ); // signal name is here
		mod_no = -1;
	}
	else if (dataPtr->build.Output_channel_no > 0) mod_no = mod_no - dataPtr->build.Output_channel_no - 1;
	
	if ((dataPtr->build.Connection_build_size > 0) && (mod_no >= 0 ) &&  (mod_no <= dataPtr->build.Connection_build_size )) {
			fprintf (file, "\tDataPtr->Connection_Frame[%d]->value", mod_no ); // signal name is here
		mod_no = -1;
	}
	
}

/********************************************************************************************************************/
/* Circuits																													*/
/******************> visit node <*****************																	*/
/*	processing data as one visits the tree																			*/
/********************************************************************************************************************/
void VisitXML(FILE *file, struct Node *nodePtr, int how, struct xml_data *dataPtr) {
	int i, mod_no;
	if (nodePtr->key < 0) return;
	
	
	if ((nodePtr->offset == 0 ) && (nodePtr->constant == 0 )) {
		fprintf(stderr, "%d\t[%s] has open connection \n", nodePtr->key, nodePtr->Input->label); fflush(stderr);
	}
	else {
		switch (how) {
			case ID:
			break;

			case CCODE: // this was to output connections as compilable code....
				//fprintf (stderr,"//%s %d\n", "No", nodePtr->key ); fflush(stderr); //  connection no //debug
				
				mod_no = nodePtr->Input->module_no;
				
				if (( mod_no >= 0 ) && (mod_no < dataPtr->build.build_size)) {
					//which = which_one();
					fprintf (file, "\tDataPtr->Modules[%d]->Input[%d]->value += ", mod_no, nodePtr->Input->module_node  );  // input signal name is here
				}
				else { second_code(file, dataPtr, mod_no, TAG_CON_INPUT_MOD_LBL, TAG_CON_INPUT_LABLE, TAG_CON_INPUT_NAME); fprintf (file, " += "); }

				if (nodePtr->offset != 0 )  { fprintf (file, "%g ",  nodePtr->offset ); fflush(stdout); }
				if ((nodePtr->offset != 0 ) && (nodePtr->constant != 0 )) { fprintf (file, "+ "); }
				if (nodePtr->constant != 0.0 ) {
					if (nodePtr->constant != 1.0 )   fprintf (file, "%g * ", nodePtr->constant ); 
					fflush(file);
					if (nodePtr->Output[0]  != NULL)  {  // fprintf (file, "%s",nodePtr->Output[0]->label); }
						mod_no = nodePtr->Output[0]->module_no;
					 	if (( mod_no >= 0 ) && (mod_no < dataPtr->build.build_size )) 	fprintf (file, "DataPtr->Modules[%d]->Output[%d]->value ", mod_no, nodePtr->Output[0]->module_node ); 
					 	else second_code(file, dataPtr, mod_no, TAG_CON_OUTPUT1_MOD_LBL, TAG_CON_OUTPUT1_LBL, TAG_CON_OUTPUT1_NAME);
					}
				} 
				fprintf (file, ";\n" ); // end of line
			break;
						
			case NAME: // connection XML with names 
				if (nodePtr->Input != NULL) {
					fprintf (file,"\t<dict>\n");
					fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%d</string>\n", TAG_CON_NO, nodePtr->key ); // uses  key to indicate the module number ? or connection no ?
					if ( dataPtr->suppressnamesflag == FALSE)  fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%d</string>\n", TAG_CON_INPUT_NO, nodePtr->Input->module_no ); //  the input module number
					mod_no = nodePtr->Input->module_no;
	
					// figure out if we are in module space or input/output space
					 if (( mod_no >= 0 ) && (mod_no < dataPtr->build.build_size)) { // in module space 
						fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", TAG_CON_INPUT_MOD_LBL, dataPtr->Modules[mod_no]->name  ); //  the module label here
						if ( dataPtr->suppressnamesflag == FALSE) 
							fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", TAG_CON_INPUT_LABLE, nodePtr->Input->label  ); // input signal name is here
						fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", TAG_CON_INPUT_NAME, nodePtr->Input->name ); //  want the input designator.
						
					 } 
					 else second(file, dataPtr, mod_no, TAG_CON_INPUT_MOD_LBL, TAG_CON_INPUT_LABLE, TAG_CON_INPUT_NAME);
	
					if (nodePtr->offset != 0 )  fprintf (file, "\t\t<key>%s</key>\n\t\t<real>%g</real>\n", TAG_CON_OFFSET,  nodePtr->offset ); 
					if (nodePtr->constant != 0 ) {
						fprintf (file, "\t\t<key>%s</key>\n\t\t<real>%g</real>\n", TAG_CON_WEIGHT,  nodePtr->constant );
					// this is horrid .. but better than it was
	//1//
						if (nodePtr->Output[0]  != NULL) {
							if ( dataPtr->suppressnamesflag == FALSE) fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%d</string>\n", TAG_CON_OUTPUT1_NO, nodePtr->Output[0]->module_no );														// module number 
							 mod_no = nodePtr->Output[0]->module_no;
							if (( mod_no >= 0 ) && (mod_no < dataPtr->build.build_size )) first (file, nodePtr, dataPtr, mod_no, TAG_CON_OUTPUT1_MOD_LBL, TAG_CON_OUTPUT1_LBL, TAG_CON_OUTPUT1_NAME, 0); 
							else second(file, dataPtr, mod_no, TAG_CON_OUTPUT1_MOD_LBL, TAG_CON_OUTPUT1_LBL, TAG_CON_OUTPUT1_NAME);
						}
	
	//2//
						if (nodePtr->Output[1]  != NULL) {
							if ( dataPtr->suppressnamesflag == FALSE) fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%d</string>\n", TAG_CON_OUTPUT2_NO, nodePtr->Output[1]->module_no ); 															// module number 
							mod_no = nodePtr->Output[1]->module_no;
							if (( mod_no >= 0 ) && (mod_no < dataPtr->build.build_size ))  first (file, nodePtr, dataPtr, mod_no, TAG_CON_OUTPUT2_MOD_LBL, TAG_CON_OUTPUT2_LBL, TAG_CON_OUTPUT2_NAME, 1);
							else second(file, dataPtr, mod_no, TAG_CON_OUTPUT2_MOD_LBL, TAG_CON_OUTPUT2_LBL, TAG_CON_OUTPUT2_NAME);
						}
						
	//3//
						if (nodePtr->Output[2]  != NULL) {
							if ( dataPtr->suppressnamesflag == FALSE) fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%d</string>\n", TAG_CON_OUTPUT3_NO, nodePtr->Output[2]->module_no ); 														// module number 
							mod_no = nodePtr->Output[2]->module_no;
							if (( mod_no >= 0 ) && (mod_no < dataPtr->build.build_size )) first (file, nodePtr, dataPtr, mod_no, TAG_CON_OUTPUT3_MOD_LBL, TAG_CON_OUTPUT3_LBL, TAG_CON_OUTPUT3_NAME, 2);
							else second(file, dataPtr, mod_no, TAG_CON_OUTPUT3_MOD_LBL, TAG_CON_OUTPUT3_LBL, TAG_CON_OUTPUT3_NAME);
						}
						
	//4//
						if (nodePtr->Output[3]  != NULL) {
							if ( dataPtr->suppressnamesflag == FALSE) fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%d</string>\n", TAG_CON_OUTPUT4_NO, nodePtr->Output[3]->module_no ); 														// module number 
							mod_no = nodePtr->Output[3]->module_no;
							if (( mod_no >= 0 ) && (mod_no < dataPtr->build.build_size )) first (file, nodePtr, dataPtr, mod_no, TAG_CON_OUTPUT4_MOD_LBL, TAG_CON_OUTPUT4_LBL, TAG_CON_OUTPUT4_NAME, 3);
							else second(file, dataPtr, mod_no, TAG_CON_OUTPUT4_MOD_LBL, TAG_CON_OUTPUT4_LBL, TAG_CON_OUTPUT4_NAME);
						}
						
	//5//
						if (nodePtr->Output[4]  != NULL) {
							if ( dataPtr->suppressnamesflag == FALSE) fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%d</string>\n", TAG_CON_OUTPUT5_NO, nodePtr->Output[4]->module_no ); 															// module number 
							mod_no = nodePtr->Output[4]->module_no;
							if (( mod_no >= 0 ) && (mod_no < dataPtr->build.build_size )) first (file, nodePtr, dataPtr, mod_no, TAG_CON_OUTPUT5_MOD_LBL, TAG_CON_OUTPUT5_LBL, TAG_CON_OUTPUT5_NAME, 4);
							else second(file, dataPtr, mod_no, TAG_CON_OUTPUT5_MOD_LBL, TAG_CON_OUTPUT5_LBL, TAG_CON_OUTPUT5_NAME);
						}
						
	//6//
						if (nodePtr->Output[5]  != NULL) {
							if ( dataPtr->suppressnamesflag == FALSE) fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%d</string>\n", TAG_CON_OUTPUT6_NO, nodePtr->Output[5]->module_no ); 														// module number 
							mod_no = nodePtr->Output[5]->module_no;
							if (( mod_no >= 0 ) && (mod_no < dataPtr->build.build_size )) first (file, nodePtr, dataPtr, mod_no, TAG_CON_OUTPUT6_MOD_LBL, TAG_CON_OUTPUT6_LBL, TAG_CON_OUTPUT6_NAME, 5);
							else second(file, dataPtr, mod_no, TAG_CON_OUTPUT6_MOD_LBL, TAG_CON_OUTPUT6_LBL, TAG_CON_OUTPUT6_NAME);
						} 
	
	
	
					}
					fprintf (file,"\t</dict>\n"); 
				}
			break;
		
			case VALUE: // initialisation 
				fprintf (file,"\t<dict>\n");
/*				fprintf (file, "\t\t<key>offset</key>\n"); 
				fprintf (file, "\t\t<real>%g</real>\n",  nodePtr->Output[0]->value ); 
				fprintf (file, "\t\t<key>input</key>\n"); 
				fprintf (file, "\t\t<string>[%s]</string>\n", nodePtr->Input->label ); 
				
*/
				fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%d</string>\n", TAG_CON_NO, nodePtr->key ); // uses  key to indicate the module number ? or connection no ?
				fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%d</string>\n", TAG_CON_INPUT_NO, mod_no = nodePtr->Input->module_no ); //  the input module number
				// figure out if we are in module space or input/output space
					 if (( mod_no >= 0 ) && (mod_no < dataPtr->build.build_size)) {
						fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", TAG_CON_INPUT_MOD_LBL, dataPtr->Modules[mod_no]->name  ); //  the module label here
							fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", TAG_CON_INPUT_LABLE, nodePtr->Input->label  ); // input signal name is here
							fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", TAG_CON_INPUT_NAME, nodePtr->Input->name ); //  want the input designator .

					 } 
					 else { // in input output space
					 	mod_no -= dataPtr->build.build_size ;
						if (( mod_no > 0) && (mod_no <= dataPtr->build.Input_channel_no) ) { // inputs
							fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", TAG_CON_INPUT_MOD_LBL, dataPtr->IO_Modules[mod_no]->name  ); //  module label here
								fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", TAG_CON_INPUT_LABLE, dataPtr->IO_Modules[mod_no]->put.In->label ); // signal-name  here 
								fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", TAG_CON_INPUT_NAME, dataPtr->build.Input_out_name ); //  output name
						}
						else { // outputs
							mod_no -= dataPtr->build.Input_channel_no + 1;
							fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", TAG_CON_INPUT_MOD_LBL, dataPtr->Output_Modules[mod_no]->name  ); // we want the module label here
									fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", TAG_CON_INPUT_LABLE, dataPtr->Output_Modules[mod_no]->put.Out->label  ); // signal name is here
						 			fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", TAG_CON_INPUT_NAME, dataPtr->build.Output_in_name); // input name for the module
					 	}
					 }

				fprintf (file, "\t\t<key>%s</key>\n\t\t<real>%g</real>\n", TAG_CON_OFFSET,  nodePtr->offset );
				fprintf (file, "\t\t<key>%s</key>\n\t\t<string>%s</string>\n", TAG_CON_WEIGHT,  " " );
				fprintf (file,"\t</dict>\n");  

			break;
			default:
			break;
		}

	}
}

/********************************************************************************************************************/
/*																													*/
/******************> Descend Inorder <****************																*/
/*																													*/
/********************************************************************************************************************/

void	InorderXML(FILE *file, struct Node *nodePtr, int how, struct xml_data *dataPtr)
{
	if ( nodePtr == NULL )
		return;
	
	InorderXML(file, nodePtr->leftPtr, how, dataPtr);
	VisitXML(file, nodePtr, how, dataPtr);
	InorderXML(file, nodePtr->rightPtr, how, dataPtr);
}

/********************************************************************************************************************/
/*																													*/
/*	Outputting an XML connectionfile from the tree																	*/
/*																													*/
/********************************************************************************************************************/
void Inorder(FILE *file, struct Node *nodePtr, int how , struct xml_data *dataPtr)
{
	fprintf (file,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n<plist version=\"1.0\">\n<array>\n");
	InorderXML(file, nodePtr,  how , dataPtr);
	fprintf (file,"</array>\n</plist>\n");
	fflush(file);
}



/****************************************************************************/
/*																			*/
/*  Tree routines to support initialisation, interconnection & sampling		*/
/*																			*/
/****************************************************************************/

/******************> create tree <*****************/
void	create( struct Node **curNodePtrPtr ) {

	int i;
	struct Node *NodePtr;
	
	if (*curNodePtrPtr == NULL) {
		NodePtr = malloc( sizeof( struct Node ) );	 //create the root node
		if ( NodePtr == NULL ) fprintf(stderr,"Could not allocate memory!\n" );
	} 
	
	NodePtr->key = -1;
	NodePtr->Input = NULL;
	NodePtr->offset = 0.0;
	NodePtr->constant = 0.0;
	for (i=0; i < MAX_PRODUCT; i++) NodePtr->Output[i] = NULL;	   
		
	NodePtr->leftPtr = NULL;
	NodePtr->rightPtr = NULL;
	*curNodePtrPtr = NodePtr;
}

/****************************************************************************/
/*																			*/
/* push Field to stack														*/
/*																			*/
/****************************************************************************/
void pushto(struct stack *stackPtr, struct field *Field) {
	stackPtr->Field[stackPtr->pointer] = Field;
	stackPtr->pointer++; // starts at zero and left pointing to the next available position
	if ( MAX_STACK_SIZE < stackPtr->pointer) { fprintf (stderr, "stack depth exceeded\n"); exit (-1); }
}

/*set stack value*/
void set_offset(double number, struct stack *stackPtr) {
	stackPtr->offset = number;
}

/*set global value*/
void set_constant(double number, struct stack *stackPtr) {
	stackPtr->constant = number;
}

/****************************************************************************/
/*																			*/
/* pop Field from stack, stop at empty 										*/
/*																			*/
/****************************************************************************/
struct field *pop(struct stack *stackPtr) {
	if (stackPtr->pointer >  0) {
		stackPtr->pointer--;
		return ((struct field *) stackPtr->Field[stackPtr->pointer] );
	}
	else return ((struct field *) NULL);
}


/****************************************************************************/
/*																			*/
/* Free Tree						 										*/
/*																			*/
/****************************************************************************/

void	freeTree(struct Node *nodePtr )
{
	if ( nodePtr == NULL )
		return;
	
	freeTree(nodePtr->leftPtr);
	freeTree(nodePtr->rightPtr);
	free(nodePtr);
}



/****************************************************************************/
/*																			*/
/* add connection to tree													*/
/*																			*/
/****************************************************************************/
// pre order adding... from XML derived stack  //
void	add_connection( struct Node *nodePtr, int key, struct stack *stackPtr ) {
	int index;

	char *presentname;

	index = stackPtr->pointer-1;
	presentname = (stackPtr->Field[index])->name;
	
	if ( nodePtr == NULL ) {
		fprintf (stderr, "hit NULL, returning\n"); fflush(stderr);
		return;
	}
	if (stackPtr->pointer == 0) {
		fprintf (stderr, "null entry in stack\n"); fflush(stderr);
		return;
	}

	if (nodePtr->key == -1 ) { 						// if  empty node content then fill with the data
		nodePtr->leftPtr = nodePtr->rightPtr = NULL; 

		nodePtr->Input = pop(stackPtr); 
		nodePtr->offset = stackPtr->offset;					// that which was derived from the current breath
		nodePtr->constant = stackPtr->constant; 
		if (stackPtr->constant) { 								// only add pointers if weight is non-zero
			while (stackPtr->pointer > 0 ) {
				index = stackPtr->pointer-1;
				nodePtr->Output[index] = pop(stackPtr);
			}
		}
		else {
			while (stackPtr->pointer > 0 ) {
				index = stackPtr->pointer-1;
				pop(stackPtr); nodePtr->Output[index] = NULL; // Empty the stack however...
			}
		}

		nodePtr->key = key; //  last thing to be updated.. since its first to be checked indicates node is assigned

		return;
	}
  
	if ( key < nodePtr->key) {
		// go left
		if (nodePtr->leftPtr == NULL) create ( &nodePtr->leftPtr );
		add_connection( nodePtr->leftPtr, key, stackPtr );
	}
	else { 
		// go right
		if (nodePtr->rightPtr == NULL) create ( &nodePtr->rightPtr );
		add_connection( nodePtr->rightPtr, key, stackPtr );
	}
}





/****************************************************************************/
/*																			*/
/* "remove" connection from tree											*/
/*																			*/
/****************************************************************************/
//specified by key
void Disconnection( struct Node *nodePtr, int key) {
	int i;
	if (( nodePtr == NULL ) || (key == -1)) return;
	


	if (nodePtr->key == key ) { 						// doesn't delete  node .. leaves node and key in the tree
		nodePtr->Input = NULL;
		nodePtr->offset = 0.0;
		nodePtr->constant = 0.0;
		for (i=0; i < MAX_PRODUCT; i++) nodePtr->Output[i] = NULL;
		fprintf (stderr, "disconnected %d \n", nodePtr->key); fflush(stderr);
		key = -1;
	}

	Disconnection( nodePtr->leftPtr, key);
	Disconnection( nodePtr->rightPtr, key);


}


/******************> restore connection in tree <*****************/
//specified by key (not used ... ?)
void Reconnection( struct Node *nodePtr, int key, struct stack *stackPtr) { // needs to put in the pointers and weights
	int i, index;
	if (( nodePtr == NULL ) || (key == -1)) return;
	


	if (nodePtr->key == key ) { 						// have an existing slot to fill
		nodePtr->key == -1;								// temproarily remove reference while repopulating
		fprintf (stderr, "restoring connection %d \n", key); fflush(stderr);

		nodePtr->offset = stackPtr->offset;					// that which was derived from the current breath		
		nodePtr->constant = stackPtr->constant; 
		fprintf (stderr, "offset %f constant %f \n", stackPtr->offset, stackPtr->constant); fflush(stderr);
		nodePtr->Input = pop(stackPtr); 				// repopulate the slot
		if (stackPtr->constant) { 								// only add pointers if weight is non-zero
			while (stackPtr->pointer > 0 ) {
				index = stackPtr->pointer-1;
				nodePtr->Output[index] = pop(stackPtr);
			}
		}
		else {
			while (stackPtr->pointer > 0 ) {
				index = stackPtr->pointer-1;
				pop(stackPtr); nodePtr->Output[index] = NULL; // Empty the stack however...
			}
		}
		nodePtr->key = key;
		key = -1;
	}

	Reconnection( nodePtr->leftPtr, key, stackPtr);
	Reconnection( nodePtr->rightPtr, key, stackPtr);


}



/******************> get node  corresponding to a given key <*****************/
//connection identified by key
void GetNodePtr( struct Node *nodePtr, int key, struct Node **foundnodePtr) { // needs to put in the pointers and weights

	if (( nodePtr == NULL ) || (key == -1)) return;
	
	if (nodePtr->key == key ) {
		*foundnodePtr = nodePtr;
		key = -1;
	}

	GetNodePtr( nodePtr->leftPtr, key, foundnodePtr);
	GetNodePtr( nodePtr->rightPtr, key, foundnodePtr);
}

/******************> get node  corresponding to a label <*****************/
//nth connection identified by string label and count = 1,2 etc, count = 0 indicates a successful find
void GetStringNodePtr( struct Node *nodePtr, char *searchstring, int *count, struct Node **foundnodePtr) { // needs to put in the pointers and weights


	if (( nodePtr == NULL ) || (*count <= 0) || (nodePtr->key == -1)) {
 		return;
 	}
	
	int n=strlen(searchstring);
	if (nodePtr->Input == NULL) {
		return;
	}

	if ((!strncmp( nodePtr->Input->name, searchstring, n)) || (!strncmp(nodePtr->Input->label,searchstring, n)) ) {		
		*count -= 1;
		if (*count == 0) *foundnodePtr = nodePtr;

	}
	int i=0;
	while ((i <= 5) && (nodePtr->Output[i] != NULL) && (*count != 0)) {
		if ((!strncmp( nodePtr->Output[i]->name, searchstring, n)) || (!strncmp(nodePtr->Output[i]->label,searchstring, n)) ) {
			*count -= 1;
			if (*count == 0) *foundnodePtr = nodePtr;
		}
		i++;
	}
   

	GetStringNodePtr( nodePtr->leftPtr, searchstring, count, foundnodePtr);
	GetStringNodePtr( nodePtr->rightPtr, searchstring, count, foundnodePtr);
}


/******************> set connection offset <*****************/
//connection identified by key
void SetOffset( struct Node *nodePtr, int key, double offset) { // needs to put in the pointers and weights
	if (( nodePtr == NULL ) || (key == -1)) return;
	
	if (nodePtr->key == key ) { 
		nodePtr->offset = offset; 
		key = -1;
	}

	SetOffset( nodePtr->leftPtr, key, offset);
	SetOffset( nodePtr->rightPtr, key, offset);


}

/******************> align the module names with module number <*****************/
//for a given module number ... the connection tree is parsed to update the corresponding module name in each connection
void UpdateModName( struct Node *nodePtr, int modnumb, char *name) { // needs to put in the pointers and weights
	int i;
	if ( nodePtr == NULL )  return;
	
	if (nodePtr->Input != NULL) {
		if (nodePtr->Input->module_no == modnumb ) {
			//nodePtr->Input->name = name;
		}
	}
	for (i=0; i < 6; i++) { //checkall possible output connections
		if (nodePtr->Output != NULL) {
			if (nodePtr->Output[i]->module_no == modnumb ) {
				//nodePtr->Output[i]->name = name;
			}
		}
	}

	UpdateModName( nodePtr->leftPtr, modnumb, name);
	UpdateModName( nodePtr->rightPtr, modnumb, name);
}


/******************> sampling  <*****************/

void Sampling(struct Node *nodePtr)
/*===============================*/
{
  int i ;

	if (nodePtr == NULL    // nonexistent node
		|| nodePtr->key < 0)  // unassigned node... is ignored
		return ;

	Sampling(nodePtr->leftPtr) ;

	if (nodePtr->Input != NULL) {
		if (nodePtr->constant != 0) {
		double t = nodePtr->constant * nodePtr->Output[0]->value ;
		for (i = 1 ;  i < MAX_PRODUCT && nodePtr->Output[i] != NULL ;  ++i)
		  t *= nodePtr->Output[i]->value ;
		nodePtr->Input->value += nodePtr->offset + t ; 
		}
		else nodePtr->Input->value += nodePtr->offset ;
	}
	Sampling(nodePtr->rightPtr) ;
}


