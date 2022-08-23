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

/* ==========================================================================
                            DISPLAY_C
=============================================================================

 ============================================================================ */

#include <math.h>           /* C header for any math functions               */
#include <stdio.h>          /* C header for standard I/O                     */
#include <string.h>         /* For String compare                            */
#include <stdlib.h>
#include <GLUT/glut.h>	    /* OpenGl headers				     */

#include "pertecs.h"	   /* local header for the face		             */

void calc_normal ( float *p1, float *p2, float *p3, float *norm );




 void
 drawText3D(float x, float y, float z, char *string)
 {
	int len, i;
		len = (int) strlen(string);
	
		glRasterPos3f(x, y, z);
		
		for (i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, string[i]);
		
		}
 }

/* void
 output(GLfloat x, GLfloat y, GLfloat z, char *text)
 {
   char *p;

   glPushMatrix();
   glColor3f ( 1,  1, 0 ) ;
   glTranslatef(x, y, z);
   for (p = text; *p; p++)
	 glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
   glPopMatrix();
 }
*/



/* ========================================================================= */  
/* screentext                         		                                 */
/* ========================================================================= */  
/*
*/
void drawSquare(int l, int r, int xlen, int ylen)
{
	glBegin(GL_QUADS);
     glVertex2i(l, r);
     glVertex2i( l + xlen, r);
     glVertex2i( l + xlen, r + ylen);
     glVertex2i(l, r + ylen);
    glEnd();
}

/* ========================================================================= */  
/* screentext                         		                                 */
/* ========================================================================= */  
/*
*/
void drawText2D(const char * text, int xpos, int ypos, int top, int bottom, int left, int right)
{
	int inc = glutBitmapWidth(GLUT_BITMAP_8_BY_13, 'M');
	
	if ((ypos <= top) && (ypos > bottom)){
		while (((xpos < left)) && (*text) ) { 
			text++; xpos += inc;
			if (xpos > right) break;
		}
		if ((xpos >= left) && (*text)) {
			glWindowPos2i(xpos, ypos);
			xpos += inc;
			while ((*text) && (xpos  < right )) {
				glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *text++);
				xpos += inc;
			}
		}
	}
}

//#define g_x  winSize_x
//#define g_y  winSize_y


/* ========================================================================= */  
/* screentext                         		                                 */
/* ========================================================================= */  
/*
*/
void drawMenu()
{
	static char buffer[255];
	int tx, g_x ;
	int ty, g_y ;
	int tinc, top, bottom, left, right ;

	g_x = glutGet	(GLUT_WINDOW_WIDTH);
	g_y = glutGet	(GLUT_WINDOW_HEIGHT);
	
	left = g_x/20;
	right = g_x - g_x/20;
	
	tinc = glutBitmapWidth(GLUT_BITMAP_8_BY_13, 'M') * 2;
	tx = g_x/10 - xtextstart ;
	top = g_y - g_y/20 - tinc; // set the top line
	bottom = g_y/20 ;
	ty = top - ytextstart ;

	/* save the current matrices */
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glDisable		( GL_LIGHTING ) ;

	 glLoadIdentity();
	 gluOrtho2D(0, g_x, 0, g_y); /* set projection to map pixels */

	 glColor4ub(0x80, 0x80, 0x80, 0x40);
	 drawSquare(g_x/20, g_y/20, g_x - g_x/10, g_y - g_y/10);
	 glColor4f(.5, 1.0, 0.5, 0.7);

	 drawText2D("PERTECS Help", tx ,ty , top, bottom, left, right);
	 drawText2D(" ?     toggle help display overlay", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D("  esc - exit program", tx, ty -= tinc , top, bottom, left, right);
	 drawText2D("  t/T - tumble clockwise/anticlockwise", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D("  B - both tumble and roll", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D("    - (space) pause", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" lft mouse button -  orientation", tx, ty -= tinc, top, bottom, left, right);


	 drawText2D("GRATICULE", tx, ty -= 2 * tinc, top, bottom, left, right);
	 drawText2D(" shft + mouse -  graticule lighting", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" shft 1 (!)  brighten xy graticule", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" shft 2 (@)  lighten  xy graticule", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" shft 3 (#)  brighten xz graticule", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" shft 4 ($)  lighten  xz graticule", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" shft 5 (%)  brighten yz graticule", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" shft 6 (^)  lighten  yz graticule", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" shft 7 (&)  toggle origin left/centre", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" shft 8 (*)  label graticule axes", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" 0-9   toggle display of corresponding scope traces", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" P     toggle background colour", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" O     output screen states", tx, ty -= tinc, top, bottom, left, right);


	 drawText2D("DATA OVERLAY", tx, ty -= 2 * tinc, top, bottom, left, right);
	 drawText2D(" I     toggle text display of state variables", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" ctrl + mouse (rht mouse button) move text overlay", tx, ty -= tinc, top, bottom, left, right);
	 
	 drawText2D("ARROW KEYS", tx, ty -= 2 * tinc, top, bottom, left, right);
	 drawText2D(" rht  -> :  move text overlay to left", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" lft  <- :  move text overlay to right", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" up arow :  move text overlay down", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" dn arow :  move text overlay up", tx, ty -= tinc, top, bottom, left, right);
	 drawText2D(" home    :  move text overlay to home position", tx, ty -= tinc, top, bottom, left, right);


/*      fprintf ( stderr,"Mouse actions:\n");
      fprintf ( stderr,"  left btn :  rotate field of view\n");
      fprintf ( stderr,"  midl btn :  light source 1 angle\n");
      fprintf ( stderr,"  rght btn :  animation counter set\n");
      fprintf ( stderr,"  shft mdl :  light source 2 angle\n");
      fprintf ( stderr,"  shft lft :  light source 3 angle\n\n");
      
      fprintf ( stderr," Keyboard actions:\n");
      fprintf ( stderr,"   Object selection:\n");
      fprintf ( stderr,"    1-6 [7-0]  :  toggle select/deselect graph object\n");
      fprintf ( stderr,"    0-7 [6-0]  :  toggle select/deselect  surface object\n\n");

      fprintf ( stderr,"   animation:\n");
      fprintf ( stderr,"   <sp>    :  freeze animation display, reset rates.\n");
      fprintf ( stderr,"     g     :  play animation sequence forward rate increase \n");
      fprintf ( stderr,"     G     :  play animation sequence reverse rate increase \n");
      fprintf ( stderr,"     t     :  tumble forward rate increase\n");
      fprintf ( stderr,"     T     :  tumble forward rate decrease\n");
      fprintf ( stderr,"     B     :  combine tumble, roll and animation \n");
      fprintf ( stderr,"           :   (may be used in conjunction with t/T and g/G)\n");
      fprintf ( stderr,"     h     :  window width increase\n\n");

      fprintf ( stderr,"   Viewing options:\n");
      fprintf ( stderr,"     l     :  toggle labels display/inhibit\n");
      fprintf ( stderr,"     i     :  toggle scope mode,\n");
      fprintf ( stderr,"              i) without out motion ii) z-motion, iii) y-motion\n");
      fprintf ( stderr,"     b     :  toggle surface display mode:\n");
      fprintf ( stderr,"              i) triangular tessellations, \n");
      fprintf ( stderr,"              ii) colour gradiant, iii) smoothed (default) \n");
      fprintf ( stderr,"     H     :  window width decrease\n");
      fprintf ( stderr,"     I     :   i) display of X-graphs toggle simultaneous \n");
      fprintf ( stderr,"              ii) display  iii) animated display\n");
      fprintf ( stderr,"     L     :  toggle static graphs intensity \n");
      fprintf ( stderr,"              controlled/disabled, by light source 2 angle \n\n");
      fprintf ( stderr,"     p     :  toggle  background colour setting to complement\n");

      fprintf ( stderr,"   Grid viewing:\n");
      fprintf ( stderr,"     !     :  illuminated scale zx plane, brightness incr.\n");
      fprintf ( stderr,"     @     :  illuminated scale zx plane, brightness dec.\n");
      fprintf ( stderr,"     #     :  illuminated scale yx plane, brightness incr.\n");
      fprintf ( stderr,"     $     :  illuminated scale yx plane, brightness dec.\n");
      fprintf ( stderr,"     %%     :  toggle major scale axis\n\n");
      fprintf ( stderr,"     P     :  toggle  grid colour setting to complement\n");

      fprintf ( stderr,"   Surface colouring options:\n");

      fprintf ( stderr,"   Coding options:\n");
      fprintf ( stderr,"     o     :  coding-markup computed/displayed\n");
      fprintf ( stderr,"    W/w    :  blue/yellow\n");
      fprintf ( stderr,"    S/s    :  yellow/blue\n");
      fprintf ( stderr,"    E/e    :  green/yellow\n");
      fprintf ( stderr,"    D/d    :  yellow/green\n");
      fprintf ( stderr,"    R/r    :  red/green\n");
      fprintf ( stderr,"    F/f    :  green/red\n");
      fprintf ( stderr,"     A     :  coarse white increase\n");
      fprintf ( stderr,"     a     :  fine while decrease white\n\n");


      fprintf ( stderr,"   Outout options:\n");
      fprintf ( stderr,"     O     :  display sleep staging \n\n");

      fprintf ( stderr,"   <esc>   :  exit display\n");
*/

	glEnable		( GL_LIGHTING ) ;

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

/* ========================================================================= */  
/* draw_io                                                                */
/* ========================================================================= */  
/*
** Draws in the wire frame for the axis and base plane
**  
** 
** 
** 
*/


void drawio()
{
	static char buffer[255];
	int	i ;
	int tx, g_x ;
	int top, bottom, left, right, ty, g_y ;
	int tinc, coloffset;
//	float text_r, text_g, text_b, sheet_r, sheet_g, sheet_b;

//	if ((pertecs_data.build.Output_channel_no > 0) || (pertecs_data.build.Input_channel_no > 0)) { // only if the inputs and outputs are in use
		g_x = glutGet	(GLUT_WINDOW_WIDTH);
		g_y = glutGet	(GLUT_WINDOW_HEIGHT);
		
		left = g_x/20;
		right = g_x - g_x/20;
		
		tinc = glutBitmapWidth(GLUT_BITMAP_8_BY_13, 'M') * 2;
		coloffset = tinc * 20; // column offset 
		top = g_y - g_y/20 - tinc; // set the top line
		bottom = g_y/20 ;

	
		/* save the current matrices */
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glDisable		( GL_LIGHTING ) ;
	
		 glLoadIdentity();
		 gluOrtho2D(0, g_x, 0, g_y); /* set projection to map pixels */


		 glColor4ub(0x80, 0x80, 0x80, 0x40); // colour for the sheet overlay
		 drawSquare(g_x/20, g_y/20, g_x - g_x/10, g_y - g_y/10);

//		text_r = background_r + .5; while  (text_r > 1.0) text_r -= 1.0;
//		text_g = background_g + 1.0; while  (text_g > 1.0) text_g -= 1.0;
//		text_b = background_b + .5; while  (text_b > 1.0) text_b -= 1.0;
		
		 glColor4f(.5, 1.0, .5, 0.8); // text
	
		// first column
		tx = g_x/10 - xtextstart ;
		ty = top - ytextstart ;
		
		if (pertecs_data.build.Input_channel_no > 0) {
			 drawText2D("STD INPUT MODULES", tx,ty, top, bottom, left, right); 
			 sprintf(buffer, "%s %s: %ld", pertecs_data.IO_Modules[0]->name, pertecs_data.IO_Modules[0]->put.In->label, pertecs_data.IO_Modules[0]->put.In->intvalue);
			 drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);
			 for (i=1; i <= pertecs_data.build.Input_channel_no; i++) {
				sprintf(buffer, "%s %s: %8.5e", pertecs_data.IO_Modules[i]->name, pertecs_data.IO_Modules[i]->put.In->label, pertecs_data.mirror_IO_Modules[i]);
				drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);
			 }
			 
			// ready for second column
			tx += coloffset;
			ty = top - ytextstart ;
		}
		if (pertecs_data.build.Output_channel_no > 0) {
			 drawText2D("STD OUTPUT MODULES", tx,ty, top, bottom, left, right); 
			 sprintf(buffer, "%s %s: %ld", pertecs_data.Output_Modules[0]->name, pertecs_data.Output_Modules[0]->put.Out->label, pertecs_data.Output_Modules[0]->put.Out->intvalue);
			 drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);
			 for (i=1; i <= pertecs_data.build.Output_channel_no; i++) {
				sprintf(buffer, "%s %s: %8.5e", pertecs_data.Output_Modules[i]->name, pertecs_data.Output_Modules[i]->put.Out->label, pertecs_data.mirror_Output_Modules[i]/pertecs_data.out_resample);
				drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);
			 }
			tx += coloffset;
			ty = top - ytextstart ;
		}
		if (pertecs_data.build.Connection_build_size > 0) {
			 drawText2D("TERMINAL LUGS", tx,ty, top, bottom, left, right); 
			 for (i=0; i < pertecs_data.build.Connection_build_size; i++) {
				sprintf(buffer, "%s %s: %8.5e", pertecs_data.lug_Modules[i]->name, pertecs_data.lug_Modules[i]->put.Out->label, pertecs_data.mirror_Lug_Modules[i]);
				drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);
			 }
			tx += coloffset;
			ty = top - ytextstart ;
		}
		if 	(pertecs_data.build.Trace_build_size > 0)  { // do trace display
			 drawText2D("SCOPE TRACE MODULES", tx,ty, top, bottom, left, right); 
			for (i=0; i < pertecs_data.build.build_size; i++) { 
				if (pertecs_data.Modules[i]->type == TRACE) {
					sprintf(buffer, "%s: %s", pertecs_data.Modules[i]->name, pertecs_data.Modules[i]->ParamPtr[TRACECOLOUR]->label);
					drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "Index: %d / %.4f", pertecs_data.Modules[i]->In_Ptr, (pertecs_data.Modules[i]->input_buffPtr[pertecs_data.Modules[i]->In_Ptr] + 1)/2  );
					drawText2D(buffer, tx+18*tinc, ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[STANDARD]->label, pertecs_data.Modules[i]->Input[STANDARD]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[SPECIAL]->label,  pertecs_data.Modules[i]->Input[SPECIAL]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[SPECIAL2]->label, pertecs_data.Modules[i]->Input[SPECIAL2]->value );
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);
					
				}
			}
			tx += 3* coloffset;
			ty = top - ytextstart ;
		}
		if 	(pertecs_data.build.ID_build_size > 0)  { // do integrator differentiator display
			 drawText2D("INTEGRATOR/DIFFERENTIATOR MODULES", tx,ty, top, bottom, left, right); 
			for (i=0; i < pertecs_data.build.build_size; i++) { 
				if (pertecs_data.Modules[i]->type == DIFFINT) {
					sprintf(buffer, "%s: ", pertecs_data.Modules[i]->name);
					drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "Index: %d", pertecs_data.Modules[i]->In_Ptr  );
					drawText2D(buffer, tx+18*tinc, ty , top, bottom, left, right);

					// inputs
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[STANDARD]->label, pertecs_data.Modules[i]->Input[STANDARD]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[SPECIAL]->label,  pertecs_data.Modules[i]->Input[SPECIAL]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %g",  pertecs_data.Modules[i]->Input[SETHOLD]->label, pertecs_data.Modules[i]->Input[SETHOLD]->value );
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);

					// outputs
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[INTEGRATED]->label, pertecs_data.Modules[i]->Output[INTEGRATED]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[SMOOTHED]->label,  pertecs_data.Modules[i]->Output[SMOOTHED]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[FST_DERIV]->label, pertecs_data.Modules[i]->Output[FST_DERIV]->value );
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);

					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[SCND_DERIV]->label, pertecs_data.Modules[i]->Output[SCND_DERIV]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[THRD_DERIV]->label,  pertecs_data.Modules[i]->Output[THRD_DERIV]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[DELAYED]->label, pertecs_data.Modules[i]->Output[DELAYED]->value );
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);

				}
			}
			tx += 3* coloffset;
			ty = top - ytextstart ;
		}
		if 	(pertecs_data.build.Comp_build_size > 0)  { // do comparators display
			 drawText2D("COMPARATOR MODULES", tx,ty, top, bottom, left, right); 
			for (i=0; i < pertecs_data.build.build_size; i++) { 
				if (pertecs_data.Modules[i]->type == COMPAR) {
					sprintf(buffer, "%s: ", pertecs_data.Modules[i]->name);
					drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);

					// input
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[STANDARD]->label, pertecs_data.Modules[i]->Input[STANDARD]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);

					// outputs
					sprintf(buffer, "%s: %g",  pertecs_data.Modules[i]->Output[POS]->label, pertecs_data.Modules[i]->Output[POS]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %g",  pertecs_data.Modules[i]->Output[ZERO]->label,  pertecs_data.Modules[i]->Output[ZERO]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %g",  pertecs_data.Modules[i]->Output[NONNEG]->label, pertecs_data.Modules[i]->Output[NONNEG]->value );
					drawText2D(buffer, tx + 32*tinc , ty , top, bottom, left, right);

					sprintf(buffer, "%s: %g",  pertecs_data.Modules[i]->Output[NEG]->label, pertecs_data.Modules[i]->Output[NEG]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %g",  pertecs_data.Modules[i]->Output[NONPOS]->label,  pertecs_data.Modules[i]->Output[NONPOS]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[DELAYED]->label, pertecs_data.Modules[i]->Output[DELAYED]->value );
					drawText2D(buffer, tx + 32*tinc , ty , top, bottom, left, right);
				}
			}
			tx += 3* coloffset;
			ty = top - ytextstart ;
		}
		if 	(pertecs_data.build.Math_build_size > 0)  { // do math display
			 drawText2D("MATH CALCULATION MODULES", tx,ty, top, bottom, left, right); 
			for (i=0; i < pertecs_data.build.build_size; i++) { 
				if (pertecs_data.Modules[i]->type == MATHMOD) {
					sprintf(buffer, "%s: ", pertecs_data.Modules[i]->name );
					drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);

					// input
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[STANDARD]->label, pertecs_data.Modules[i]->Input[STANDARD]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);

					// outputs
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[SQRT]->label, pertecs_data.Modules[i]->Output[SQRT]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[LOG]->label,  pertecs_data.Modules[i]->Output[LOG]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[EXP]->label, pertecs_data.Modules[i]->Output[EXP]->value ); // need to limit display
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);

					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[SIN]->label, pertecs_data.Modules[i]->Output[SIN]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[RECIPROCAL]->label,  pertecs_data.Modules[i]->Output[RECIPROCAL]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[DELAYED]->label, pertecs_data.Modules[i]->Output[DELAYED]->value );
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);
				}
			}
			tx += 3* coloffset;
			ty = top - ytextstart ;
		}
		if 	(pertecs_data.build.Plr_build_size > 0)  { // do polar display
			 drawText2D("POLAR CALCULATION MODULES", tx,ty, top, bottom, left, right); 
			for (i=0; i < pertecs_data.build.build_size; i++) { 
				if (pertecs_data.Modules[i]->type == POLAR) {
					sprintf(buffer, "%s: ", pertecs_data.Modules[i]->name );
					drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);

					// inputs
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[STANDARD]->label, pertecs_data.Modules[i]->Input[STANDARD]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[SPECIAL]->label,  pertecs_data.Modules[i]->Input[SPECIAL]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[SPECIAL2]->label,  pertecs_data.Modules[i]->Input[SPECIAL2]->value );
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);

					// outputs
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[NORM]->label, pertecs_data.Modules[i]->Output[NORM]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[THETA]->label,  pertecs_data.Modules[i]->Output[THETA]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[PHI]->label,  pertecs_data.Modules[i]->Output[PHI]->value );
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);

					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[DELAYED]->label, pertecs_data.Modules[i]->Output[DELAYED]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[DELAYED2]->label,  pertecs_data.Modules[i]->Output[DELAYED2]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[DELAYED3]->label,  pertecs_data.Modules[i]->Output[DELAYED3]->value );
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);
				}
			}
			tx += 3* coloffset;
			ty = top - ytextstart ;
		}
		if 	(pertecs_data.build.Filter_build_size > 0)  { // do trace display
			 drawText2D("FILTER MODULES", tx,ty, top, bottom, left, right); 
			for (i=0; i < pertecs_data.build.build_size; i++) { 
				if (pertecs_data.Modules[i]->type == FILTER) {
					sprintf(buffer, "%s: ", pertecs_data.Modules[i]->name );
					drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);

					// input
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[STANDARD]->label, pertecs_data.Modules[i]->Input[STANDARD]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);


					// outputs
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[FILTEROUT]->label, pertecs_data.Modules[i]->Output[FILTEROUT]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[DELAYED]->label,  pertecs_data.Modules[i]->Output[DELAYED]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
				}
			}
			tx += 2 * coloffset;
			ty = top - ytextstart ;
		}
		if 	(pertecs_data.build.Noise_channel_size > 0)  { // do trace display
			 drawText2D("NOISE GENERATOR MODULES", tx,ty, top, bottom, left, right); 
			for (i=0; i < pertecs_data.build.build_size; i++) { 
				if (pertecs_data.Modules[i]->type == NOISE) {
					sprintf(buffer, "%s: ", pertecs_data.Modules[i]->name );
					drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);

					// input
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[STANDARD]->label, pertecs_data.Modules[i]->Input[STANDARD]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);

					// outputs
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[NOISEOUT]->label, pertecs_data.Modules[i]->Output[NOISEOUT]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[DELAYED]->label,  pertecs_data.Modules[i]->Output[DELAYED]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
				}
			}
			tx += 2 * coloffset ;
			ty = top - ytextstart ;
		}
		if 	(pertecs_data.build.Ent_build_size > 0)  { // do entropy display
			 drawText2D("T-ENTROPY MODULES", tx,ty, top, bottom, left, right); 
			for (i=0; i < pertecs_data.build.build_size; i++) { 
				if (pertecs_data.Modules[i]->type == QTC) {
					sprintf(buffer, "%s: ", pertecs_data.Modules[i]->name );
					drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);

					// input
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[STANDARD]->label, pertecs_data.Modules[i]->Input[STANDARD]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);

					// outputs
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[RAST_ENTROPY]->label, pertecs_data.Modules[i]->Output[RAST_ENTROPY]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[DELAYED]->label,  pertecs_data.Modules[i]->Output[DELAYED]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
				}
			}
			tx += 2 * coloffset;
			ty = top - ytextstart ;
		}
		if 	(pertecs_data.build.Delay_build_size > 0)  { // do delay line display
			 drawText2D("DELAY LINE MODULES", tx,ty, top, bottom, left, right); 
			for (i=0; i < pertecs_data.build.build_size; i++) { 
				if (pertecs_data.Modules[i]->type == DELINE) {
					sprintf(buffer, "%s: ", pertecs_data.Modules[i]->name );
					drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);

					// input
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[STANDARD]->label, pertecs_data.Modules[i]->Input[STANDARD]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);

					// outputs
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[DELAYED]->label,  pertecs_data.Modules[i]->Output[DELAYED]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
				}
			}
			tx +=  2 * coloffset;
			ty = top - ytextstart ;
		}
		if 	(pertecs_data.build.Extern_build_size > 0)  { // do external display
			 drawText2D("EXTERNAL MODULES", tx,ty, top, bottom, left, right); 
			for (i=0; i < pertecs_data.build.build_size; i++) { 
				if (pertecs_data.Modules[i]->type == EXTERN) {
					sprintf(buffer, "%s: ", pertecs_data.Modules[i]->name );
					drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);

					// inputs
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[STANDARD]->label, pertecs_data.Modules[i]->Input[STANDARD]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[SPECIAL]->label,  pertecs_data.Modules[i]->Input[SPECIAL]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[SETHOLD]->label, pertecs_data.Modules[i]->Input[SETHOLD]->value );
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);

					// outputs
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[ARG1]->label, pertecs_data.Modules[i]->Output[ARG1]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[ARG2]->label,  pertecs_data.Modules[i]->Output[ARG2]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[ARG3]->label, pertecs_data.Modules[i]->Output[ARG3]->value );
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);

					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[ARG4]->label, pertecs_data.Modules[i]->Output[ARG4]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[ARG5]->label,  pertecs_data.Modules[i]->Output[ARG5]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[ARG6]->label, pertecs_data.Modules[i]->Output[ARG6]->value );
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);
				}
			}
			tx += 3 * coloffset;
			ty = top - ytextstart ;
		}
		if 	(pertecs_data.build.Pot_channel_no > 0)  { // do preset display
			 drawText2D("POTENTIOMETER MODULES", tx,ty, top, bottom, left, right); 
			for (i=0; i < pertecs_data.build.build_size; i++) { 
				if (pertecs_data.Modules[i]->type == PRESETS) {
					sprintf(buffer, "%s: ", pertecs_data.Modules[i]->name );
					drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);

					// Potentiometer Outputs
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[POTINX]->label, pertecs_data.Modules[i]->Output[POTINX]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[POTINY]->label,  pertecs_data.Modules[i]->Output[POTINY]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Output[POTINZ]->label, pertecs_data.Modules[i]->Output[POTINZ]->value );
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);


					// Potentiometer selects
					sprintf(buffer, "%s: %g",  pertecs_data.Modules[i]->Output[SELINX]->label, pertecs_data.Modules[i]->Output[SELINX]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %g",  pertecs_data.Modules[i]->Output[SELINY]->label,  pertecs_data.Modules[i]->Output[SELINY]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
					sprintf(buffer, "%s: %g",  pertecs_data.Modules[i]->Output[SELINZ]->label, pertecs_data.Modules[i]->Output[SELINZ]->value );
					drawText2D(buffer, tx + 34*tinc , ty , top, bottom, left, right);

				}
			}
			tx += 3 * coloffset;
			ty = top - ytextstart ;
		}
		if 	(pertecs_data.build.FFT_build_size > 0)  { // do fft display
			 drawText2D("FFT MODULES", tx,ty, top, bottom, left, right); 
			for (i=0; i < pertecs_data.build.build_size; i++) { 
				if (pertecs_data.Modules[i]->type == FFT) {
					sprintf(buffer, "%s: ", pertecs_data.Modules[i]->name );
					drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);


					// inputs and outputs to be sorted
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[STANDARD]->label, pertecs_data.Modules[i]->Input[STANDARD]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[SPECIAL]->label,  pertecs_data.Modules[i]->Input[SPECIAL]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
				}
			}
			tx += 2 * coloffset;
			ty = top - ytextstart ;
		}
/*	should look like i/o device rather than a processing module
		if 	(pertecs_data.build.Connection_build_size > 0)  { // do lug terminals
			 drawText2D("TERMINAL LUGS", tx,ty, top, bottom, left, right); 
			for (i=0; i < pertecs_data.build.build_size; i++) { 
				if (pertecs_data.Modules[i]->type == LUG) {
					sprintf(buffer, "%s: ", pertecs_data.Modules[i]->name );
					drawText2D(buffer, tx, ty -= tinc, top, bottom, left, right);


					// inputs
					sprintf(buffer, "%s: %8.5e",  pertecs_data.Modules[i]->Input[STANDARD]->label, pertecs_data.Modules[i]->Input[STANDARD]->value );
					drawText2D(buffer, tx + 2*tinc, ty -= tinc, top, bottom, left, right);
					sprintf(buffer, "%s: %.6f",  pertecs_data.Modules[i]->Input[SPECIAL]->label,  pertecs_data.Modules[i]->Input[SPECIAL]->value );
					drawText2D(buffer, tx + 18*tinc , ty , top, bottom, left, right);
				}
			}
			tx += 2* coloffset;
			ty = top - ytextstart ;
		}
*/

		glEnable		( GL_LIGHTING ) ;
	
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	
}


/* ========================================================================= */  
/* draw_axis-graticule                                                               */
/* ========================================================================= */  
/*
** Draws in the wire frame for the axis and base plane
**  
** 
** 
** 
*/

void draw_axis ( float zaxis, float yaxis, float xaxis, float grid_r, float grid_g, float grid_b, int offsetFlg, int axisFlg)
{
	int i,k;
 	float x_red,x_green,x_blue,y_red,y_green,y_blue,z_red,z_green,z_blue, m_red,m_green,m_blue;
	float luminance = sqrt (background_r*background_r + background_g*background_g + background_b*background_b);
	float mincolour;
	
	float xgridpoints[100];
	float ygridpoints[100];
	float zgridpoints[100];
	

    if (grid_r < grid_g) mincolour = grid_r; else mincolour = grid_g;
    if (mincolour > grid_b) mincolour = grid_b;
    luminance -= mincolour;
	
 	x_red = xaxis * grid_r;
 	x_green = xaxis * grid_g;
 	x_blue = xaxis * grid_b;
 	y_red = yaxis * grid_r;
 	y_green = yaxis * grid_g;
 	y_blue = yaxis * grid_b;
 	z_red = zaxis * grid_r;
 	z_green = zaxis * grid_g;
 	z_blue = zaxis * grid_b;
 	
 	if (complement) {  
 			x_red =  complement - x_red;  y_red =  complement - y_red;  z_red =  complement - z_red; 
 			x_green =  complement - x_green;  y_green =  complement - y_green;  z_green =  complement - z_green; 
 			x_blue =  complement - x_blue;  y_blue =  complement - y_blue;  z_blue =  complement - z_blue; 
 	}
    
 	x_red += luminance;
 	x_green += luminance;
 	x_blue += luminance;
 	y_red += luminance;
 	y_green += luminance;
 	y_blue += luminance;
 	z_red += luminance;
 	z_green += luminance;
 	z_blue += luminance;
 	
 	float cxlimit;
 	
 	
 	
 	
 	if (!offsetFlg) cxlimit = xlimit;												// cxlimit is either at 0 or left limit .... if offset axis required
 	else {
 		cxlimit = 0;
 	}

	glLineWidth   ( 0.50 ) ;
//xaxis = yz plane + yaxis = xz plane   -> z axis


// ORIGIN CROSS HAIRS ????
/*	if (( (xaxis > 0.05) || (yaxis > 0.05)) && (offsetFlg))  { 			// z axis 
 		if ( y_red > (m_red = x_red)) m_red = y_red ;
 		if ( y_green > (m_green = x_green)) m_green = y_green ;
 		if ( y_blue > (m_blue = x_blue)) m_blue = y_blue ;
		glColor3f	( m_red ,  m_green , m_blue) ;
 		glBegin ( GL_LINES ) ; { // x-axis
		  glVertex3f (xorig , yorig, - zlimit ) ;
		  glVertex3f (xorig , yorig,  zlimit ) ;
		}  glEnd ( ) ;

	}
	else if ((yaxis > 0.05) && (!offsetFlg)) {
		glColor3f	( y_red ,  y_green , y_blue) ;
 		glBegin ( GL_LINES ) ; { // x-axis
		  glVertex3f (xorig , yorig, - zlimit ) ;
		  glVertex3f (xorig , yorig,  zlimit ) ;
		}  glEnd ( ) ;	
	}
	
	
	
	
	
	if (( (zaxis > 0.05) || (xaxis > 0.05))  && (offsetFlg))  { 		// y axis
 		if ( z_red > (m_red = x_red)) m_red = z_red ;
 		if ( z_green > (m_green = x_green)) m_green = z_green ;
 		if ( z_blue > (m_blue = x_blue)) m_blue = z_blue ;
		glColor3f	( m_red ,  m_green , m_blue) ;
 		glBegin ( GL_LINES ) ; { // x-axis
		  glVertex3f (xorig ,  - ylimit, zorig ) ;
		  glVertex3f (xorig ,   ylimit, zorig ) ;
		}  glEnd ( ) ;
	
	}
	else if ((zaxis > 0.05) && (!offsetFlg)) {
		glColor3f	( z_red ,  z_green , z_blue) ;
 		glBegin ( GL_LINES ) ; { // x-axis
		  glVertex3f (xorig ,  - ylimit, zorig ) ;
		  glVertex3f (xorig ,  ylimit, zorig ) ;
		}  glEnd ( ) ;
	}



	if ((yaxis > 0.05) || (zaxis > 0.05) ) { 							// x axis
 		if ( y_red > (m_red = z_red)) m_red = y_red ;
 		if ( y_green > (m_green = z_green)) m_green = y_green ;
 		if ( y_blue > (m_blue = z_blue)) m_blue = y_blue ;
		glColor3f	( m_red ,  m_green , m_blue) ;
		glBegin ( GL_LINES ) ; { // x-axis
		  glVertex3f (xorig - xlimit, yorig, zorig ) ;
		  glVertex3f (xorig + xlimit, yorig, zorig ) ;
		}  glEnd ( ) ;
	
	}	

	if (( xaxis > 0.05) && (!offsetFlg)) {
		glColor3f	( x_red ,  x_green , x_blue) ;
 		glBegin ( GL_LINES ) ; { // x-axis
		  glVertex3f (xorig - xlimit , - ylimit, zorig ) ;
		  glVertex3f (xorig - xlimit ,  ylimit, zorig ) ;
		}  glEnd ( ) ;
 		glBegin ( GL_LINES ) ; { // x-axis
		  glVertex3f (xorig - xlimit, yorig, - zlimit ) ;
		  glVertex3f (xorig - xlimit, yorig,  zlimit ) ;
		}  glEnd ( ) ;
	}

*/


// The yz plane
int xoffset = (abs((int) xorig)) % 10;
int yoffset = (abs((int) yorig)) % 10;
int zoffset = (abs((int) zorig)) % 10;
float scaleit;

		if ( xlinlog <=0 ) {
			switch ((int) xlinlog ) {
				case -1: // linear unity step
					xoffset = ((int) xorig) % ((int) xlimit) ;
					for (i = 0; i <= 2 * ((int) xlimit) ; i++) xgridpoints[i] = i - (int) xlimit ;
					xgridpoints[ 2 * (int) xlimit + 1] = xlimit + 1;		
					break;
				case 0: // linear .5 step
					xoffset = ((int) xorig) % ((int) xlimit);
					for (i = 0; i <= 4 * ((int) xlimit) ; i++)  xgridpoints[i] = i * 0.5 - (int) xlimit ;
					xgridpoints[ 4 * (int) xlimit + 1] = xlimit + 1;		
					break;
				default: 
					break;
			}
		}
		else { // log scales 
			if (xlinlog == ((int) xlinlog)) { scaleit = xlimit*2.0/((int) xlinlog); }
			else scaleit = xlimit*2.0/(((int) xlinlog) + log10f(10*(xlinlog - (int)xlinlog)));						

			for (k = 0; k < (int) xlinlog + 1; k++) {
				for (i = 0; i < 10; i++)  {
					xgridpoints[i+k*9] = log10f(i + 1) * scaleit -  xlimit +  (scaleit * k);
				}
			}				
		}
		 
		if ( ylinlog <=0 ) {
			switch ((int) ylinlog ) {
				case -1: // linear .5 step
					yoffset = ((int) yorig) % ((int) ylimit) ;
					for (i = 0; i <= 2 * ((int) ylimit) ; i++) ygridpoints[i] = i - (int) ylimit ;
					ygridpoints[ 2 * (int) ylimit + 1] = ylimit + 1;		
	
					break;
				case 0: // linear unity step
					yoffset = ((int) yorig) % ((int) ylimit);
					for (i = 0; i <= 4 * ((int) ylimit) ; i++)  ygridpoints[i] = i * 0.5 - (int) ylimit ;
					ygridpoints[ 4 * (int) ylimit + 1] = ylimit + 1;		
					break;
				default: 
					break;
			}
		}
		else { // log scales 
			if (ylinlog == (int)ylinlog) scaleit = ylimit*2.0/((int) ylinlog); 
			else scaleit = ylimit*2.0/(((int) ylinlog) + log10f(10*(ylinlog - (int)ylinlog)));						

			for (k = 0; k < (int) (ylinlog + 1); k++) {
				for (i = 0; i < 10; i++)  {
					ygridpoints[i+k*9] = log10f(i + 1) * scaleit -  ylimit +  (scaleit * k);
				}
			}				
		}
		
		if ( ylinlog <=0 ) {
			switch ((int) zlinlog ) {
				case -1: // linear .5 step
					zoffset = ((int) zorig) % ((int) zlimit) ;
					for (i = 0; i <= 2 * ((int) zlimit) ; i++) zgridpoints[i] = i - (int) zlimit ;
					zgridpoints[ 2 * (int) zlimit + 1] = zlimit + 1;		
	
					break;
				case 0: // linear unity step
					zoffset = ((int) zorig) % ((int) zlimit) ;
					for (i = 0; i <= 4 * ((int) zlimit) ; i++)  zgridpoints[i] = i * 0.5 - (int) zlimit ;
					zgridpoints[ 4 * (int) zlimit + 1] = zlimit + 1;		
					break;
				default: 
					break;
			}
		}
		else { // log scales 
			if (zlinlog == (int)zlinlog) scaleit = zlimit*2.0/((int) zlinlog); 
			else scaleit = zlimit*2.0/(((int) zlinlog) + log10f(10*(zlinlog - (int)zlinlog)));						

			for (k = 0; k < (int) (zlinlog + 1); k++) {
				for (i = 0; i < 10; i++)  {
					zgridpoints[i+k*9] = log10f(i + 1) * scaleit -  zlimit +  (scaleit * k);
				}
			}				
		}


float gridlinepos; 
// The yz plane
	if (xaxis>.05)  {
		glColor3f  ( x_red ,  x_green, x_blue ) ;
		i = 0;
		while ( ygridpoints[i] <= ylimit) {
			if ( yoffset && (ygridpoints[i] == -ylimit)) gridlinepos = ylimit;
			else {
				gridlinepos = ygridpoints[i] + (ygridpoints[ yoffset ] + ylimit);
				if (gridlinepos > ylimit) gridlinepos -= 2 * ylimit;
				else if (gridlinepos  < -ylimit) gridlinepos += 2 * xlimit;
			}
			glBegin ( GL_LINES ) ; { 
					glVertex3f ( xorig - cxlimit, gridlinepos , zlimit ) ;						// y increment   z direction
					glVertex3f ( xorig - cxlimit, gridlinepos , -zlimit ) ;
			}  glEnd ( ) ;
			i++;
		}

		i = 0;
		while ( zgridpoints[i] <= zlimit) {
			if ( zoffset && (zgridpoints[i] == -zlimit)) gridlinepos = zlimit;
			else {
				gridlinepos = zgridpoints[i] + (zgridpoints[ zoffset ] + zlimit);
				if (gridlinepos > zlimit) gridlinepos -= 2 * zlimit;
				else if (gridlinepos  < - zlimit) gridlinepos += 2 * zlimit;
			}
			glBegin ( GL_LINES ) ; { 
					glVertex3f (xorig - cxlimit, ylimit, gridlinepos ) ;							// z increment   y direction
					glVertex3f (xorig - cxlimit, -ylimit, gridlinepos ) ;
			}  glEnd ( ) ;
			i++;

		}
	}

// The xz plane
	if  (yaxis>.05)  {
		glColor3f  ( y_red ,  y_green, y_blue ) ;
		i = 0;
		while ( xgridpoints[i] <= xlimit) {
			if ( xoffset && ( xgridpoints[i] == -xlimit)) gridlinepos = xlimit;
			else {
				gridlinepos = xgridpoints[i] - ( xgridpoints[ xoffset ] + xlimit);
				if (gridlinepos > xlimit) gridlinepos -= 2 * xlimit;
				else if (gridlinepos  < - xlimit) gridlinepos += 2 * xlimit;
			}
			glBegin ( GL_LINES ) ; { 
					glVertex3f ( gridlinepos, yorig ,  zlimit) ;						// x increment   z direction
					glVertex3f ( gridlinepos, yorig ,  -zlimit) ;
			}  glEnd ( ) ;
			i++;
		}



		i = 0;
		while ( zgridpoints[i] <= zlimit) { // in the yz plane
			if ( zoffset && ( zgridpoints[i] == -zlimit)) gridlinepos = zlimit;
			else {
				gridlinepos = zgridpoints[i] + (zgridpoints[ zoffset ] + zlimit);
				if (gridlinepos > zlimit) gridlinepos -= 2 * zlimit;
				else if (gridlinepos  < - zlimit) gridlinepos += 2 * zlimit;
			}
			glBegin ( GL_LINES ) ; { 
					glVertex3f ( xlimit , yorig, gridlinepos ) ;							// z increment   x direction
					glVertex3f ( -xlimit , yorig, gridlinepos ) ;
			}  glEnd ( ) ;
			i++;
		}
	}

// The xy plane
	if (zaxis>.05) {
		glColor3f  ( z_red ,  z_green, z_blue ) ;
		i = 0;
		while ( xgridpoints[i] <= xlimit) {
			if ( xoffset && ( xgridpoints[i] == -xlimit)) gridlinepos = xlimit;
			else {
				gridlinepos = xgridpoints[i] - ( xgridpoints[ xoffset ] + xlimit);
				if (gridlinepos > xlimit) gridlinepos -= 2 * xlimit;
				else if (gridlinepos  < - xlimit) gridlinepos += 2 * xlimit;
			}
			glBegin ( GL_LINES ) ; { 
					glVertex3f ( gridlinepos, ylimit ,  zorig) ;						// x increment   y direction
					glVertex3f ( gridlinepos, -ylimit ,  zorig) ;
			}  glEnd ( ) ;
			i++;
		}
		
		i = 0;
		while ( ygridpoints[i] <= ylimit) {
			if ( yoffset && (ygridpoints[i] == -ylimit)) gridlinepos = ylimit;
			else {
				gridlinepos = ygridpoints[i] + (ygridpoints[ yoffset ] + ylimit);
				if (gridlinepos > ylimit) gridlinepos -= 2 * ylimit;
				else if (gridlinepos  < - ylimit) gridlinepos += 2 * xlimit;
			}
			glBegin ( GL_LINES ) ; { 
					glVertex3f ( xlimit , gridlinepos , zorig) ;							// y increment   x direction
					glVertex3f ( -xlimit , gridlinepos , zorig ) ;
			}  glEnd ( ) ;
			i++;
		}
	}


	glLineWidth   ( 1.50 ) ;
 
 
 // drawing major axes

	if (axisFlg) { 
	 		if ( y_red > (m_red = x_red)) m_red = y_red ;
	 		if ( m_red < z_red) m_red = z_red ;
	 		
 			if ( y_green > (m_green = x_green)) m_green = y_green ;
	 		if ( m_green < z_green) m_green = z_green ;

 			if ( y_blue > (m_blue = x_blue)) m_blue = y_blue ;
	 		if ( m_green < z_blue) m_green = z_blue ;
			glColor3f  ( m_red ,  m_green, m_blue ) ;


			if ((xaxis>.05) && (yaxis>.05)) {
				glBegin ( GL_LINES ) ; // z-axis
				  glVertex3f (xorig - cxlimit, yorig, - zlimit ) ;
				  glVertex3f (xorig - cxlimit, yorig,  zlimit ) ;
				glEnd ( ) ;	
			}
			if ((yaxis>.05) && (zaxis>.05)) {
				glBegin ( GL_LINES ) ;  // x-axis
				  glVertex3f ( - xlimit, yorig, zorig ) ;
				  glVertex3f ( xlimit, yorig, zorig ) ;
				glEnd ( ) ;
			}
			if ((zaxis>.05) && (xaxis>.05)) {
				glBegin ( GL_LINES ) ;  // x-axis
				  glVertex3f (xorig - cxlimit, - ylimit, zorig ) ;
				  glVertex3f (xorig - cxlimit,  ylimit, zorig ) ;
			    glEnd ( ) ;
			}
			
		glDisable		( GL_LIGHTING ) ;
			if ((xaxis>.05) && (yaxis>.05)) drawText3D(xorig - cxlimit ,    yorig,    zlimit+.2, "z" ) ;
			if ((yaxis>.05) && (zaxis>.05)) drawText3D(xlimit+.2 ,   yorig,    zorig, "x" ) ;
			if ((zaxis>.05) && (xaxis>.05)) drawText3D( xorig - cxlimit ,    ylimit+.2,    zorig , "y" ) ;
		glEnable		( GL_LIGHTING ) ;
 
 	}


  glLineWidth   ( 1.00 ) ;  
 	  if (helpmenu == TRUE) drawMenu();
 	  else if (textdisp == TRUE ) drawio(); 
 	  
 	  
 	  

}
/* ========================================================================= */  
/* screentext                         		                                 */
/* ========================================================================= */  
/*
*/
void screentext (LINE **curvePtr, int Num_curve, TXTLINE **textPtr, int Num_texts)
{
  int plabel, p;
  char  scanpos[LABEL_SIZE], graphlabels[LABEL_SIZE], indextime[LABEL_SIZE];
  float left, middle, right, window, timestuff;
  int	hh, h, mm, m, ss, s, ps, pss;
  
    int pdivcpy = pdiv;
	if (sleepStateFlag == 1) { Num_curve++; pdivcpy++; } // raise count for the duration of the display call

	glDisable		( GL_LIGHTING ) ;
	glColor3f	( 0.5 ,  0.5 , 0.5) ;
	if (Showlabels) { // display the scan start/middle/finish   and optional time

		left = (1+ start_scan);
		middle = left + width_scan/2;
		right = left + width_scan;
		window = duration *  width_scan/2;
		if (left < 0) left = 0;
		if (middle < 0) middle = 0;
		if (right < 0) right = 0;


		if (duration > 0) { // I expect there is a c-library conversion available for this...
			timestuff = duration *  middle / 2.0 ;
			ps = (int) (timestuff * 100) % 100 ;
			pss = ps % 10 ;
			ps = ps/10;
			s = ((int) timestuff) % 60;
			ss = s % 10;
			s = s/10 ;
			m = ((int) timestuff/60) % 60;
			mm = m % 10;
			m = m/10 ;
			h = ((int) timestuff/3600) % 24;
			hh = h % 10;
			h = h/10;

			if (window < 1) 
				snprintf(&scanpos[0],LABEL_SIZE, "[  %0.4f  |  %0.3f  |  %d%d:%d%d:%d%d.%d%d  ]" , timestuff, window, h, hh , m, mm, s, ss, ps, pss); //removed \000
			else if (window < 10) 
				snprintf(&scanpos[0],LABEL_SIZE, "[  %0.3f  |  %0.2f  |  %d%d:%d%d:%d%d.%d%d  ]" , timestuff, window, h, hh , m, mm, s, ss, ps, pss); //removed \000
			else 
				snprintf(&scanpos[0],LABEL_SIZE, "[  %0.2f  |  %0.1f  |  %d%d:%d%d:%d%d.%d%d  ]" , timestuff, window, h, hh , m, mm, s, ss, ps, pss); //removed \000
			drawText3D(-2, -5.6 , 0.4, scanpos) ;
		}
		else {
			snprintf(&scanpos[0],LABEL_SIZE, "%f  |  %f  |  %f",left/2.0, middle/2.0, right/2.0); drawText3D(-1, -5.6 , 0.4, scanpos) ; //removed \000
		}
		

		if (simultrace == 1) { // graph label now allows for LABEL_SIZE chars
			if (Num_curve-pdivcpy ) {
				plabel = pdivcpy + (int) ( (float) (Num_curve-pdivcpy) * (1+ start_scan + width_scan)/2);
				if (plabel > Num_curve-1) plabel = Num_curve-1;
				if ( plabel > pdivcpy) {snprintf(&graphlabels[0],LABEL_SIZE, "%s", curvePtr[plabel]->label );  drawText3D(-6.5, -5.0 , -.4, graphlabels);}
				// printf("NumCurve %d, pdiv  %d, plabel  %d\n", Num_curve, pdivcpy, plabel );
			}
		}
		for (p=0; ((p < Num_curve) && (p <= 6) && (simultrace == 0)) ; p++) {
			if ((curvePtr[p])->visible == GLUT_VISIBLE) {
				drawText3D(-6.0 + 2*p , -5.0   , .2, (curvePtr[p])->label) ; 
			}
		}
	}
	if (Num_texts) {
		for (p=0; p < Num_texts ; p++) {
			if ((textPtr[p])->visible == GLUT_VISIBLE) {
				drawText3D(textPtr[p]->xoffset, textPtr[p]->yoffset  , textPtr[p]->zoffset, (textPtr[p])->label) ; 
			}
		}
	}
	glEnable		( GL_LIGHTING ) ;

	
}

extern long oframe_counter;
#define TAB		12
#define MAXTEXT		LABEL_SIZE		// used to be 60 but smaller than LABEL_SIZE

/* ========================================================================= */  
/* paint_curves                                                      		 */
/* ========================================================================= */  
/*
** Paints traces and graphs. 
*/

void paint_curves ( LINE **curvePtr, int Num_curve )
{
  int p, q, s, curv, pos, pstart, pinc, pdone, plabel, reversescandir, tab;
  float tx, ty, tz,  tinc;
  long    i, j, k, ii,	nxtii, ring_ii;
  float intensity, ydepth, zdepth;
  float	ring_scan, shift, coeff;
	char stringt[MAXTEXT], stringx[MAXTEXT], stringy[MAXTEXT], stringz[MAXTEXT];
	float deltaoffset = .2;


	int pdivcpy = pdiv; // pass in pdiv as an argument in future
	if (sleepStateFlag == 1) { Num_curve++; pdivcpy++; }// raise count for the duration of the display call
		
	glEnable		( GL_LIGHTING ) ;
	
	tinc = .34;

	pstart = 0; pinc = 1; pdone = Num_curve;  
	for (p=pstart; p != pdone; p += pinc) { 
//		printf ("curve = %d, visible = %d, kind %d, pdiv %d \n",p, curvePtr[p]->visible, curvePtr[p]->kind, pdiv); 


		if (curvePtr[p]->kind == COLOUR_FUNC) { // treat the colour functions as invisible and static
			curvePtr[p]->visible = GLUT_NOT_VISIBLE;
			pos = (int) ((float) curvePtr[p]->npolylinenodes * (1.0 + start_scan + width_scan/2.0)/2.0);
			pos = 3*pos;

			curvePtr[p]->colour_red = curvePtr[p]->polyline[pos];
			curvePtr[p]->colour_green = curvePtr[p]->polyline[pos + 1];
			curvePtr[p]->colour_blue = curvePtr[p]->polyline[pos + 2];
			
		} else if (curvePtr[p]->visible == GLUT_VISIBLE) {
//		   printf ("simultrace %d, kind %d, pdiv %d\n",simultrace, curvePtr[p]->kind, pdivcpy); 
			if ( ((simultrace == 0) &&  (curvePtr[p]->kind == DYNAMIC) && (p > pdivcpy)) || ((p <= pdivcpy) &&  (curvePtr[p]->kind == STATIC)) || (curvePtr[p]->colour == ' ') ) {	// static graphs or Simultrace == 0 or colour is black
//					printf("got to lc %c \n", curvePtr[p]->colour);
				glLineWidth   ( 1.10 ) ;  
			   ii=0;
			   if (curvePtr[p]->polyline) {

					glColor3f  ( 0.0 ,  0.0 , 0.0 ) ; // black default
					if (curvePtr[p]->colour == 'p') { // only does line segments for a 'plane' intersected curve
						if (curvePtr[p]->linetype == '-') {
							glLineWidth   ( curvePtr[p]->width ) ;  

							for (i=0; i < (curvePtr[p])->npolylinenodes - 1 ; i++ )  // draw the (curvePtr[p])
							   {
									if (  ( curvePtr[p]->polyline[ii+2] < awake) || (curvePtr[p]->polyline[ii+5] < awake) ) glColor3f  (10.0 ,  10.0 , 10.0 ) ; // disconnected
									else if (sleepwake -((curvePtr[p])->polyline[ii+2] + (curvePtr[p])->polyline[ii+5])/2 < 0) glColor3f  (0.0 ,  1.0 , 15.0 ) ; // set blue for the awake
									else if (sleep21 + ((curvePtr[p])->polyline[ii+1] + (curvePtr[p])->polyline[ii+4])/2 - ((curvePtr[p])->polyline[ii+2] + (curvePtr[p])->polyline[ii+5])/2  < 0) {
										if (sleepREM  + ((curvePtr[p])->polyline[ii+1] + (curvePtr[p])->polyline[ii+4])/2 - sleepREMslope *((curvePtr[p])->polyline[ii+2] + (curvePtr[p])->polyline[ii+5])/2  < 0) glColor3f  (15.0 ,  0.0 , 0.0 ) ;
										else glColor3f  (0.0 ,  15.0 , 0.0 ) ;
									}
									else glColor3f  (15.0 ,  15.0 , 0.0 ) ; // yellow

								glBegin ( GL_LINES ) ; { //GL_LINE_STRIP
								// draw the lines
								  glVertex3f ( (curvePtr[p])->polyline[ii] ,    (curvePtr[p])->polyline[ii+1],    (curvePtr[p])->polyline[ii+2]     ) ;
								  glVertex3f ( (curvePtr[p])->polyline[ii+3] ,    (curvePtr[p])->polyline[ii+4],    (curvePtr[p])->polyline[ii+5]     ) ;
								}  glEnd ( ) ;

							ii+=3;
							}
						}
						else if (curvePtr[p]->linetype == '.') {
							glPointSize (curvePtr[p]->width);
							for (i=0; i < (curvePtr[p])->npolylinenodes; i++ )  // draw the (curvePtr[p])
							   {
									if (curvePtr[p]->polyline[ii+2] < awake ) glColor3f  (10.0 ,  10.0 , 10.0 ) ; // disconnected
									else if (sleepwake -(curvePtr[p])->polyline[ii+2] < 0) glColor3f  (0.0 ,  1.0 , 15.0 ) ; // set blue for the awake
									else if (sleep21 + (curvePtr[p])->polyline[ii+1]  - (curvePtr[p])->polyline[ii+2]  < 0) {
										if (sleepREM  + (curvePtr[p])->polyline[ii+1]  - sleepREMslope * curvePtr[p]->polyline[ii+2]   < 0) glColor3f  (15.0 ,  0.0 , 0.0 ) ;
										else glColor3f  (0.0 ,  15.0 , 0.0 ) ;
									}
									else glColor3f  (15.0 ,  15.0 , 0.0 ) ; // yellow

								glBegin ( GL_POINTS ) ; { //points
								// draw the point
								  glVertex3f ( (curvePtr[p])->polyline[ii] ,    (curvePtr[p])->polyline[ii+1],    (curvePtr[p])->polyline[ii+2]     ) ;
								}  glEnd ( ) ;

							ii+=3;
							}
						}
						else if (curvePtr[p]->linetype == '|') {
							glPointSize (curvePtr[p]->width);
							for (i=0; i < (curvePtr[p])->npolylinenodes; i++ )  // draw the (curvePtr[p])
							   {
									if (curvePtr[p]->polyline[ii+2] < awake ) glColor3f  (10.0 ,  10.0 , 10.0 ) ; // disconnected
									else if (sleepwake -(curvePtr[p])->polyline[ii+2] < 0) glColor3f  (0.0 ,  1.0 , 15.0 ) ; // set blue for the awake
									else if (sleep21 + (curvePtr[p])->polyline[ii+1]  - (curvePtr[p])->polyline[ii+2]  < 0) {
										if (sleepREM  + (curvePtr[p])->polyline[ii+1]  - sleepREMslope * curvePtr[p]->polyline[ii+2]   < 0) glColor3f  (15.0 ,  0.0 , 0.0 ) ;
										else glColor3f  (0.0 ,  15.0 , 0.0 ) ;
									}
									else glColor3f  (15.0 ,  15.0 , 0.0 ) ; // yellow

								glBegin ( GL_LINES ) ; { // virtical line to x-axis
								// draw a vertical line
								  glVertex3f ( (curvePtr[p])->polyline[ii] ,    0,    0     ) ; // origin
								  glVertex3f ( (curvePtr[p])->polyline[ii] ,    (curvePtr[p])->polyline[ii+1],    (curvePtr[p])->polyline[ii+2]     ) ;
								}  glEnd ( ) ;

							ii+=3;
							}
						}
						

					} // done 'p's  which is the partitioned colours
					else 
					{ // do the other colours
//static
//**** connected curves
						if (curvePtr[p]->linetype == '_') { // does a faded trace on retrace

							glColor3f  (Tracelight * curvePtr[p]->colour_red * .2 +  background_r , Tracelight * curvePtr[p]->colour_green * .2  +  background_g, Tracelight * curvePtr[p]->colour_blue *.2 +  background_b) ;
							glLineWidth   ( curvePtr[p]->width ) ; 
							ii = 3 * (curvePtr[p]->npolylinenodes - 3) ; // two points back because we use a pair to draw a line segment
							for (  i = curvePtr[p]->npolylinenodes - 3 ; i >  curvePtr[p]->retrace + 3; i-- ) { // draw the (curvePtr[p])
								glBegin ( GL_LINES ) ; { //GL_LINE_STRIP
								// draw the lines
								  glVertex3f ( (curvePtr[p])->polyline[ii] ,    (curvePtr[p])->polyline[ii+1],    (curvePtr[p])->polyline[ii+2]     ) ;
								  glVertex3f ( (curvePtr[p])->polyline[ii+3] ,    (curvePtr[p])->polyline[ii+4],    (curvePtr[p])->polyline[ii+5]     ) ;
								}  glEnd ( ) ;
								ii-=3;
							}

				// from retrace to the start
							glColor3f  (Tracelight * curvePtr[p]->colour_red , Tracelight * curvePtr[p]->colour_green , Tracelight * curvePtr[p]->colour_blue ) ;
							ii = 3 * (curvePtr[p]->retrace - 3) ; // three back so that we never include the current retrace
							for ( i  =  curvePtr[p]->retrace - 3  ; i > 0; i-- ) { // draw the (curvePtr[p])
								glBegin ( GL_LINES ) ; { //GL_LINE_STRIP
								// draw the lines
								  glVertex3f ( (curvePtr[p])->polyline[ii] ,    (curvePtr[p])->polyline[ii+1],    (curvePtr[p])->polyline[ii+2]     ) ;
								  glVertex3f ( (curvePtr[p])->polyline[ii+3] ,    (curvePtr[p])->polyline[ii+4],    (curvePtr[p])->polyline[ii+5]     ) ;
								}  glEnd ( ) ;
								ii-=3;
							}
						} // end of linetype

						else if (curvePtr[p]->linetype == '-') {  // usual connected trace with overwrite
							glColor3f  (Tracelight * curvePtr[p]->colour_red , Tracelight * curvePtr[p]->colour_green , Tracelight * curvePtr[p]->colour_blue ) ;
							glLineWidth   ( curvePtr[p]->width ) ; 
							ii = 3 * (curvePtr[p]->npolylinenodes - 3) ; // two points back because we use a pair to draw a line segment
							for (  i = curvePtr[p]->npolylinenodes - 3 ; i >  curvePtr[p]->retrace + 3; i-- ) { // draw the (curvePtr[p])
								glBegin ( GL_LINES ) ; { //GL_LINE_STRIP
								// draw the lines
								  glVertex3f ( (curvePtr[p])->polyline[ii] ,    (curvePtr[p])->polyline[ii+1],    (curvePtr[p])->polyline[ii+2]     ) ;
								  glVertex3f ( (curvePtr[p])->polyline[ii+3] ,    (curvePtr[p])->polyline[ii+4],    (curvePtr[p])->polyline[ii+5]     ) ;
								}  glEnd ( ) ;
								ii-=3;
							}

				// from retrace to the start
							glColor3f  (Tracelight * curvePtr[p]->colour_red , Tracelight * curvePtr[p]->colour_green , Tracelight * curvePtr[p]->colour_blue ) ;
							ii = 3 * (curvePtr[p]->retrace - 3) ; // three back so that we never include the current retrace
							for ( i  =  curvePtr[p]->retrace - 3  ; i > 0; i-- ) { // draw the (curvePtr[p])
								glBegin ( GL_LINES ) ; { //GL_LINE_STRIP
								// draw the lines
								  glVertex3f ( (curvePtr[p])->polyline[ii] ,    (curvePtr[p])->polyline[ii+1],    (curvePtr[p])->polyline[ii+2]     ) ;
								  glVertex3f ( (curvePtr[p])->polyline[ii+3] ,    (curvePtr[p])->polyline[ii+4],    (curvePtr[p])->polyline[ii+5]     ) ;
								}  glEnd ( ) ;
								ii-=3;
							}
						} // end of linetype

// points

						else if (curvePtr[p]->linetype == '.') { // point type
							glColor3f  (Tracelight * curvePtr[p]->colour_red , Tracelight * curvePtr[p]->colour_green , Tracelight * curvePtr[p]->colour_blue ) ;
							glPointSize (curvePtr[p]->width);
	
							for (i=0; i < (curvePtr[p])->npolylinenodes ; i++ )  { // draw the (curvePtr[p])
								glBegin ( GL_POINTS ) ; { //points
								// draw the point
								  glVertex3f ( (curvePtr[p])->polyline[ii] ,    (curvePtr[p])->polyline[ii+1],    (curvePtr[p])->polyline[ii+2]     ) ;
								}  glEnd ( ) ;
								ii+=3;
							}
						} // end points
// drop lines
						else if (curvePtr[p]->linetype == '|') { // virtical type
							glColor3f  (Tracelight * curvePtr[p]->colour_red , Tracelight * curvePtr[p]->colour_green , Tracelight * curvePtr[p]->colour_blue ) ;
							glPointSize (curvePtr[p]->width);

							for (i=0; i < (curvePtr[p])->npolylinenodes ; i++ )  { // draw the (curvePtr[p])
								glBegin ( GL_LINES ) ; { //points
								// draw the point
								  glVertex3f ( (curvePtr[p])->polyline[ii] ,    0,  0 ) ;
								  glVertex3f ( (curvePtr[p])->polyline[ii] ,    (curvePtr[p])->polyline[ii+1],    (curvePtr[p])->polyline[ii+2]     ) ;
								}  glEnd ( ) ;
								ii+=3;
							}
						} // end virtical lines
// balls
						else if (curvePtr[p]->linetype == 'b') { //  balls
							glColor3f  ( surface_intsty *  (1-surface_shadow) * curvePtr[p]->colour_red +  surface_shadow, surface_intsty *  (1-surface_shadow) * curvePtr[p]->colour_green +  surface_shadow, surface_intsty  *  (1-surface_shadow) * curvePtr[p]->colour_blue +  surface_shadow ) ;
							for (i=0; i < (curvePtr[p])->npolylinenodes ; i++ )  { // draw the (curvePtr[p])
								// draw the ball

								glPushMatrix ( ) ;
								glTranslated( (curvePtr[p])->polyline[ii] ,    (curvePtr[p])->polyline[ii+1],    (curvePtr[p])->polyline[ii+2]     ) ;
								glutSolidSphere ((GLdouble) 0.1 * curvePtr[p]->width , 20, 20) ;
								glPopMatrix ( ) ;
								
								ii+=3;
							}
						} // end balls

// ball
						else if (curvePtr[p]->linetype == 'B') { //  single ball on the most recent entry
							glColor3f  ( surface_intsty * curvePtr[p]->colour_red +  surface_shadow, surface_intsty * curvePtr[p]->colour_green +  surface_shadow, surface_intsty * curvePtr[p]->colour_blue +  surface_shadow ) ;
							ii = 3 * curvePtr[p]->retrace;

							// draw the ball

							glPushMatrix ( ) ;
							glTranslated( (curvePtr[p])->polyline[ii] ,    (curvePtr[p])->polyline[ii+1],    (curvePtr[p])->polyline[ii+2]     ) ;
							glutSolidSphere ((GLdouble) 0.1 * curvePtr[p]->width , 20, 20) ;
							glPopMatrix ( ) ;
															
						} // end balls
// text
						else if (curvePtr[p]->linetype == 't') { //  text wipe  nned to add time to time coordinate
							glColor3f  (Tracelight * curvePtr[p]->colour_red , Tracelight * curvePtr[p]->colour_green , Tracelight * curvePtr[p]->colour_blue ) ;
							ii = 3 * curvePtr[p]->retrace;


							tx = curvePtr[p]->polyline[ii] + deltaoffset;
							ty = curvePtr[p]->polyline[ii+1] + deltaoffset;
							tz = curvePtr[p]->polyline[ii+2] + deltaoffset;

							switch (curvePtr[p]->timeaxis) {
								case '-':
										snprintf(&stringt[0],MAXTEXT, "T: %ld", oframe_counter ); 
										snprintf(&stringx[0],MAXTEXT, "X: %8.5f",  curvePtr[p]->width * curvePtr[p]->polyline[ii] );
										snprintf(&stringy[0],MAXTEXT, "Y: %8.5f", curvePtr[p]->width * curvePtr[p]->polyline[ii + 1] );
										snprintf(&stringz[0],MAXTEXT, "Z: %8.5f", curvePtr[p]->width * curvePtr[p]->polyline[ii+ 2] ); 
										glDisable		( GL_LIGHTING ) ;
											drawText3D(tx ,ty  , tz, stringt ) ;
											drawText3D(tx ,ty -= tinc, tz, stringx ) ;
											drawText3D(tx ,ty -= tinc, tz, stringy ) ;
											drawText3D(tx ,ty -= tinc, tz, stringz ) ;											
										glEnable		( GL_LIGHTING ) ;
									break;

								case 'z':
										snprintf(&stringt[0],MAXTEXT, "T: %ld", oframe_counter ); 
										snprintf(&stringx[0],MAXTEXT, "X: %8.5f",  curvePtr[p]->width * curvePtr[p]->polyline[ii] );
										snprintf(&stringy[0],MAXTEXT, "Y: %8.5f", curvePtr[p]->width * curvePtr[p]->polyline[ii + 1] );
										stringz[0] = 0; 
										glDisable		( GL_LIGHTING ) ;
											drawText3D(tx ,ty  , tz, stringt ) ;
											drawText3D(tx ,ty -= tinc, tz, stringx ) ;
											drawText3D(tx ,ty -= tinc, tz, stringy ) ;
										glEnable		( GL_LIGHTING ) ;
									break;

								case 'y':
										snprintf(&stringt[0],MAXTEXT, "T: %ld", oframe_counter ); 
										snprintf(&stringx[0],MAXTEXT, "X: %8.5f",  curvePtr[p]->width * curvePtr[p]->polyline[ii] );
										stringy[0] = 0;
										snprintf(&stringz[0],MAXTEXT, "Z: %8.5f", curvePtr[p]->width * curvePtr[p]->polyline[ii+ 2] ); 
										glDisable		( GL_LIGHTING ) ;
											drawText3D(tx ,ty  , tz, stringt ) ;
											drawText3D(tx ,ty -= tinc, tz, stringx ) ;
											drawText3D(tx ,ty -= tinc, tz, stringz ) ;											
										glEnable		( GL_LIGHTING ) ;
									break;
	
								
								case 'x':
								default:
										snprintf(&stringt[0],MAXTEXT, "T: %ld", oframe_counter ); //removed \000
										stringx[0]= 0;
										snprintf(&stringy[0],MAXTEXT, "Y: %8.5f", curvePtr[p]->width * curvePtr[p]->polyline[ii + 1] ); //removed \000
										snprintf(&stringz[0],MAXTEXT, "Z: %8.5f", curvePtr[p]->width * curvePtr[p]->polyline[ii+ 2] ); //removed \000
										glDisable		( GL_LIGHTING ) ;
											drawText3D(tx ,ty  , tz, stringt ) ;
											drawText3D(tx ,ty -= tinc, tz, stringy ) ;
											drawText3D(tx ,ty -= tinc, tz, stringz ) ;											
										glEnable		( GL_LIGHTING ) ;
									break;
	
							}

						} // end text

// Text
						else if (curvePtr[p]->linetype == 'T') { //  text wipe  nned to add time to time coordinate
							glColor3f  (Tracelight * curvePtr[p]->colour_red , Tracelight * curvePtr[p]->colour_green , Tracelight * curvePtr[p]->colour_blue ) ;
							ii = 3 * curvePtr[p]->retrace;


							tx = curvePtr[p]->polyline[ii] + deltaoffset;
							ty = curvePtr[p]->polyline[ii+1] + deltaoffset;
							tz = curvePtr[p]->polyline[ii+2] + deltaoffset;

							snprintf(&stringt[0],LABEL_SIZE, "%s", curvePtr[p]->label ); // need to check out the sizes of the destination buffer ... could be a problem here
						//	snprintf(&stringt[0],MAXTEXT, "%s", name of module ); 
							glDisable		( GL_LIGHTING ) ;
								drawText3D(tx ,ty  , tz, stringt ) ;
							glEnable		( GL_LIGHTING ) ;

						} // end Text Label

					
					} // end of other colours
				
				} // end if (curvePtr[p]->polyline)

			} // end of LC colour attribute (static curves)
//dynamic
			else if (curvePtr[p]->kind == DYNAMIC) // polygraph (DYNAMIC traces and graphs)
			{

				if (p <= pdivcpy) { // for all polygraph traces ...  toggle between polygraph trace and multi-static scan
				   if (curvePtr[p]->polyline) { // polygraph trace
						glColor3f  (15.0 * curvePtr[p]->colour_red , 15.0 * curvePtr[p]->colour_green , 15.0 * curvePtr[p]->colour_blue ) ;  // to do tag: look into this saturation factor of 15...
						
						ring_scan = (start_scan + 1.0)/2.0 ; // + ring buffer pointer ; // allow ring buffer handling
						if (ring_scan >=0) ring_scan = (ring_scan - ((int) ring_scan)) * 2.0 -1.0;
				//		else {};
				//		while (ring_scan > 1.0 ) ring_scan -= 2.0;
				//		while (ring_scan < -1.0 ) ring_scan += 2.0; // modulo the scaning range
						
						ii = 3 * (int) ((float) curvePtr[p]->npolylinenodes * (1.0 + ring_scan )/2.0) ; // -1.0 <= ring_scan <= 1.0
						nxtii = (ii + 3) % (3 * curvePtr[p]->npolylinenodes); // precalculate the next, to cut down on processing
						ring_ii = 0 ; // index for x scale only references always from the start of the array, instead of with respect to the data pointer
						
						switch (curvePtr[p]->timeaxis) {
							case '-':
								break;
							case 'z':
									if (curvePtr[p]->linetype == '-') { // trace
										glLineWidth   ( curvePtr[p]->width ) ; 
										while (ring_ii < 3 * ( curvePtr[p]->npolylinenodes - 10))  {
											if ((curvePtr[p]->polyline[ring_ii+3] > xorig - xlimit ) && ( curvePtr[p]->polyline[ring_ii] != curvePtr[p]->polyline[ring_ii + 3 ] )) {
												glBegin ( GL_LINES ) ; { // GL_LINE_STRIP				
												// draw the lines
												  glVertex3f (curvePtr[p]->polyline[ii] ,    (curvePtr[p])->polyline[ii+1],    (curvePtr[p])->polyline[ring_ii+2]     ) ;
												  glVertex3f (curvePtr[p]->polyline[nxtii] ,    (curvePtr[p])->polyline[nxtii+1],    (curvePtr[p])->polyline[(ring_ii+=3) + 2]     ) ;
												}  glEnd ( ) ;
											}
											else ring_ii+=3 ;
											ii = nxtii; nxtii = (ii + 3) % (3 * curvePtr[p]->npolylinenodes); // precalculate next
										} // end while
									} // end of trace
			
									else if (curvePtr[p]->linetype == '.') { // points
									glPointSize (curvePtr[p]->width);
										while (ring_ii < 3 * ( curvePtr[p]->npolylinenodes - 10))  {
											if ( (curvePtr[p]->polyline[ring_ii+3] > xorig - xlimit ) && ( curvePtr[p]->polyline[ring_ii] != curvePtr[p]->polyline[ring_ii + 3 ] )) {
												glBegin ( GL_POINTS ) ; {	// GL_LINE_STRIP				
												// draw the point
												  glVertex3f ( curvePtr[p]->polyline[nxtii]  ,    (curvePtr[p])->polyline[nxtii+1],    (curvePtr[p])->polyline[(ring_ii+=3)+2]     ) ;
												}  glEnd ( ) ;
											}
											else ring_ii+=3 ;
											ii = nxtii; nxtii = (ii + 3) % (3 * curvePtr[p]->npolylinenodes);
										} // end while
									} // end of points
			
									else if ( curvePtr[p]->linetype == '|') { // virtical lines
									glPointSize (curvePtr[p]->width);
										while (ring_ii < 3 * ( curvePtr[p]->npolylinenodes - 10))  {
											if ( (curvePtr[p]->polyline[ring_ii+3] > xorig - xlimit ) && ( curvePtr[p]->polyline[ring_ii] != curvePtr[p]->polyline[ring_ii + 3 ] )) {
												glBegin ( GL_LINES ) ; {	// GL_LINE_STRIP				
												// draw the drop line
												  glVertex3f (  0,  0, curvePtr[p]->polyline[(ring_ii+=3) + 2]  ) ;
												  glVertex3f ( curvePtr[p]->polyline[nxtii]  ,    (curvePtr[p])->polyline[nxtii+1],    (curvePtr[p])->polyline[ring_ii+2]     ) ;
												}  glEnd ( ) ;
											}
											else ring_ii+=3 ;
											ii = nxtii; nxtii = (ii + 3) % (3 * curvePtr[p]->npolylinenodes);
										} // end while
									} // end of virtical lines
								break;
							case 'y':
									if (curvePtr[p]->linetype == '-') { // trace
										glLineWidth   ( curvePtr[p]->width ) ; 
										while (ring_ii < 3 * ( curvePtr[p]->npolylinenodes - 10))  {
											if ((curvePtr[p]->polyline[ring_ii+3] > xorig - xlimit ) && ( curvePtr[p]->polyline[ring_ii] != curvePtr[p]->polyline[ring_ii + 3 ] )) {
												glBegin ( GL_LINES ) ; { // GL_LINE_STRIP				
												// draw the lines
												  glVertex3f (curvePtr[p]->polyline[ii] ,    curvePtr[p]->polyline[ring_ii+1],    curvePtr[p]->polyline[ii+2]     ) ;
												  glVertex3f (curvePtr[p]->polyline[nxtii] ,    curvePtr[p]->polyline[(ring_ii+=3)+1],    curvePtr[p]->polyline[nxtii+2]     ) ;
												}  glEnd ( ) ;
											}
											else ring_ii+=3 ;
											ii = nxtii; nxtii = (ii + 3) % (3 * curvePtr[p]->npolylinenodes); // precalculate next
										} // end while
									} // end of trace
			
									else if (curvePtr[p]->linetype == '.') { // points
									glPointSize (curvePtr[p]->width);
										while (ring_ii < 3 * ( curvePtr[p]->npolylinenodes - 10))  {
											if ( (curvePtr[p]->polyline[ring_ii+3] > xorig - xlimit ) && ( curvePtr[p]->polyline[ring_ii] != curvePtr[p]->polyline[ring_ii + 3 ] )) {
												glBegin ( GL_POINTS ) ; {	// GL_LINE_STRIP				
												// draw the point
												  glVertex3f ( curvePtr[p]->polyline[nxtii]  ,    curvePtr[p]->polyline[(ring_ii+=3)+1],    curvePtr[p]->polyline[nxtii+2]     ) ;
												}  glEnd ( ) ;
											}
											else ring_ii+=3 ;
											ii = nxtii; nxtii = (ii + 3) % (3 * curvePtr[p]->npolylinenodes);
										} // end while
									} // end of points
			
									else if ( curvePtr[p]->linetype == '|') { // virtical lines
									glPointSize (curvePtr[p]->width);
										while (ring_ii < 3 * ( curvePtr[p]->npolylinenodes - 10))  {
											if ( (curvePtr[p]->polyline[ring_ii+3] > xorig - xlimit ) && ( curvePtr[p]->polyline[ring_ii] != curvePtr[p]->polyline[ring_ii + 3 ] )) {
												glBegin ( GL_LINES ) ; {	// GL_LINE_STRIP				
												// draw the drop line
												  glVertex3f (   0, curvePtr[p]->polyline[(ring_ii+=3) + 1]  ,  0  ) ;
												  glVertex3f ( curvePtr[p]->polyline[nxtii]  ,    curvePtr[p]->polyline[ring_ii+1],    curvePtr[p]->polyline[nxtii+2]     ) ;
												}  glEnd ( ) ;
											}
											else ring_ii+=3 ;
											ii = nxtii; nxtii = (ii + 3) % (3 * curvePtr[p]->npolylinenodes);
										} // end while
									} // end of virtical lines
								break;
							case 'x':
							default:
									if (curvePtr[p]->linetype == '-') { // trace
										glLineWidth   ( curvePtr[p]->width ) ; 
										while (ring_ii < 3 * ( curvePtr[p]->npolylinenodes - 10))  {
											if ((curvePtr[p]->polyline[ring_ii+3] > xorig - xlimit ) && ( curvePtr[p]->polyline[ring_ii] != curvePtr[p]->polyline[ring_ii + 3 ] )) {
												glBegin ( GL_LINES ) ; { // GL_LINE_STRIP				
												// draw the lines
												  glVertex3f (curvePtr[p]->polyline[ring_ii] ,    curvePtr[p]->polyline[ii+1],    curvePtr[p]->polyline[ii+2]     ) ;
												  glVertex3f (curvePtr[p]->polyline[ring_ii+=3] ,    curvePtr[p]->polyline[nxtii+1],    curvePtr[p]->polyline[nxtii+2]     ) ;
												}  glEnd ( ) ;
											}
											else ring_ii+=3 ;
											ii = nxtii; nxtii = (ii + 3) % (3 * curvePtr[p]->npolylinenodes); // precalculate next
										} // end while
									} // end of trace
			
									else if (curvePtr[p]->linetype == '.') { // points
									glPointSize (curvePtr[p]->width);
										while (ring_ii < 3 * ( curvePtr[p]->npolylinenodes - 10))  {
											if ( (curvePtr[p]->polyline[ring_ii+3] > xorig - xlimit ) && ( curvePtr[p]->polyline[ring_ii] != curvePtr[p]->polyline[ring_ii + 3 ] )) {
												glBegin ( GL_POINTS ) ; {	// GL_LINE_STRIP				
												// draw the point
												  glVertex3f ( curvePtr[p]->polyline[ring_ii+=3]  ,    (curvePtr[p])->polyline[nxtii+1],    (curvePtr[p])->polyline[nxtii+2]     ) ;
												}  glEnd ( ) ;
											}
											else ring_ii+=3 ;
											ii = nxtii; nxtii = (ii + 3) % (3 * curvePtr[p]->npolylinenodes);
										} // end while
									} // end of points
			
									else if ( curvePtr[p]->linetype == '|') { // vertical lines
									glPointSize (curvePtr[p]->width);
										while (ring_ii < 3 * ( curvePtr[p]->npolylinenodes - 10))  {
											if ( (curvePtr[p]->polyline[ring_ii+3] > xorig - xlimit ) && ( curvePtr[p]->polyline[ring_ii] != curvePtr[p]->polyline[ring_ii + 3 ] )) {
												glBegin ( GL_LINES ) ; {	// GL_LINE_STRIP				
												// draw the drop line
												  glVertex3f ( curvePtr[p]->polyline[ring_ii+=3]  ,   0,  0  ) ;
												  glVertex3f ( curvePtr[p]->polyline[ring_ii]  ,    (curvePtr[p])->polyline[nxtii+1],    (curvePtr[p])->polyline[nxtii+2]     ) ;
												}  glEnd ( ) ;
											}
											else ring_ii+=3 ;
											ii = nxtii; nxtii = (ii + 3) % (3 * curvePtr[p]->npolylinenodes);
										} // end while
									} // end of vertical lines
									
									else if ( curvePtr[p]->linetype == 't') { // text
										ring_ii = 3 * ( curvePtr[p]->npolylinenodes - 10);
										ii = 3 *  curvePtr[p]->retrace ;
										
										glDisable		( GL_LIGHTING ) ;
										switch (curvePtr[p]->timeaxis) {
											case '-':
													snprintf(&stringt[0],MAXTEXT, "T: %ld", oframe_counter ); 
													snprintf(&stringx[0],MAXTEXT, "X: %8.5f",  curvePtr[p]->width * curvePtr[p]->polyline[ii] );
													snprintf(&stringy[0],MAXTEXT, "Y: %8.5f", curvePtr[p]->width * curvePtr[p]->polyline[ii + 1] );
													snprintf(&stringz[0],MAXTEXT, "Z: %8.5f", curvePtr[p]->width * curvePtr[p]->polyline[ii+ 2] ); 
													tx = curvePtr[p]->polyline[ring_ii] + deltaoffset;
													ty = curvePtr[p]->polyline[ii+1] + deltaoffset;
													tz = curvePtr[p]->polyline[ii+2] + deltaoffset;
													drawText3D(tx ,ty  , tz, stringt ) ;
													drawText3D(tx ,ty += tinc, tz, stringx ) ;
													drawText3D(tx ,ty += tinc, tz, stringy ) ;
													drawText3D(tx ,ty += tinc, tz, stringz ) ;											
												break;
			
											case 'z':
													snprintf(&stringt[0],MAXTEXT, "T: %ld", oframe_counter ); 
													snprintf(&stringx[0],MAXTEXT, "X: %8.5f",  curvePtr[p]->width * curvePtr[p]->polyline[ii] );
													snprintf(&stringy[0],MAXTEXT, "Y: %8.5f", curvePtr[p]->width * curvePtr[p]->polyline[ii + 1] );
													stringz[0] = 0; 
													tx = curvePtr[p]->polyline[ii] + deltaoffset;
													ty = curvePtr[p]->polyline[ii+1] + deltaoffset;
													tz = curvePtr[p]->polyline[ring_ii+2] + deltaoffset;
													drawText3D(tx ,ty  , tz, stringt ) ;
													drawText3D(tx ,ty += tinc, tz, stringx ) ;
													drawText3D(tx ,ty += tinc, tz, stringy ) ;
												break;
			
											case 'y':
													snprintf(&stringt[0],MAXTEXT, "T: %ld", oframe_counter ); 
													snprintf(&stringx[0],MAXTEXT, "X: %8.5f",  curvePtr[p]->width * curvePtr[p]->polyline[ii] );
													stringy[0] = 0;
													snprintf(&stringz[0],MAXTEXT, "Z: %8.5f", curvePtr[p]->width * curvePtr[p]->polyline[ii+ 2] ); 
													tx = curvePtr[p]->polyline[ii] + deltaoffset;
													ty = curvePtr[p]->polyline[ring_ii+1] + deltaoffset;
													tz = curvePtr[p]->polyline[ii+2] + deltaoffset;
													drawText3D(tx ,ty  , tz, stringt ) ;
													drawText3D(tx ,ty += tinc, tz, stringx ) ;
													drawText3D(tx ,ty += tinc, tz, stringz ) ;											
												break;
				
											
											case 'x':
											default:
													snprintf(&stringt[0],MAXTEXT, "T: %ld", oframe_counter ); 
													stringx[0]= 0;
													snprintf(&stringy[0],MAXTEXT, "Y: %8.5f", curvePtr[p]->width * curvePtr[p]->polyline[ii + 1] );
													snprintf(&stringz[0],MAXTEXT, "Z: %8.5f", curvePtr[p]->width * curvePtr[p]->polyline[ii+ 2] ); 
													tx = curvePtr[p]->polyline[ring_ii] + deltaoffset;
													ty = curvePtr[p]->polyline[ii+1] + deltaoffset;
													tz = curvePtr[p]->polyline[ii+2] + deltaoffset;
													drawText3D(tx ,ty  , tz, stringt ) ;
													drawText3D(tx ,ty += tinc, tz, stringy ) ;
													drawText3D(tx ,ty += tinc, tz, stringz ) ;											
												break;
				
										}
										glEnable		( GL_LIGHTING ) ;
							
									
									} 
							break;
						}
					} // end polygraph trace
				} // end simultrace == 0
				else if ((simultrace != 0) && (p > pdivcpy)) // and p > pdivcpy then display selected traces simultaneously
				{
					intensity = 2.0 * ((float) (p-pdivcpy)/ (float) (Num_curve-pdivcpy)  - (start_scan + 1.0)/2.0) / width_scan ; // in the range of 0.0 - 1.0 
					if ((intensity >= 0 )  && ( intensity <=  1.0 ) ) { // polyline is in the window so display it

//						if (scan_incr < 0) intensity = 1.0 - intensity; // if travelling back in time reverse persistence
//						else 
//						if (scan_incr == 0) intensity = 1.0; // if stopped give all curves equal brightness

						zdepth = ydepth = 0;
						if ( flat == 1 )  ydepth = 10.0* intensity - 5;
						else if ( flat == 2 ) zdepth = 10.0* intensity - 5;
						else if (scan_incr == 0) intensity = 1.0;

						intensity = sqrt(sqrt(intensity)); // make the fade out more sudden... compress the rest

						glLineWidth   ( 1.10 ) ; 
						 
					
						glColor3f  (intensity * curvePtr[p]->colour_red , intensity * curvePtr[p]->colour_green , intensity * curvePtr[p]->colour_blue ) ;

						ii=0;
						if (curvePtr[p]->linetype == '-') {
							glLineWidth   ( curvePtr[p]->width ) ; 
							if (curvePtr[p]->polyline) for (i=0; i < curvePtr[p]->npolylinenodes - 1 ; i++ ) { // draw the (curvePtr[p])
								if (!((i == curvePtr[p]->retrace  ) || (i+1 == curvePtr[p]->retrace  )) ){
									glBegin ( GL_LINES ) ; { // GL_LINE_STRIP
									// draw the lines
									  glVertex3f ( curvePtr[p]->polyline[ii] ,    curvePtr[p]->polyline[ii+1] - ydepth ,   curvePtr[p]->polyline[ii+2] + zdepth    ) ;
									  glVertex3f ( curvePtr[p]->polyline[ii+3] ,    curvePtr[p]->polyline[ii+4] - ydepth,   curvePtr[p]->polyline[ii+5] + zdepth    ) ;
									}  glEnd ( ) ;
								}
							ii+=3;
							}
						}
						else if (curvePtr[p]->linetype == '.') {
						glPointSize (curvePtr[p]->width);
							if (curvePtr[p]->polyline) for (i=0; i < curvePtr[p]->npolylinenodes ; i++ ) { // draw the (curvePtr[p])
								glBegin ( GL_POINTS ) ; { // points
								// draw the point
								  glVertex3f ( curvePtr[p]->polyline[ii] ,    curvePtr[p]->polyline[ii+1] - ydepth ,   curvePtr[p]->polyline[ii+2] + zdepth    ) ;
								}  glEnd ( ) ;
							ii+=3;
							}
						}
						else if (curvePtr[p]->linetype == '|') { // virtical line type
						glPointSize (curvePtr[p]->width);
							if (curvePtr[p]->polyline) for (i=0; i < curvePtr[p]->npolylinenodes ; i++ ) { // draw the (curvePtr[p])
								glBegin ( GL_LINES ) ; { // points
								// draw the line
								  glVertex3f ( curvePtr[p]->polyline[ii] ,  0 ,  0    ) ;
								  glVertex3f ( curvePtr[p]->polyline[ii] ,    curvePtr[p]->polyline[ii+1] - ydepth ,   curvePtr[p]->polyline[ii+2] + zdepth    ) ;
								}  glEnd ( ) ;
							ii+=3;
							}
						}
						else if (curvePtr[p]->linetype == '/') { // vector to origin
						glPointSize (curvePtr[p]->width);
							if (curvePtr[p]->polyline) for (i=0; i < curvePtr[p]->npolylinenodes ; i++ ) { // draw the (curvePtr[p])
								glBegin ( GL_LINES ) ; { // points
								// draw the line
								  glVertex3f ( 0 ,  0 ,  0    ) ;
								  glVertex3f ( curvePtr[p]->polyline[ii] ,    curvePtr[p]->polyline[ii+1] - ydepth ,   curvePtr[p]->polyline[ii+2] + zdepth    ) ;
								}  glEnd ( ) ;
							ii+=3;
							}
						}

				

					} // end of polyline window.
				} // end of simul == 1
			} // end else
		} // end while (visble)
	} // end of curve treatment


	glLineWidth   ( 1.00 ) ;  
}


/* ========================================================================= */  
/* paint_polygons_lines                                                      */
/* ========================================================================= */  
/*
** Paints the polygons of the surface. 
** Type indicates if they are to be 
** drawn         (type=0), 
** flat shaded   (type=1),
** smooth shaded (type=2).
*/

void paint_polygons ( SURF **surfacePtr, int Num_surface, long type, int spinx, int spiny)
{
  int p, q, s, curv, pos, pstart, pinc, pdone, plabel, reversescandir;
  long    i, j, k, ii,	nxtii, ring_ii;
  float  v1[3], v2[3], v3[3] ;
  float  norm1[3], norm2[3], norm3[3] ;
  float  vn1[3], vn2[3], vn3[3], AvgColour;
  float	ring_scan, shift, coeff;


	if (( ((spinx >= 0) && (spinx <= 90)) || ((spinx > 270) && (spinx < 360)) ) && ((spiny >= 0) && (spiny <= 180))) {reversescandir = TRUE;}
	else {reversescandir = FALSE;}
	
	for (q=0; q < Num_surface; q++) {
		if (surfacePtr[q]->visible == GLUT_VISIBLE) {
			calculate_polygon_vertex_normal ( surfacePtr,q ) ;
			if ((surfacePtr[q]->kind == STATIC) || (surfacePtr[q]->kind == FULLYSPECIFIED)) {
				for (k=0; k<(surfacePtr[q])->npolygons; k++ ) {
				    if (reversescandir==TRUE) {i = (surfacePtr[q])->npolygons - 1 - k;}
				    else  {i = k;}

//				for (i=0; i<(surfacePtr[q])->npolygons; i++ ) {

				  	for (j=0; j<3; j++) {
						v1[j] = (surfacePtr[q])->polygon[i]->vertex[0]->xyz[j] ;
						v2[j] = (surfacePtr[q])->polygon[i]->vertex[1]->xyz[j] ;
						v3[j] = (surfacePtr[q])->polygon[i]->vertex[2]->xyz[j] ;
					}
					

		// rendering only if not all 3 polygon vertices in the zero plane
					if (( v1[2] != surfacePtr[q]->zoffset) || ( v2[2] != surfacePtr[q]->zoffset) || ( v3[2] != surfacePtr[q]->zoffset)) {
	
						if ( type ) { 
							if (surfacePtr[q]->kind == STATIC) { 
						// averaging z-values to estimate a colour for the polygon							
								if (v1[0] == v2[0])
									AvgColour =  v3[2]/2.0  + (v1[2] + v2[2])/4.0 - surfacePtr[q]->zoffset;
								
								else if (v2[0] == v3[0])
									AvgColour = v1[2]/2.0  + (v2[2] + v3[2])/4.0 - surfacePtr[q]->zoffset;
								
								else if (v3[0] == v1[0])
									AvgColour = v2[2]/2.0  + (v1[2] + v3[2])/4.0 - surfacePtr[q]->zoffset;
								glColor3f	( (double) Glowr + ScaleIntensity*AvgColour ,  (double) Glowg - ScaleIntensity*AvgColour , (double) Glowb) ; // set the colour
							}
							else if (surfacePtr[q]->kind == FULLYSPECIFIED) { 
	//							glDepthMask(GL_FALSE) ;
								// basically i runs through the polygons... we could use this to call up the colour functions directly
								glColor3f  ((surfacePtr[q])->polygon[i]->rgb[0], (surfacePtr[q])->polygon[i]->rgb[1], (surfacePtr[q])->polygon[i]->rgb[2]);
							}
	
						} /* endif painted */
						else { 
							glColor3f  (1.0 ,  1.0 , 1.0) ; // colour of tesselation map
						}
		
		
						if ( type == 0 ) { // triangular grid
					
							for (j=0; j<3; j++) {
							  norm1[j] = (surfacePtr[q])->polygon[i]->vertex[0]->norm[j] ;
							  norm2[j] = (surfacePtr[q])->polygon[i]->vertex[1]->norm[j] ;
							  norm3[j] = (surfacePtr[q])->polygon[i]->vertex[2]->norm[j] ;
							}
							glBegin ( GL_LINE_LOOP ) ; {
							  glNormal3f ( norm1[0], norm1[1], norm1[2] ) ;
							  glVertex3f ( v1[0],    v1[1],    v1[2]    ) ;
							  glNormal3f ( norm2[0], norm2[1], norm2[2] ) ;
							  glVertex3f ( v2[0],    v2[1],    v2[2]    ) ;
							  glNormal3f ( norm3[0], norm3[1], norm3[2] ) ;
							  glVertex3f ( v3[0],    v3[1],    v3[2]    ) ;
							}  glEnd ( ) ;
		
						} /* end if drawn */
		
	
	
						else if (( type == 1 ) || ( type == 4 )) {  // unsmoothed ... without/with norms
					
							for (j=0; j<3; j++) {
							  norm1[j] = (surfacePtr[q])->polygon[i]->vertex[0]->norm[j] ;
							  norm2[j] = (surfacePtr[q])->polygon[i]->vertex[1]->norm[j] ;
							  norm3[j] = (surfacePtr[q])->polygon[i]->vertex[2]->norm[j] ;
							}
							
							glBegin ( GL_TRIANGLES ) ; {
							  glNormal3f ( norm1[0], norm1[1], norm1[2] ) ;
							  glVertex3f ( v1[0],    v1[1],    v1[2]    ) ;
							  glNormal3f ( norm2[0], norm2[1], norm2[2] ) ;
							  glVertex3f ( v2[0],    v2[1],    v2[2]    ) ;
							  glNormal3f ( norm3[0], norm3[1], norm3[2] ) ;
							  glVertex3f ( v3[0],    v3[1],    v3[2]    ) ;
							}  glEnd ( ) ;
						} /* end if drawn */
		
						else if (( type == 2 ) || ( type == 5 )) { // smoothed ... without/with norms
		
							averaged_vertex_normals ( surfacePtr[q], i, norm1, norm2, norm3 ) ;
							glBegin ( GL_TRIANGLES ) ; { 
							
							  glNormal3f ( norm1[0], norm1[1], norm1[2] ) ;
							  glVertex3f ( v1[0],    v1[1],    v1[2]    ) ;
							  glNormal3f ( norm2[0], norm2[1], norm2[2] ) ;
							  glVertex3f ( v2[0],    v2[1],    v2[2]    ) ;
							  glNormal3f ( norm3[0], norm3[1], norm3[2] ) ;
							  glVertex3f ( v3[0],    v3[1],    v3[2]    ) ;
							}  glEnd ( ) ;
		
						} /* end if smoothed */
		
						else if (type == 3) { //  colour only ... no height
						glBegin ( GL_TRIANGLES ) ; { 
							  glNormal3f ( 0, -1, 0 ) ;
							  glVertex3f ( v1[0],    v1[1],  ( v1[2] = 0 )   ) ;
							  glNormal3f ( 0, -1, 0 ) ;
							  glVertex3f ( v2[0],    v2[1],  ( v2[2] = 0 )   ) ;
							  glNormal3f ( 0, -1, 0 ) ;
							  glVertex3f ( v3[0],    v3[1],  ( v3[2] = 0 )	 ) ;
							}  glEnd ( ) ;

						}
						
						if (( type == 4 ) || ( type == 5 ) || ( type == 6 )) { // draw normals
							if (type == 6) averaged_vertex_normals ( surfacePtr[q], i, norm1, norm2, norm3 ) ;
							for (j=0; j<3; j++) {
								vn1[j] = (surfacePtr[q])->polygon[i]->vertex[0]->xyz[j] - .5* norm1[j] ;
								vn2[j] = (surfacePtr[q])->polygon[i]->vertex[1]->xyz[j] - .5*  norm2[j] ;
								vn3[j] = (surfacePtr[q])->polygon[i]->vertex[2]->xyz[j] - .5*  norm3[j] ;
							}
		
							glBegin ( GL_LINE_STRIP  ) ; { // GL_LINE_STRIP GL_LINES
								glVertex3f ( (v1[0] + v2[0] + v3[0])/3.0 ,     (v1[1] + v2[1] + v3[1])/3.0 ,     (v1[2] + v2[2] + v3[2])/3.0      ) ;
								glVertex3f ( (vn1[0] +  vn2[0] +  vn3[0]) /3.0 ,  (vn1[1] +  vn2[1] +  vn3[1]) /3.0 , (vn1[2] +  vn2[2] +  vn3[2]) /3.0   ) ;
							}  glEnd ( ) ;



/*							glBegin ( GL_LINE_STRIP  ) ; { // GL_LINE_STRIP GL_LINES
								glVertex3f ( v1[0],    v1[1],    v1[2]     ) ;
								glVertex3f ( vn1[0],  vn1[1],    vn1[2]    ) ;
							}  glEnd ( ) ;
		
		
							glBegin ( GL_LINES ) ; {
								glVertex3f ( v2[0],    v2[1],    v2[2]     ) ;
								glVertex3f ( vn2[0],  vn2[1],    vn2[2]    ) ;
							}  glEnd ( ) ;
		
		
							glBegin ( GL_LINES ) ; {
								glVertex3f ( v3[0],    v3[1],    v3[2]     ) ;
								glVertex3f ( vn3[0],  vn3[1],    vn3[2]    ) ;
							}  glEnd ( ) ;
*/							
		
						} // if (normals)
					} // rendering if at least one polygon vertices not in the zero plane
				} // for (i=0; i<(surfacePtr[q])->npolygons; i++ ) 
			} // if ((surfacePtr[q])->kind == STATIC) 

/*						ring_scan = start_scan ; // + ring buffer pointer ; // allow ring buffer handling
						if (ring_scan > 1.0 ) ring_scan -= 2.0; else if (ring_scan < -1.0 ) ring_scan += 2.0 ; // modulo the scaning range
						ii = 3 * (int) ((float) curvePtr[p]->npolylinenodes * (1.0 + ring_scan )/2.0) ; // -1.0 <= ring_scan <= 1.0
						nxtii = (ii + 3) % (3 * curvePtr[p]->npolylinenodes); // precalculate the next, to cut down on processing
						ring_ii = 0 ; // temporary index references always from the start of the array, instead of with respect to the pointers
						shift = curvePtr[p]->xoffset -curvePtr[p]->xscale;
						coeff = 2*(curvePtr[p]->xscale) / width_scan;
						if (curvePtr[p]->linetype == '-') { // trace
							glLineWidth   ( curvePtr[p]->width ) ; 
							while ((curvePtr[p]->polyline[ring_ii] < -1 + width_scan) )  {
								if (curvePtr[p]->polyline[ring_ii+3] > -1) {
									glBegin ( GL_LINES ) ; { // GL_LINE_STRIP				
									// draw the lines
									  glVertex3f (shift + coeff *  (curvePtr[p]->polyline[ring_ii] + 1) ,    (curvePtr[p])->polyline[ii+1],    (curvePtr[p])->polyline[ii+2]     ) ;
									  glVertex3f (shift + coeff * (curvePtr[p]->polyline[ring_ii+=3] + 1) ,    (curvePtr[p])->polyline[nxtii+1],    (curvePtr[p])->polyline[nxtii+2]     ) ;
									}  glEnd ( ) ;
								}
								ii = nxtii; nxtii = (ii + 3) % (3 * curvePtr[p]->npolylinenodes); // precalculate next
											
							} // end while
						} // end of trace
*/
		} // VISIBLE
	} // for (q=0; q < Num_surface; q++) {
  glLineWidth   ( 1.0 ) ;  // reset line size
} 

/* ========================================================================= */  
/* paint ball.					     */
/* ========================================================================= */
/*
** As it says.
*/
void paint_ball ( LINE **curvePtr, BALL **ballPtr)
{
  int  	s, curv, pos;
 // long  ii,	nxtii, ring_ii;
  float	ring_scan, red, green, blue;
  char	label[LABEL_SIZE];


	s = 0;
	while ( ballPtr[s]->ball > -1 ) {
		curv = ballPtr[s]->ball;
		ring_scan = (start_scan  + 1.0 + width_scan/2.0)/2.0; 
		if (ring_scan >=0) ring_scan = (ring_scan - ((int) ring_scan)) * 2.0 -1.0;

		pos = (int) ((float) curvePtr[curv]->npolylinenodes * (1.0 + ring_scan )/2.0);
		pos = 3*pos;


		if (curvePtr[curv]->kind == COLOUR_FUNC) { //test ball placed at origion
			ballPtr[s]->xval = 0; ballPtr[s]->yval = 0; ballPtr[s]->zval = 0;
				glEnable		( GL_LIGHTING ) ;
				glPushMatrix ( ) ;
					
					glTranslated (ballPtr[s]->txt_xoffset,  ballPtr[s]->txt_yoffset, ballPtr[s]->txt_zoffset ) ;
						red = ballPtr[s]->xscale * curvePtr[curv]->polyline[pos] + ballPtr[s]->xoffset; 
						green = ballPtr[s]->yscale * curvePtr[curv]->polyline[pos+1] + ballPtr[s]->yoffset; 
						blue = ballPtr[s]->zscale * curvePtr[curv]->polyline[pos+2] + ballPtr[s]->zoffset; 
					//	if (red > 1.0) red = 1.0;																//need to think about this further
					//	else 
						if (red < 0.0) red = 0.0;
					//	if (green > 1.0) green = 1.0;
					//	else 
						if (green < 0.0) green = 0.0;
					//	if (blue > 1.0) blue = 1.0;
					//	else 
						if (blue < 0.0) blue = 0.0;
						glColor3f ( red, green, blue); // use values to give colour

					switch (ballPtr[s]->type) {
						case 's' :
							glutSolidSphere ((GLdouble) ballPtr[s]->size , 50, 50) ; // ball
						break;
						case 't' :
							glutSolidTeapot ((GLdouble) ballPtr[s]->size); //teaPot
						break;
						case 'b' :
							glutSolidCube ((GLdouble) ballPtr[s]->size);  //cube
						break;
						case 'c' :
							glutSolidCone  ((GLdouble) (ballPtr[s]->size)/2 , (GLdouble) ballPtr[s]->size, 50, 50) ;
						break;
						case 'd' :
							glutSolidTorus ((GLdouble) (ballPtr[s]->size)/2 , (GLdouble) ballPtr[s]->size, 50, 50) ;
						break;
					}  
			   glPopMatrix ( ) ;
			
		}
		else if (curvePtr[curv]->visible == GLUT_VISIBLE) {

			// caculate scaled/offset values to ball structure 
			ballPtr[s]->xval = ballPtr[s]->xscale * curvePtr[curv]->polyline[pos] + ballPtr[s]->xoffset;   // compute scaled / offset value x
			ballPtr[s]->yval = ballPtr[s]->yscale * curvePtr[curv]->polyline[pos+1] + ballPtr[s]->yoffset; // compute scaled / offset value y
			ballPtr[s]->zval = ballPtr[s]->zscale * curvePtr[curv]->polyline[pos+2] + ballPtr[s]->zoffset; // compute scaled / offset value z
			
			// what combination of drawText3Ds
			if ((ballPtr[s]->xscale) && (ballPtr[s]->yscale) && (ballPtr[s]->zscale)) snprintf(&label[0], LABEL_SIZE, "%s [%0.3f][%0.3f][%0.3f]", ballPtr[s]->label, ballPtr[s]->xval, ballPtr[s]->yval, ballPtr[s]->zval );
			else if ((ballPtr[s]->xscale) && (ballPtr[s]->yscale)) snprintf(&label[0], LABEL_SIZE, "%s [%0.3f][%0.3f]", ballPtr[s]->label, ballPtr[s]->xval, ballPtr[s]->yval );
			else if ((ballPtr[s]->xscale) && (ballPtr[s]->zscale)) snprintf(&label[0], LABEL_SIZE, "%s [%0.3f][%0.3f]", ballPtr[s]->label, ballPtr[s]->xval, ballPtr[s]->zval );
			else if ((ballPtr[s]->yscale) && (ballPtr[s]->zscale)) snprintf(&label[0], LABEL_SIZE, "%s [%0.3f][%0.3f]", ballPtr[s]->label, ballPtr[s]->yval, ballPtr[s]->zval );
			else if (ballPtr[s]->xscale) snprintf(&label[0], LABEL_SIZE, "%s [%0.3f]", ballPtr[s]->label, ballPtr[s]->xval );
			else if (ballPtr[s]->yscale) snprintf(&label[0], LABEL_SIZE, "%s [%0.3f]", ballPtr[s]->label, ballPtr[s]->yval );
			else if (ballPtr[s]->zscale) snprintf(&label[0], LABEL_SIZE, "%s [%0.3f]", ballPtr[s]->label, ballPtr[s]->zval );
			else  snprintf(&label[0], LABEL_SIZE, "%s", ballPtr[s]->label);
			
			
			if ((ballPtr[s]->txt_xoffset == 0) && (ballPtr[s]->txt_yoffset == 0) && (ballPtr[s]->txt_zoffset == 0) ) { // track the ball
					glColor3f ( 0.5, 0.5, 0.5);
					glDisable		( GL_LIGHTING ) ;

					if (curvePtr[curv]->kind == STATIC) {
						drawText3D(curvePtr[curv]->polyline[pos] + ballPtr[s]->size, curvePtr[curv]->polyline[pos+1] + ballPtr[s]->size, curvePtr[curv]->polyline[pos+2] + ballPtr[s]->size , label) ; 
					}
					else if (curvePtr[curv]->kind == DYNAMIC) {
						drawText3D( ballPtr[s]->size, curvePtr[curv]->polyline[pos+1] + ballPtr[s]->size, curvePtr[curv]->polyline[pos+2] + ballPtr[s]->size , label) ; 
					}
					glEnable		( GL_LIGHTING ) ;

			}
			else {
					glColor3f ( 0.5, 0.5, 0.5);
					glDisable		( GL_LIGHTING ) ;
				drawText3D(ballPtr[s]->txt_xoffset, ballPtr[s]->txt_yoffset, ballPtr[s]->txt_zoffset, label) ; // output at preset position
					glEnable		( GL_LIGHTING ) ;

				if ( ballPtr[s]->visible == GLUT_VISIBLE) { //  show if ball-visible is specifically set

				glPushMatrix ( ) ; 
					glTranslated 	(ballPtr[s]->txt_xoffset, ballPtr[s]->txt_yoffset, ballPtr[s]->txt_zoffset ) ;
					glColor3f ( ballPtr[s]->colour_red, ballPtr[s]->colour_green, ballPtr[s]->colour_blue);
					switch (ballPtr[s]->type) {
						case 's' :
							glutSolidSphere ((GLdouble) ballPtr[s]->size , 50, 50) ; // ball
						break;
						case 't' :
							glutSolidTeapot ((GLdouble) ballPtr[s]->size); //teaPot
						break;
						case 'b' :
							glutSolidCube ((GLdouble) ballPtr[s]->size);  //cube
						break;
						case 'c' :
							glutSolidCone  ((GLdouble) (ballPtr[s]->size)/2 , (GLdouble) ballPtr[s]->size, 50, 50) ;
						break;
						case 'd' :
							glutSolidTorus ((GLdouble) (ballPtr[s]->size)/2 , (GLdouble) ballPtr[s]->size, 50, 50) ;
						break;
					}
				glPopMatrix ( ) ;
				}
			}
				
		
				
			if ( ballPtr[s]->visible == GLUT_VISIBLE) { //  show if associated curve is visible or ball-visible has been specifically set
		
				glPushMatrix ( ) ; 
					if (curvePtr[curv]->kind == STATIC) glTranslated 	(curvePtr[curv]->polyline[pos],  curvePtr[curv]->polyline[pos+1], curvePtr[curv]->polyline[pos+2] ) ;
					else if (curvePtr[curv]->kind == DYNAMIC) glTranslated 	(0,  curvePtr[curv]->polyline[pos+1], curvePtr[curv]->polyline[pos+2] ) ;
					glColor3f ( ballPtr[s]->colour_red, ballPtr[s]->colour_green, ballPtr[s]->colour_blue);
					switch (ballPtr[s]->type) {
						case 's' :
							glutSolidSphere ((GLdouble) ballPtr[s]->size , 50, 50) ; // ball
						break;
						case 't' :
							glutSolidTeapot ((GLdouble) ballPtr[s]->size); //teaPot
						break;
						case 'b' :
							glutSolidCube ((GLdouble) ballPtr[s]->size);  //cube
						break;
						case 'c' :
							glutSolidCone  ((GLdouble) (ballPtr[s]->size)/2 , (GLdouble) ballPtr[s]->size, 50, 50) ;
						break;
						case 'd' :
							glutSolidTorus ((GLdouble) (ballPtr[s]->size)/2 , (GLdouble) ballPtr[s]->size, 50, 50) ;
						break;
					}  
			   glPopMatrix ( ) ;
			} //end if ball visible
		} // end if graph visible
		s++;
	} // while


}

/* ========================================================================= */  
/* calculate_polygon_vertex_normal.					     */
/* ========================================================================= */
/*
** As it says.
*/

void
calculate_polygon_vertex_normal ( SURF **surfacePtr, int Num_surface ) 
{
  long i,j,k ;
  float p1[3], p2[3], p3[3] ;
  float norm[3] ;
  int p;

for (p=0; p < Num_surface; p++) {
	if (surfacePtr[p]->visible == GLUT_VISIBLE) {
		  for (i=0; i<surfacePtr[p]->npolygons; i++ )
			{
			  for (j=0; j<3; j++) 
			p1[j] = surfacePtr[p]->polygon[i]->vertex[0]->xyz[j] ;
			  for (j=0; j<3; j++) 
			p2[j] = surfacePtr[p]->polygon[i]->vertex[1]->xyz[j] ;
			  for (j=0; j<3; j++) 
			p3[j] = surfacePtr[p]->polygon[i]->vertex[2]->xyz[j] ;

			  calc_normal ( p1, p2, p3, norm ) ;

			  for (j=0; j<3; j++) 
			for (k=0; k<3; k++)
			  surfacePtr[p]->polygon[i]->vertex[j]->norm[k] = norm[k] ;
			}
		}
	}
}

/* ========================================================================= */  
/* calc_normal.					     			     */
/* ========================================================================= */
/*
** Calculates the normal vector from three vertices.
*/
void
calc_normal ( float *p1, float *p2, float *p3, float *norm )
{
  float coa, cob, coc ;
  float px1, py1, pz1 ;
  float px2, py2, pz2 ;
  float px3, py3, pz3 ;
  
  float absvec ;
  
  px1 = p1[0] ;
  py1 = p1[1] ;
  pz1 = p1[2] ;
  
  px2 = p2[0] ;
  py2 = p2[1] ;
  pz2 = p2[2] ;
  
  px3 = p3[0] ;
  py3 = p3[1] ;
  pz3 = p3[2] ;
  
  coa = -(py1 * (pz2-pz3) + py2*(pz3-pz1) + py3*(pz1-pz2)) ;
  cob = -(pz1 * (px2-px3) + pz2*(px3-px1) + pz3*(px1-px2)) ;
  coc = -(px1 * (py2-py3) + px2*(py3-py1) + px3*(py1-py2)) ;
  
  absvec = sqrt ((double) ((coa*coa) + (cob*cob) + (coc*coc))) ;
  
  norm[0] = coa/absvec ;
  norm[1] = cob/absvec ;
  norm[2] = coc/absvec ;
}
