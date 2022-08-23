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


/*  bringing the spod graphics closer to pertecs 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <GLUT/glut.h>
#include <unistd.h>


#include "pertecs.h"


float Glowr = 1.0, Glowg = 1.0, Glowb = 0.0, ScaleIntensity = 1.0;
float background_r = 0.0, background_g = 0.0, background_b = 0.0, complement = 0.0;
float limit = LIMIT;
float  sleepwake , sleep21 , sleepREM , sleepREMslope , awake, scale=0.5;
float start_scan = -1.0 , width_scan = 0.01, scan_incr = 0.0 ;
float xorig = DEF_ORG, yorig = DEF_ORG, zorig = DEF_ORG, xlimit = DEF_X, ylimit = DEF_Y , zlimit = DEF_Z, zaxis = DEF_ORG, yaxis = DEF_ORG, xaxis = DEF_ORG;
int center_axis_flag = TRUE, axis_markers_flag = FALSE, lock_axis_flag = FALSE, lock_axis_light_flag = FALSE, lock_surface_light_flag = FALSE, lock_markers_light_flag = FALSE;
float grid_r = 1.0, grid_g = 1.0, grid_b =0.0, xlinlog = 0.0, ylinlog = 0.0, zlinlog = 0.0; // grid colour
void print_mesg(void);

int DRAW_MODE = 2 ;

SURF *surfacePtr[NUM_SURF] ; // maximum number of surfaces
LINE *curvePtr[NUM_GRAPH] ; // maximum number of graphs
BALL *ballPtr[NUM_BALLS] ; // maximum no of balls
TXTLINE *textPtr[NUM_TXTLINES] ;  // maximum no of text lines
JUMP *jumpPtr[JUMP_SIZE];  // number of views

int maxJumpcntr=-1, Jumpcntr=-1;
int Num_curve = 0, Num_surface = 0, sleepStateFlag = 0, Num_texts = 0;
int	p = -1; // initialised to empty value for traces
int pdiv = -1; // counts number of curves loaded

int Showlabels = 0, fullscreen = 0, simultrace = 0, flat = 0;
float Tracelight = 10.0;
float   trace = 0.0, tracex = -1.0, toggletrace = 0.0; 

float spinxlight = 0 ;
float spinylight = 0 ;
float spinxlight2 = 0 ;
float spinylight2 = 0 ;
float spinxlight3 = 0 ;
float spinylight3 = 0 ;
float spinxsurface = 0 ;
float spinysurface = 0 ;

float surface_red = 0 ;
float surface_green = 0 ;
float surface_blue = 0 ;
float surface_intsty = 0 ;
float surface_shadow = 0 ;


float delay = 0.0, duration = -1, cycles  = 0;
int	realtime_rate = 0, usleep_time = 0; 
GLdouble lastseconds;

int newtitle = FALSE;
char title[TITLE_SIZE]; // any new title limited to TITLE_SIZE chars
int winSize_x , winSize_y  ; // default window size 
int winPos_x , winPos_y ; // default window position left to system to determine

int helpmenu = FALSE, textdisp = FALSE;
int	xtextstart = 0, ytextstart = 0;



/* ========================================================================= */
/* timing	                                                             */
/* ========================================================================= */  
/*
** returns number of lapsed seconds 
*/

GLdouble get_secs(void)
{
  return glutGet(GLUT_ELAPSED_TIME) / 1000.0;
}



/* ========================================================================= */
/* setview  orients axis to given angle	                                                             */
/* ========================================================================= */  
/*
** Rotate the surface and light about.
*/

void setview(int x_orientation, int y_orientation) {
	if ((x_orientation > -360) && (x_orientation < 360)) spinxsurface = (GLdouble) x_orientation; 
	else spinxsurface = 0;
	if ((y_orientation > -360) && (y_orientation < 360)) spinysurface = (GLdouble) y_orientation; 
	else spinysurface = 0;
}

void setlight_obj(int x_orientation, int y_orientation) {
	if ((x_orientation > -360) && (x_orientation < 360)) spinxlight = (GLdouble) x_orientation; 
	else spinxlight = 0;
	if ((y_orientation > -360) && (y_orientation < 360)) spinylight = (GLdouble) y_orientation; 
	else spinylight = 0;

}

void setlight_2(int x_orientation, int y_orientation) {
	if ((x_orientation > -360) && (x_orientation < 360)) spinxlight2 = (GLdouble) x_orientation; 
	else spinxlight2 = 0;
	if ((y_orientation > -360) && (y_orientation < 360)) spinylight2 = (GLdouble) y_orientation; 
	else spinylight2 = 0;
}

void setlight_3(int x_orientation, int y_orientation) {
	if ((x_orientation > -360) && (x_orientation < 360)) spinxlight3 = (GLdouble) x_orientation; 
	else spinxlight3 = 0;
	if ((y_orientation > -360) && (y_orientation < 360)) spinylight3 = (GLdouble) y_orientation; 
	else spinylight3 = 0;
}

/* ========================================================================= */
/* motion	                                                                 */
/* ========================================================================= */  
/*
** Rotate the surface and light about.
*/

int movestart = 0, rotate = 0, movelight = 0, moveangle = 0, origx, origy; 
GLdouble angle = 8.0;

void motion ( int x, int y )
{
  if (rotate) {
	movestart = movelight = 0;
	spinysurface = ( spinysurface + (GLdouble) (x - origx)/8.0 ) ;
		while (spinysurface > 360.0) spinysurface -= 360.0 ;
		while (spinysurface < 0.0) spinysurface += 360.0 ;
	spinxsurface = ( spinxsurface + (GLdouble) (y - origy)/4.0 ) ;
		while (spinxsurface > 360.0) spinxsurface -= 360.0 ;
		while (spinxsurface < 0.0) spinxsurface += 360.0 ;
	origx = x;
	origy = y;
	glutPostRedisplay();
  }
  if (movestart) {
	rotate = movelight = 0;
//	start_scan = start_scan + (float) (x - origx )/600;
	xtextstart -= (int) (x - origx ) ;
	ytextstart += (int) (y - origy ) ;
//	if (start_scan + width_scan > 1.0) start_scan = 1.0 - width_scan;
//	else if (start_scan < -1.0) start_scan = -1.0;
	origx = x;
    origy = y;
    glutPostRedisplay();
  }
  if (movelight) {
	rotate = movestart = 0;
	switch (movelight) {	
	  case 1:
	  		spinylight = ( spinylight + (GLdouble) (x - origx ) ) ;
				while (spinylight > 360.0) spinylight -= 360.0 ;
				while (spinylight < 0.0) spinylight += 360.0 ;
			spinxlight = ( spinxlight + (GLdouble) (y - origy ) ) ;
				while (spinxlight > 360.0) spinxlight -= 360.0 ;
				while (spinxlight < 0.0) spinxlight += 360.0 ;
		break;
	  case 2:
			spinylight2 = ( spinylight2 + (GLdouble) (x - origx ) ) ;
				while (spinylight2 > 360.0) spinylight2 -= 360.0 ;
				while (spinylight2 < 0.0) spinylight2 += 360.0 ;
			spinxlight2 = ( spinxlight2 + (GLdouble) (y - origy ) ) ;
				while (spinxlight2 > 360.0) spinxlight2 -= 360.0 ;
				while (spinxlight2 < 0.0) spinxlight2 += 360.0 ;
		break;
	  case 3:
			spinylight3 = ( spinylight3 + (GLdouble) (x - origx ) ) ;
				while (spinylight3 > 360.0) spinylight3 -= 360.0 ;
				while (spinylight3 < 0.0) spinylight3 += 360.0 ;
			spinxlight3 = ( spinxlight3 + (GLdouble) (y - origy ) ) ;
				while (spinxlight3 > 360.0) spinxlight3 -= 360.0 ;
				while (spinxlight3 < 0.0) spinxlight3 += 360.0 ;
		break;
	}
    origx = x;
    origy = y;
    glutPostRedisplay();
  }
 
}

/* ========================================================================= */
/* real_scope	                                                                 */
/* ========================================================================= */  
/*
** real-time scope animation
*/


void
 real_scope(void)
 {
 	GLdouble seconds;
 		if ( !realtime_rate ) glutIdleFunc(NULL);
// this was using the spod update to drive the timing... now want to do this in the main loop using the 
//		seconds = get_secs();
//		cycles += (float) ((seconds - lastseconds) *  realtime_rate) ;
//		lastseconds = seconds;

// this was for realtime replay of spod files
//	if ( duration < 0 ) glutIdleFunc(NULL); 
//	else {
//		start_scan = (2*frame.time / duration - width_scan -.01)-1.0;  // fix this when we have the time loaded
//		if (start_scan < -1.0) start_scan = -1.0; // this happens only at the begining of the session
//	}
	glutPostRedisplay();
	if (pertecs_data.thread_status == QUIT) glutIdleFunc(NULL); //exit (0);

 }


/* ========================================================================= */
/* roll	                                                                 */
/* ========================================================================= */  
/*
** roll object.
*/

void
 roll(void)
 {
    spinysurface = ( spinysurface + (GLdouble) trace ) ;
		while (spinysurface > 360.0) spinysurface -= 360.0 ;
		while (spinysurface < 0.0) spinysurface += 360.0 ;
//		if (start_scan + width_scan >= 1) { scan_incr = -scan_incr; }
//		else if (start_scan <= -1) { scan_incr = -scan_incr; }
   
   real_scope();
 }

/* ========================================================================= */
/* timed_scope	                                                                 */
/* ========================================================================= */  
/*
** real-time replay  animation
*/
void
 timed_scope(void)
 {
 	GLdouble seconds;

	if ( duration < 0 ) glutIdleFunc(NULL);
	else {
		seconds = get_secs();
		if (seconds < delay)  start_scan = -1.0;
		else start_scan = -1.0 - width_scan/2 + 2*(seconds-delay)/duration;
		if (start_scan + width_scan > 1) { start_scan = 1.0 - width_scan ; scan_incr = 0.0; glutIdleFunc(NULL); }
		else if (start_scan < -1) { start_scan = -1.0;  }
	}
	glutPostRedisplay();
	
 }



/* ========================================================================= */
/* scope	                                                                 */
/* ========================================================================= */  
/*
** variable-rate scope animation
*/

void
 scope(void)
 {
	if ( start_scan += scan_incr) {
		if (start_scan + width_scan > 1) { start_scan = 1.0 - width_scan ; scan_incr = 0.0; glutIdleFunc(NULL); }
		else if (start_scan < -1) { start_scan = -1.0; scan_incr = 0.0;  glutIdleFunc(NULL); }
	}
	else glutIdleFunc(NULL);
	glutPostRedisplay();
	
 }



/* ========================================================================= */
/* roll & scope	                                                             */
/* ========================================================================= */  
/*
** both roll and scope actions together
*/


void
 both(void)
 {
    spinysurface = ( spinysurface + (GLdouble) trace ) ;
		while (spinysurface > 360.0) spinysurface -= 360.0 ;
		while (spinysurface < 0.0) spinysurface += 360.0 ;
    spinxsurface = ( spinxsurface + (GLdouble) tracex ) ;
		while (spinxsurface > 360.0) spinxsurface -= 360.0 ;
		while (spinxsurface < 0.0) spinxsurface += 360.0 ;
//	if ( start_scan += scan_incr) {
//		if (start_scan + width_scan > 1) { start_scan = 1.0 - width_scan; scan_incr = 0.0; glutIdleFunc(NULL); }
//		else if (start_scan < -1) { start_scan = -1.0; scan_incr = 0.0;  glutIdleFunc(NULL); }
//	}
//	else glutIdleFunc(NULL);
	 real_scope();	
 }


/* ========================================================================= */
/* mouse action	                                                                 */
/* ========================================================================= */  
/*
** 
*/


void
mouse(int button, int state, int x, int y)
{
  switch(button) {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN) {
        origx = x;
        origy = y;
	    if (GLUT_ACTIVE_SHIFT == glutGetModifiers()) {
	    	if (lock_axis_light_flag == FALSE) movelight = 3; else movelight = 0;
	    }
        else {
        	movelight = 0;
      		if (lock_axis_flag == FALSE) rotate = 1; else rotate = 0;
      	}
    } else {
      rotate = 0; moveangle = 0;
    }
    break;
  case GLUT_MIDDLE_BUTTON: // option-
    if (state == GLUT_DOWN) {
        origx = x;
        origy = y;
	    if (GLUT_ACTIVE_SHIFT == glutGetModifiers()) {
	    	if (lock_markers_light_flag == FALSE) movelight = 2; else movelight = 0;	// need to think about this
	    }
        else {
        	if (lock_markers_light_flag == FALSE)  movelight = 1; else movelight = 0;	// need to add the lock switch to this
        }
    } else {
      movelight = 0; moveangle = 0;
    }
    break;
  case GLUT_RIGHT_BUTTON: // ctrl-
    if (state == GLUT_DOWN) {
      origx = x;
      origy = y;
      if (GLUT_ACTIVE_SHIFT != glutGetModifiers()) { 
      		movestart = 1;
      	}
		else {
			if (lock_surface_light_flag == FALSE) moveangle = 1;  else moveangle = 0; 		// need to check which switch we want on this
		}
	} else {
		movestart = 0; moveangle = 0;
	}
    break;
  }
}




/* ========================================================================= */
/* myinit		                                                     		 */
/* ========================================================================= */  
/*
** Do the lighting thing.
*/

void myinit ( void )
{
	int i;
	glEnable    ( GL_LIGHTING   ) ;
	glEnable    ( GL_LIGHT0     ) ;
	glDepthFunc ( GL_LEQUAL     ) ;
	glEnable    ( GL_DEPTH_TEST ) ;
	glColorMaterial ( GL_FRONT_AND_BACK , GL_AMBIENT_AND_DIFFUSE ); // GL_EMISSION ); 

   glEnable (GL_LINE_SMOOTH);
   glEnable (GL_BLEND);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);




//	glEnable(GL_BLEND);
//	glCullFace(GL_FRONT );
//	glEnable(GL_CULL_FACE); 
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable    ( GL_COLOR_MATERIAL );

	if (maxJumpcntr == -1) { // if no jump states loaded, it sets the load state to the default jump state
		Jumpcntr = 0; maxJumpcntr = 0;
		jumpPtr[Jumpcntr] = (JUMP *) malloc(sizeof(JUMP));
				 
		jumpPtr[Jumpcntr]->spinxsurface = spinxsurface;
		jumpPtr[Jumpcntr]->spinysurface = spinysurface;
		jumpPtr[Jumpcntr]->spinxlight = spinxlight;
		jumpPtr[Jumpcntr]->spinylight = spinylight;
		jumpPtr[Jumpcntr]->spinxlight2 = spinxlight2;
		jumpPtr[Jumpcntr]->spinylight2 = spinylight2;
		jumpPtr[Jumpcntr]->spinxlight3 = spinxlight3;
		jumpPtr[Jumpcntr]->spinylight3 = spinylight3;
		
		jumpPtr[Jumpcntr]->Glowr = Glowr;
		jumpPtr[Jumpcntr]->Glowg = Glowg;
		jumpPtr[Jumpcntr]->Glowb = Glowb;
		jumpPtr[Jumpcntr]->ScaleIntensity = ScaleIntensity;
		jumpPtr[Jumpcntr]->background_r = background_r;
		jumpPtr[Jumpcntr]->background_g = background_g;
		jumpPtr[Jumpcntr]->background_b = background_b;
		
		
		jumpPtr[Jumpcntr]->start_scan = start_scan;
		jumpPtr[Jumpcntr]->scan_incr = scan_incr;
		jumpPtr[Jumpcntr]->width_scan = width_scan;
		jumpPtr[Jumpcntr]->trace = trace;
		jumpPtr[Jumpcntr]->tracex = tracex;
		
		jumpPtr[Jumpcntr]->xaxis = xaxis;
		jumpPtr[Jumpcntr]->yaxis = yaxis;
		jumpPtr[Jumpcntr]->zaxis = zaxis;
		jumpPtr[Jumpcntr]->xlimit = xlimit;
		jumpPtr[Jumpcntr]->ylimit = ylimit;
		jumpPtr[Jumpcntr]->zlimit = zlimit;
		jumpPtr[Jumpcntr]->xorig = xorig;
		jumpPtr[Jumpcntr]->yorig = yorig;
		jumpPtr[Jumpcntr]->zorig = zorig;
		jumpPtr[Jumpcntr]->grid_r = grid_r;
		jumpPtr[Jumpcntr]->grid_g = grid_g;
		jumpPtr[Jumpcntr]->grid_b = grid_b;
		
		jumpPtr[Jumpcntr]->simultrace = simultrace;
		jumpPtr[Jumpcntr]->flat = flat;
		jumpPtr[Jumpcntr]->Showlabels = Showlabels;
		jumpPtr[Jumpcntr]->Tracelight = Tracelight;
		
		jumpPtr[Jumpcntr]->awake = awake;
		jumpPtr[Jumpcntr]->sleepwake = sleepwake;
		jumpPtr[Jumpcntr]->sleep21 = sleep21;
		jumpPtr[Jumpcntr]->sleepREM = sleepREM;
		
		for (i=0; i < 10; i++) { // only look at the first 10 objects
			if ((surfacePtr[i]) && (surfacePtr[i]->npolylinenodes)) {
				jumpPtr[Jumpcntr]->object[i] = i+1;
				jumpPtr[Jumpcntr]->visibility[i] = surfacePtr[i]->visible ;
			}
			else jumpPtr[Jumpcntr]->object[i] = 0;

			if (curvePtr[i]) {
				jumpPtr[Jumpcntr]->object[i+10] = i+1;
				jumpPtr[Jumpcntr]->visibility[i+10] = curvePtr[i]->visible ;
			}
			else jumpPtr[Jumpcntr]->object[i+10] = 0;
		} 

	}
}



/* ========================================================================= */
/* display                                                                   */
/* ========================================================================= */  
/*
** Here's were all the display action takes place.
*/

void display ( void )
{
  GLfloat position [] = { 30.0, 70.0, 10.0, 1.0 }  ;			// position of light ?

    

  glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;

  glPushMatrix ( ) ; 
  
	glTranslatef ( 0.0, 0.0, -90.0 ) ;							// position of viewer ? 
    glRotated	( (GLdouble) spinxsurface, 1.0, 0.0, 0.0 ) ;
    glRotated	( (GLdouble) spinysurface, 0.0, 1.0, 0.0 ) ;
  
    glPushMatrix ( ) ; 														// light source 1
      glRotated		( (GLdouble) spinxlight, 1.0, 0.0, 0.0 ) ;
      glRotated		( (GLdouble) spinylight, 0.0, 1.0, 0.0 ) ;
      glLightfv		( GL_LIGHT0, GL_POSITION, position ) ;
   glPopMatrix ( ) ;


  paint_curves ( curvePtr, Num_curve ) ;	

//*  paint_polygons ( surfacePtr, Num_surface, DRAW_MODE, (int) spinxsurface, (int)  spinysurface) ;	


   glPushMatrix ( ) ; 														// light source 2
      glRotated		( (GLdouble) spinxlight2, 1.0, 0.0, 0.0 ) ;
      glRotated		( (GLdouble) spinylight2, 0.0, 1.0, 0.0 ) ;
      glLightfv		( GL_LIGHT0, GL_POSITION, position ) ;
   glPopMatrix ( ) ; 

//*  paint_ball ( curvePtr, ballPtr) ;	

   glPushMatrix ( ) ;  														// light source 3
      glRotated		( (GLdouble) spinxlight3, 1.0, 0.0, 0.0 ) ;
      glRotated		( (GLdouble) spinylight3, 0.0, 1.0, 0.0 ) ;
      glLightfv		( GL_LIGHT0, GL_POSITION, position ) ;
   glPopMatrix ( ) ; 
   
  draw_axis ( zaxis, yaxis, xaxis, grid_r, grid_g, grid_b, center_axis_flag, axis_markers_flag  );	

 
 
//*  screentext (curvePtr, Num_curve, textPtr, Num_texts);
   
  glPopMatrix();

  glutSwapBuffers();
}





/* ========================================================================= */
/* myReshape		                                                     */
/* ========================================================================= */  
/*
** What to do if the window is modified.
*/

void myReshape ( int w, int h )
{
  glViewport	( 0,0,w,h ) ;
  glMatrixMode  ( GL_PROJECTION ) ;
  glLoadIdentity( ) ;
//  gluPerspective( 8.0, (GLfloat) w/(GLfloat) h, 1.0, 200.0 ) ;
  gluPerspective( angle, (GLfloat) w/(GLfloat) h, 1.0, 200.0 ) ;
  glMatrixMode  ( GL_MODELVIEW ) ;
}


/* ========================================================================= */
/* error_exit	                                                             */
/* ========================================================================= */  
/*
** Problems!
*/

void error_exit( char *error_message )
{
    fprintf ( stderr, "%s\n", error_message ) ; fflush(stderr);
    exit( 1 ) ;
}

/* ========================================================================= */
/* Key			                                                     */
/* ========================================================================= */
/*
** Actions on a key press.
*/

// static int m = 0, e = 0;

/* ARGSUSED1 */
static void Key ( unsigned char key, int x, int y )
{
	int i, rc;
	
    switch ( key ) {
      case 27 :

	exit (0) ;

//      case 'r' :
//      case 'R' :
//	  fprintf(stderr, "Restore surface %ld \n", sizeof ( POLYGON ));
//		Glowr = 1.0 ; Glowg = 1.0 ; Glowb = 0.0 ;
//		glutPostRedisplay();
//	break;

    case '0' : // toggle surface
    case '9' : 	
    case '8' : 	
    case '7' :
		if ((int) key == (int) '0') i = 0;
		else i =  (int) '9' + 1 - (int) key ;	
		if ((surfacePtr[i]) && (surfacePtr[i]->npolylinenodes)) {
			if ((surfacePtr[i])->visible == GLUT_NOT_VISIBLE ) (surfacePtr[i])->visible = GLUT_VISIBLE ;
			else (surfacePtr[i])->visible = GLUT_NOT_VISIBLE ;
		}
		else {
			i = (int) key - (int) '1' ;
			if (i>=6) {
				if (curvePtr[i]) {
					if ((curvePtr[i])->visible == GLUT_NOT_VISIBLE ) (curvePtr[i])->visible = GLUT_VISIBLE ;
					else (curvePtr[i])->visible = GLUT_NOT_VISIBLE ;
				}
			}
		}
		glutPostRedisplay();
		break;
    case '1' : // toggle point/line objects
    case '2' : //
    case '3' : //
    case '4' : //
    case '5' : //
    case '6' : //
		i = (int) key - (int) '1' ;
		if (curvePtr[i]) {
			if ((curvePtr[i])->visible == GLUT_NOT_VISIBLE ) (curvePtr[i])->visible = GLUT_VISIBLE ;
			else (curvePtr[i])->visible = GLUT_NOT_VISIBLE ;
		}
		glutPostRedisplay();
		break;

      case 'p' : // toggle background colour
		background_r = 1.0 - background_r;
		background_g = 1.0 - background_g;
		background_b = 1.0 - background_b;
		glClearColor( background_r, background_g, background_b, 1.0);
		glutPostRedisplay();
	break;
      case 'P' : // toggle background colour
		complement = 1.0 - complement;
		glutPostRedisplay();
	break;

/*
      case 'l' : // show labels
		if (Showlabels) Showlabels = 0;
		else Showlabels = 1;
		glutPostRedisplay();
	break;
      case 'L' : // allow/disallow Light control of graphs and traces
		if (Tracelight==1.0 ) Tracelight = 10.0;
		else Tracelight = 1.0;
		glutPostRedisplay();
	break;
*/

      case 'I' : // trace/simul
      	helpmenu = FALSE;
      	
		if (textdisp == TRUE) textdisp = FALSE;
		else textdisp = TRUE;

	break;
	
	
      case 'j' : // jump
      break;												// disble til fixed
		Jumpcntr++;
		if (Jumpcntr > maxJumpcntr) Jumpcntr = 0;			

		spinxsurface = jumpPtr[Jumpcntr]->spinxsurface;
		spinysurface = jumpPtr[Jumpcntr]->spinysurface;
		
		spinxlight = jumpPtr[Jumpcntr]->spinxlight;
		spinylight = jumpPtr[Jumpcntr]->spinylight;
		spinxlight2 = jumpPtr[Jumpcntr]->spinxlight2;
		spinylight2 = jumpPtr[Jumpcntr]->spinylight2;
		spinxlight3 = jumpPtr[Jumpcntr]->spinxlight3;
		spinylight3 = jumpPtr[Jumpcntr]->spinylight3;

		
		Glowr = jumpPtr[Jumpcntr]->Glowr;
		Glowg = jumpPtr[Jumpcntr]->Glowg;
		Glowb = jumpPtr[Jumpcntr]->Glowb;
		ScaleIntensity = jumpPtr[Jumpcntr]->ScaleIntensity;
		background_r = jumpPtr[Jumpcntr]->background_r;
		background_g = jumpPtr[Jumpcntr]->background_g;
		background_b = jumpPtr[Jumpcntr]->background_b;

		start_scan = jumpPtr[Jumpcntr]->start_scan;
		scan_incr = jumpPtr[Jumpcntr]->scan_incr;
		width_scan = jumpPtr[Jumpcntr]->width_scan;
		trace = jumpPtr[Jumpcntr]->trace;
		tracex = jumpPtr[Jumpcntr]->tracex;
		
	  	xaxis = jumpPtr[Jumpcntr]->xaxis;     
		yaxis = jumpPtr[Jumpcntr]->yaxis;     
		zaxis  = jumpPtr[Jumpcntr]->zaxis;     
		xlimit = jumpPtr[Jumpcntr]->xlimit;     
		ylimit = jumpPtr[Jumpcntr]->ylimit;     
		zlimit = jumpPtr[Jumpcntr]->zlimit;     
		xorig  = jumpPtr[Jumpcntr]->xorig;     
		yorig  = jumpPtr[Jumpcntr]->yorig;     
		zorig  = jumpPtr[Jumpcntr]->zorig;     
		grid_r = jumpPtr[Jumpcntr]->grid_r;     
		grid_g = jumpPtr[Jumpcntr]->grid_g;     
		grid_b = jumpPtr[Jumpcntr]->grid_b;     

		simultrace = jumpPtr[Jumpcntr]->simultrace;     
		flat = 	jumpPtr[Jumpcntr]->flat;  
		Showlabels = jumpPtr[Jumpcntr]->Showlabels;     
		Tracelight = jumpPtr[Jumpcntr]->Tracelight;
       
		awake = jumpPtr[Jumpcntr]->awake;     
		sleepwake = jumpPtr[Jumpcntr]->sleepwake;     
		sleep21 = jumpPtr[Jumpcntr]->sleep21;     
		sleepREM = jumpPtr[Jumpcntr]->sleepREM;
		
		for (i=0; i < 10; i++) { // only look at first ten objects
			if (jumpPtr[Jumpcntr]->object[i]) {
				if ((surfacePtr[jumpPtr[Jumpcntr]->object[i] - 1]) && (surfacePtr[jumpPtr[Jumpcntr]->object[i] - 1]->npolylinenodes)) 
					surfacePtr[jumpPtr[Jumpcntr]->object[i] - 1]->visible =  jumpPtr[Jumpcntr]->visibility[i];
			}
			if (jumpPtr[Jumpcntr]->object[i+10]) {
				if (curvePtr[jumpPtr[Jumpcntr]->object[i+10] - 1]) 
					curvePtr[jumpPtr[Jumpcntr]->object[i+10] - 1]->visible = jumpPtr[Jumpcntr]->visibility[i+10];
			}
		} 

/*
		glutPostRedisplay();
	break;
      case 'J' : // jump
      	break;												// disble til fixed
		Jumpcntr--;
		if (Jumpcntr < 0) Jumpcntr = maxJumpcntr;			

		spinxsurface = jumpPtr[Jumpcntr]->spinxsurface;
		spinysurface = jumpPtr[Jumpcntr]->spinysurface;
		
		spinxlight = jumpPtr[Jumpcntr]->spinxlight;
		spinylight = jumpPtr[Jumpcntr]->spinylight;
		spinxlight2 = jumpPtr[Jumpcntr]->spinxlight2;
		spinylight2 = jumpPtr[Jumpcntr]->spinylight2;
		spinxlight3 = jumpPtr[Jumpcntr]->spinxlight3;
		spinylight3 = jumpPtr[Jumpcntr]->spinylight3;

		
		Glowr = jumpPtr[Jumpcntr]->Glowr;
		Glowg = jumpPtr[Jumpcntr]->Glowg;
		Glowb = jumpPtr[Jumpcntr]->Glowb;
		ScaleIntensity = jumpPtr[Jumpcntr]->ScaleIntensity;
		background_r = jumpPtr[Jumpcntr]->background_r;
		background_g = jumpPtr[Jumpcntr]->background_g;
		background_b = jumpPtr[Jumpcntr]->background_b;

		start_scan = jumpPtr[Jumpcntr]->start_scan;
		scan_incr = jumpPtr[Jumpcntr]->scan_incr;
		width_scan = jumpPtr[Jumpcntr]->width_scan;
		trace = jumpPtr[Jumpcntr]->trace;
		tracex = jumpPtr[Jumpcntr]->tracex;
		
	  	xaxis = jumpPtr[Jumpcntr]->xaxis;     
		yaxis = jumpPtr[Jumpcntr]->yaxis;     
		zaxis  = jumpPtr[Jumpcntr]->zaxis;     
		xlimit = jumpPtr[Jumpcntr]->xlimit;     
		ylimit = jumpPtr[Jumpcntr]->ylimit;     
		zlimit = jumpPtr[Jumpcntr]->zlimit;     
		xorig  = jumpPtr[Jumpcntr]->xorig;     
		yorig  = jumpPtr[Jumpcntr]->yorig;     
		zorig  = jumpPtr[Jumpcntr]->zorig;     
		grid_r = jumpPtr[Jumpcntr]->grid_r;     
		grid_g = jumpPtr[Jumpcntr]->grid_g;     
		grid_b = jumpPtr[Jumpcntr]->grid_b;     

		simultrace = jumpPtr[Jumpcntr]->simultrace;     
		flat = 	jumpPtr[Jumpcntr]->flat;  
		Showlabels = jumpPtr[Jumpcntr]->Showlabels;     
		Tracelight = jumpPtr[Jumpcntr]->Tracelight;
       
		awake = jumpPtr[Jumpcntr]->awake;     
		sleepwake = jumpPtr[Jumpcntr]->sleepwake;     
		sleep21 = jumpPtr[Jumpcntr]->sleep21;     
		sleepREM = jumpPtr[Jumpcntr]->sleepREM;     

		for (i=0; i < 10; i++) { // only look at frst ten objects
			if (jumpPtr[Jumpcntr]->object[i]) {
				if ((surfacePtr[jumpPtr[Jumpcntr]->object[i] - 1]) && (surfacePtr[jumpPtr[Jumpcntr]->object[i] - 1]->npolylinenodes)) 
					surfacePtr[jumpPtr[Jumpcntr]->object[i] - 1]->visible =  jumpPtr[Jumpcntr]->visibility[i];
			}
			if (jumpPtr[Jumpcntr]->object[i+10]) {
				if (curvePtr[jumpPtr[Jumpcntr]->object[i+10] - 1]) 
					curvePtr[jumpPtr[Jumpcntr]->object[i+10] - 1]->visible = jumpPtr[Jumpcntr]->visibility[i+10];
			}
		} 

		glutPostRedisplay();
	break;


      case 'i' : // depth
		 flat++; if (flat >=3) {flat = 0;}
		glutPostRedisplay();
	break;


      case 'g' : // scope 
		if (width_scan < .0025) { // expanded scale
			if (scan_incr > .00001) { scan_incr = .000009; glutIdleFunc(scope);}
			else if (scan_incr < -.00001) {scan_incr = -.000009; glutIdleFunc(scope);}
			else {
				if (scan_incr += .000001) glutIdleFunc(scope);
				else glutIdleFunc(NULL); 
			}
		}
		else if (scan_incr += .00001) glutIdleFunc(scope); 
		else glutIdleFunc(NULL); 
	break;
      case 'G' : // scope
		if (width_scan < .0025) { // expanded scale
			if (scan_incr > .00001) { scan_incr = .000009; glutIdleFunc(scope);}
			else if (scan_incr < -.00001) {scan_incr = -.000009; glutIdleFunc(scope);}
			else {
				if (scan_incr -= .000001) glutIdleFunc(scope);
				else glutIdleFunc(NULL); 
			}
		}
		else if (scan_incr -= .00001) glutIdleFunc(scope); 
		else  glutIdleFunc(NULL); 
	break;


	  case 'Y' : // timed_scope 
			glutIdleFunc(timed_scope);
	break;	  
      case 'y' : // scope
			glutIdleFunc(NULL); 
	break;


      case 'h' : // scope
      	start_scan += width_scan/2.0;  // shift reference point to middle of screen 
		if (width_scan >= .0025) width_scan += .0025 ;
		else if  (width_scan >= .00025) width_scan += .00025 ;
		else if  (width_scan >= .000025) width_scan += .000025 ;
		start_scan -= width_scan/2.0; // shift back to left hand side of the screen... this keeps the middle stationary in respect of the window changes
		if (start_scan < -1.0) start_scan = -1.0; else if (start_scan + width_scan  > 1.0) start_scan = 1.0 - width_scan;
		glutPostRedisplay();
		break;
      case 'H' : // scope
      	start_scan += width_scan/2.0;
		if (width_scan > .005) width_scan -= .0025 ;
		else if (width_scan > .0005) width_scan -= .00025 ;
		else if (width_scan > .00005) width_scan -= .000025;
		start_scan -= width_scan/2.0; 
		if (start_scan < -1.0) start_scan = -1.0; else if (start_scan + width_scan  > 1.0) start_scan = 1.0 - width_scan;
		glutPostRedisplay();
	break;
*/

      case 't' : // tumble
		toggletrace = 0.0;
		if (lock_axis_flag == FALSE) {
			if (trace += RATESTEP) glutIdleFunc(roll); 
			else glutIdleFunc(NULL); 
		}
		else glutIdleFunc(real_scope);
	break;
      case 'T' : // tumble
		toggletrace = 0.0;
		if (lock_axis_flag == FALSE) {
			if (trace -= RATESTEP) glutIdleFunc(roll); 
			else  glutIdleFunc(NULL); 
		}
		else glutIdleFunc(real_scope);
	break;

      case ' ' : // tumble off
		if (trace == 0.0) trace = toggletrace; 
		else if ((trace  > 0.0) && (toggletrace > 0.0 )) {trace = 0.0;}
		else  {toggletrace = trace;  trace = 0.0;}
		tracex = -1.0;
		glutIdleFunc(real_scope);
	break;
      case 'B' : // both tumble and trace
		if (lock_axis_flag == FALSE) {
			if (tracex < 0) tracex = 0.0;
			else tracex += RATESTEP;
			glutIdleFunc(both); 
		}
		else glutIdleFunc(real_scope);

	break;
	// the screen grid
      case '!' : // increase zaxis brightness
		if (zaxis <= .9) { zaxis+=.1 ; glutPostRedisplay(); }
	break;
      case '#' : // increase yaxis brightness
		if (yaxis <= .9) { yaxis+=.1 ; glutPostRedisplay(); }
	break;
      case '%' : // increase xaxis brightness 
		if (xaxis <= .9 ) { xaxis +=.1 ; glutPostRedisplay(); }
	break;


      case '@' : // decrease zaxis brightness
		if (zaxis > 0.0 ) { zaxis-=.1 ; glutPostRedisplay(); }
	break;
      case '$' : // decrease yaxis brightness
		if (yaxis > 0.0 ) { yaxis-=.1 ; glutPostRedisplay(); }
	break;
      case '^' : // toggle center axis 
		if (xaxis > 0.0 ) { xaxis-=.1 ; glutPostRedisplay(); }
	break;
      case '&' : // toggle center axis 
		if (center_axis_flag == TRUE ) center_axis_flag = FALSE; 
		else center_axis_flag = TRUE ;
		glutPostRedisplay();
	break;
      case '*' : // toggle axis markers/labels 
		if (axis_markers_flag == TRUE ) axis_markers_flag = FALSE; 
		else axis_markers_flag = TRUE;
		glutPostRedisplay();
	break;

	
      case 'O' : 
	  // output screen states
		fprintf(stderr,"\nO: %d:%d\n",(int) spinxsurface, (int) spinysurface);
		fprintf(stderr,"L: %d:%d: %d:%d: %d:%d\n", (int) spinxlight, (int) spinylight, (int) spinxlight2, (int) spinylight2, (int) spinxlight3, (int) spinylight3);
		fprintf(stderr,"C: %g:%g:%g: %g:%g:%g: %g\n",Glowr, Glowg, Glowb, ScaleIntensity, background_r, background_g, background_b);
		fprintf(stderr,"G: %g:%g:%g: %g:%g:%g: %g:%g:%g: %g:%g:%g\n",xaxis, yaxis, zaxis, xlimit, ylimit, zlimit, xorig, yorig, zorig, grid_r, grid_g, grid_b);
		fprintf(stderr,"M: %d:%d:%d: %g\n", simultrace, flat, Showlabels, Tracelight);
		fprintf(stderr,"o: ");
		for (i=0; i < 10; i++) { // only first ten objects
			if ((surfacePtr[i]) && (surfacePtr[i]->npolylinenodes)) fprintf (stderr,"%d:%d ",i+1,(1==surfacePtr[i]->visible) );
			else fprintf (stderr,"0:0 ");
		} 
		for (i=0; i < 10; i++) {
			if (curvePtr[i]) fprintf (stderr,"%d:%d ",i+1,curvePtr[i]->visible );
			else fprintf (stderr,"0:0 ");
		} 
		fprintf(stderr,"\nI: %g:%g:%g:%g:%g\n", start_scan, scan_incr, width_scan, trace, tracex); fflush(stderr);		
	break;
	
      case 'b' :
//      	DRAW_MODE = 0;
		DRAW_MODE++ ;
		if ( DRAW_MODE >= 3 ) DRAW_MODE = 0 ;

	glutPostRedisplay();
	break;

      case '?' :
      	textdisp = FALSE;
      	
		if (helpmenu == TRUE) helpmenu = FALSE;
		else helpmenu = TRUE;

	break;
		
    }
}

void
special(int key, int x, int y)
{

  switch(key) {
  case GLUT_KEY_RIGHT:
   	if (helpmenu || textdisp) {
   		xtextstart += glutBitmapWidth(GLUT_BITMAP_8_BY_13, 'M') ;
   	}
    else if (GLUT_ACTIVE_SHIFT == glutGetModifiers()) {
     	Glowb += 0.1 ; Glowg -= 0.1 ;
    } 
    else {
		Glowr -= 0.05 ; Glowg += 0.05 ;
	}
	glutPostRedisplay();
    break;
  case GLUT_KEY_LEFT:
    if (helpmenu || textdisp) {
   		xtextstart -= glutBitmapWidth(GLUT_BITMAP_8_BY_13, 'M') ;
   	}
    else if (GLUT_ACTIVE_SHIFT == glutGetModifiers()) {
    	Glowb -= 0.1 ; Glowg += 0.1 ;
    } else {
		Glowr += 0.05 ; Glowg -= 0.05 ;
	}
	glutPostRedisplay();
    break;
  case GLUT_KEY_UP:
    if (helpmenu || textdisp) {
   		ytextstart += glutBitmapWidth(GLUT_BITMAP_8_BY_13, 'M') * 2;
   	}
    else if (GLUT_ACTIVE_SHIFT == glutGetModifiers()) {
    	ScaleIntensity += 0.05;
    } 
    else {
		Glowr += 0.1 ; Glowg += 0.1 ;
	}
	glutPostRedisplay();
    break;
  case GLUT_KEY_DOWN:
    if (helpmenu || textdisp) {
   		ytextstart -= glutBitmapWidth(GLUT_BITMAP_8_BY_13, 'M') * 2;
   	}
    else if (GLUT_ACTIVE_SHIFT == glutGetModifiers()) {
    	ScaleIntensity -= 0.05;
    } 
    else {
		Glowr -= 0.1 ; Glowg -= 0.1 ;
	}
	glutPostRedisplay();
    break;
	
  case GLUT_KEY_PAGE_UP:
     if (helpmenu || textdisp) {
   	
   	}
    else 
  		ScaleIntensity += 0.05;
	glutPostRedisplay();
    break;
	
  case GLUT_KEY_PAGE_DOWN:
     if (helpmenu || textdisp) {
   	
   	}
    else 
  		ScaleIntensity -= 0.05;
	glutPostRedisplay();
    break;

  case GLUT_KEY_HOME:
	if (helpmenu || textdisp) {
   		xtextstart = ytextstart = 0;
   	}
    else {
    	Glowb -= 0.1 ; Glowg += 0.1 ;
    }
	glutPostRedisplay();
    break;

  case GLUT_KEY_END:
	Glowb += 0.1 ; Glowg -= 0.1 ;
	glutPostRedisplay();
    break;
  }
}


void
main_menu_select(int value)
{
  char title[512];

  switch(value) {
  case 1:
    glutPostRedisplay();
    break;
  case 2:
    break;
  case 3:
    glutPostRedisplay();
    break;
  case 4:
    glutPostRedisplay();
    break;
  case 5:
    glutSetWindowTitle(title);
    break;
  case 666:
    exit(0);
    break;
  }
}

void
draw_mode_select(int value)
{
  DRAW_MODE = value;
  glutPostRedisplay();
}




/****************************************************************************/
/*																			*/
/*  initiate the graphical display stuff									*/
/*																			*/
/****************************************************************************/

void	display_setup() {

	  glutInitWindowPosition ( winPos_x, winPos_y );
	  glutInitWindowSize	( winSize_x, winSize_y );
	  glutInitDisplayMode	(GLUT_ACCUM | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE );
	  
	  glutCreateWindow	( "PERTECS:- by MRT" ) ;

	  myinit		    ( ) ;
	  glClearColor( background_r, background_g, background_b, 1.0);

	
	  if (fullscreen) glutFullScreen(); // set by S:F object
	  else if (newtitle) glutSetWindowTitle(title);  // 
	
	
//*	  make_surface ( surfacePtr, Num_surface ) ; // out of mesh data
	
//*	  data_struct       ( surfacePtr, Num_surface ) ;	
	  glutMouseFunc     ( mouse );
	  glutMotionFunc    ( motion );
	  glutKeyboardFunc  ( Key ) ;	
	  glutSpecialFunc   ( special );
	  glutReshapeFunc 	( myReshape ) ;
	  glutDisplayFunc   ( display );

//		  make_menus();

	if (tracex >= 0.00) glutIdleFunc(both);
	else if (trace != 0.00) glutIdleFunc(roll);
	else if (scan_incr != 0.00) glutIdleFunc(scope);
	else glutIdleFunc(NULL);
}




//****************************************************************************************//
void Initscreen (struct window *screenPtr) {

	strcpy (title, "PERTECS 2008: by MRT") ; // default window title for spod
	delay	= 0.0 ;
	duration = 0.0 ;
	
	winSize_x = 800 ;
	winPos_y = 600 ;
	winPos_x = 200 ;
	winPos_y = 100 ;

}


void Initview (struct envir *viewPtr) {
	int i;
	
	viewPtr->status = FALSE									;
	viewPtr->ViewNo	= 1										;
	viewPtr->viewlabel[0] = 0								;
	for (i=0; i < 1; i++)	viewPtr->Orientation[i]	= 0		;
	for (i=0; i < 6; i++)	viewPtr->Lighting[i]			;
	for (i=0; i < 12; i++)	viewPtr->Grid[i] = 0.0			;
	for (i=0; i < 7; i++) 	viewPtr->Colour[i] = 0.0		;
	for (i=0; i < 5; i++)	viewPtr->Partition[i] = 0.0		;
	for (i=0; i < 3; i++)	viewPtr->Mode[3] = FALSE		;
		viewPtr->Mode_4 = 10								;			

	for (i=0; i < 5; i++)	viewPtr->Index[i] = 0.0			;

		viewPtr->obj_enable	 = FALSE						;
	for (i=0; i < 1; i++) {
							viewPtr->surf_obj[i] = FALSE	;	
							viewPtr->graph_obj[i] = FALSE	;
	}
}

#define SLIPTIME 1000 // may need to think about ways to improve this estimating the computational cycle time

//****************************************************************************************//
void spod(struct xml_data *DataPtr) {

	display_setup();
//	if ( (realtime_rate = DataPtr->build.System_Sample_Rate ) == 0)  {
//		fprintf(stderr, "Sample rate is to be positive integer ... exiting... \n"); fflush(stderr);
//		exit (1);
//	}
	
//	realtime_rate = DEFAULTRATE; 

//	usleep_time =  (int) 1000000 / realtime_rate  - SLIPTIME;
//	if (usleep_time < 0 ) usleep_time = 0;
	
	real_scope(); 		// setting up time = 0
	cycles = 0;			// resetting cycles
	glutIdleFunc(real_scope);
	glutMainLoop() ;
}


//****************************************************************************************//
// this creates the spod graphics objects once all the info has been read in from plist files
void mkSpodobjects(struct xml_data *DataPtr)
{

	int  i;


	for (i=0; i < DataPtr->build.build_size; i++) { 
		if ( DataPtr->Modules[i]->type == TRACE) {


			pdiv++; // counts the number of curves which are not X: types. Convention is to load all curves before X: types.

	// start off the arrays  should only do this if Trace is selected
			Num_curve++; p++;
			
			curvePtr[p] = calloc( 1, sizeof( LINE )) ;

			if (( DataPtr->Modules[i]->ParamPtr[TRACELINETYP]->label[0] == '-') || ( DataPtr->Modules[i]->ParamPtr[TRACELINETYP]->label[0] == '_') ||( DataPtr->Modules[i]->ParamPtr[TRACELINETYP]->label[0] == '.') || ( DataPtr->Modules[i]->ParamPtr[TRACELINETYP]->label[0] == '|') || ( DataPtr->Modules[i]->ParamPtr[TRACELINETYP]->label[0] == ' ') || ( DataPtr->Modules[i]->ParamPtr[TRACELINETYP]->label[0] == 'b') || ( DataPtr->Modules[i]->ParamPtr[TRACELINETYP]->label[0] == 'B') || ( DataPtr->Modules[i]->ParamPtr[TRACELINETYP]->label[0] == 't') || ( DataPtr->Modules[i]->ParamPtr[TRACELINETYP]->label[0] == 'T') )
				 curvePtr[p]->linetype = DataPtr->Modules[i]->ParamPtr[TRACELINETYP]->label[0];
			 else curvePtr[p]->linetype = '.'; // default

			

// check for proper syntax here and exit if not right tbd
//			if ( DataPtr->Modules[i]->ParamPtr[TRACETIMER]->label[0] == 'x') curvePtr[p]->mode = xtime; ytime; ztime; blank or persistence


			if ( DataPtr->Modules[i]->ParamPtr[TRACEVISIBLE]->label[0] == 'e') curvePtr[p]->visible = GLUT_VISIBLE;
			else if ( DataPtr->Modules[i]->ParamPtr[TRACEVISIBLE]->label[0] == 'd') curvePtr[p]->visible = GLUT_NOT_VISIBLE;
			// else 


			if ( DataPtr->Modules[i]->ParamPtr[TRACESTATIC]->label[0] == 's') curvePtr[p]->kind = STATIC; // default to static for the time being
			else if ( DataPtr->Modules[i]->ParamPtr[TRACESTATIC]->label[0] == 'd') curvePtr[p]->kind = DYNAMIC; 

			
// weight
			curvePtr[p]->width = DataPtr->Modules[i]->ParamPtr[TRACELINEWGHT]->value; 



// this block of spod parameters are not used but initialised anyway		
			curvePtr[p]->xmax = 0.0 ;
			curvePtr[p]->ymax = 0.0 ;
			curvePtr[p]->zmax = 0.0 ;
			
			curvePtr[p]->xscale = 1;		//set some defaults ... but these are not used in pertecs  DataPtr->Modules[i]->ParamPtr[0]->value ;
			curvePtr[p]->yscale = 1;		//set some defaults ... but these are not used in pertecs  DataPtr->Modules[i]->ParamPtr[1]->value ;  
			curvePtr[p]->zscale = 1;		//set some defaults ... but these are not used in pertecs  DataPtr->Modules[i]->ParamPtr[2]->value ;  
			curvePtr[p]->xoffset = 0;		//set some defaults ... but these are not used in pertecs  DataPtr->Modules[i]->ParamPtr[3]->value ;  
			curvePtr[p]->yoffset = 0;		//set some defaults ... but these are not used in pertecs  DataPtr->Modules[i]->ParamPtr[4]->value ;  
			curvePtr[p]->zoffset = 0;		//set some defaults ... but these are not used in pertecs  DataPtr->Modules[i]->ParamPtr[5]->value ;  
//


			strncpy( curvePtr[p]->label, DataPtr->Modules[i]->name, LABEL_SIZE) ;
			
			curvePtr[p]->colour = (char) DataPtr->Modules[i]->ParamPtr[TRACECOLOUR]->label[0]; // first letter

			switch (curvePtr[p]->colour) {
				case 'r': //red
					curvePtr[p]->colour_red = 1.0; curvePtr[p]->colour_green = 0.0; curvePtr[p]->colour_blue = 0.0;
					break;
				case 'g': //green
					curvePtr[p]->colour_red = 0.0; curvePtr[p]->colour_green = 1.0; curvePtr[p]->colour_blue = 0.0;
					break;
				case 'b': //blue
					curvePtr[p]->colour_red = 0.0; curvePtr[p]->colour_green = 0.0; curvePtr[p]->colour_blue = 1.0;
					break;
				case 'm': //magenta
					curvePtr[p]->colour_red = 1.0; curvePtr[p]->colour_green = 0.0; curvePtr[p]->colour_blue = 1.0;
					break;
				case 'c': //cyan
					curvePtr[p]->colour_red = 0.0; curvePtr[p]->colour_green = 1.0; curvePtr[p]->colour_blue = 1.0;
					break;
				case 'y': //yellow
					curvePtr[p]->colour_red = 1.0; curvePtr[p]->colour_green = 1.0; curvePtr[p]->colour_blue = 0.0;
					break;
				case 'B': //black
					curvePtr[p]->colour_red = 0.0; curvePtr[p]->colour_green = 0.0; curvePtr[p]->colour_blue = 0.0;
					break;
				case 'W': //white
				case 'w': 
				default :
					curvePtr[p]->colour_red = 1.0; curvePtr[p]->colour_green = 1.0; curvePtr[p]->colour_blue = 1.0;
					break;
				
			}

		
			curvePtr[p]->part = 1 ;  //  always  1  for a trace
			curvePtr[p]->npolylinenodes = curvePtr[p]->index = DataPtr->Modules[i]->buff_sz ;  
			
			curvePtr[p]->polyline  =  ( float * )  calloc ( curvePtr[p]->npolylinenodes * 3,  sizeof ( float )) ; //creating buffer space
			
			int j, ii = 0;
			for ( j =0; j < curvePtr[p]->npolylinenodes; j++, ii+=3) { // generate a time trace in a buffer and clear the graph object
				DataPtr->Modules[i]->input_buffPtr[j] = (double) (-1.0 + 2.0 * ((float) j )/ ((float) (curvePtr[p]->npolylinenodes ) )) ; // in range -1 to 1 ...
				curvePtr[p]->polyline[ii]  = curvePtr[p]->xoffset ; // init to zero
				curvePtr[p]->polyline[ii+1] = curvePtr[p]->yoffset ; // init to zero
				curvePtr[p]->polyline[ii+2] = curvePtr[p]->zoffset ; //
			}
			
			
		}
	}
}
