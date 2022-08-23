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



#define	_new(t)			((t*)malloc(sizeof(t)))
#define	_new_array(t, n)	((t*)malloc(sizeof(t) * (n)))
#define	_resize_array(a, t, n)	((t*)realloc((a), sizeof(t) * (n)))
#define _size_array(a,t,n0,n1)  a = (n0 == 0 ? _new_array(t,n1) : \
                                               _resize_array(a,t,n1))
#define	_delete(object)		((void)(((object)!=NULL) ? \
				    free((char*)(object)),(object)=NULL : 0))


#define FALSE 0
#define TRUE 1


#define	VISIBLE 1
#define	NOTVISIBLE 0
#define DYNAMIC	1

#define INTEGER	2
#define	FLOAT	4


#define STATIC 0
#define DYNAMIC 1
#define FULLYSPECIFIED 2
#define COLOUR_FUNC 2


#define NUM_TXTLINES 100
#define TITLE_SIZE 150
#define LABEL_SIZE 120
#define JUMP_SIZE 150



#define RAW 1
#define TEXT 0


#define MIN_CHROM 0.0
#define MAX_CHROM 1.0

#define MIN_LUM	0.0
#define MAX_LUM	1.0
#define DEF_LUM	0.0

#define DEF_ORG	0.0
#define MAX_X	25.0
#define DEF_X	9.0

#define MAX_Y	15.0
#define DEF_Y	6.0

#define MAX_Z	15.0
#define DEF_Z	6.0

#define ORIGIN	0.0
#define	NORMALE	1.0





typedef struct  JUMP {

  int  spinxsurface  		;     
  int  spinysurface  		;     
  int  spinxlight  			;     
  int  spinylight  			;     
  int  spinxlight2  		;     
  int  spinylight2  		;     
  int  spinxlight3  		;     
  int  spinylight3  		;     

  float  Glowr  			;     
  float  Glowg  			;     
  float  Glowb  			;     
  float  ScaleIntensity  	;     
  float  background_r  		;     
  float  background_g  		;     
  float  background_b  		;     

  float  start_scan  		;     
  float  scan_incr  		;   
  float  width_scan   		;  
  float  trace    			;   
  float  tracex  			;


  float  xaxis  			;     
  float  yaxis  			;     
  float  zaxis  			;     
  float  xlimit  			;     
  float  ylimit  			;     
  float  zlimit  			;     
  float  xorig  			;     
  float  yorig  			;     
  float  zorig  			;     
  float  grid_r  			;     
  float  grid_g  			;     
  float  grid_b  			;     

  int  simultrace  			;     
  int  flat  				;  
  int  Showlabels  			;     
  float  Tracelight  		;
       
  float  awake  			;     
  float  sleepwake  		;     
  float  sleep21  			;     
  float  sleepREM  			;     

  int	object[20]			;
  int	visibility[20]		;

} JUMP ;


typedef struct TAG {

  long poly					;      /* an index to a tagged polygon               */
  long vert					;      /* an index to the tagged vertex              */

} TAG ;


typedef struct  VERTEX {

  float    xyz[3]			;     /* x,y,z of the vertex (modified)            */
  float    nxyz[3]			;     /* x,y,z of the vertex (never modified)      */
  long      np				;     /* number of polygons associated with node   */
  long      plist[10]		;     /* list of polygons associated with node     */
  float    norm[3]			;     /* polygon vertex normal                     */

} VERTEX ;


typedef struct  POLYGON {

  VERTEX  *vertex[3]		;     /* pointer to an array of three vertices     */
  float	   rgb[3]			;     /* colour of polygon*/

} POLYGON ;


typedef struct  SURF {

  float		 xscale			;  /* x-scale */
  float		 yscale			;  /* y- */
  float		 zscale			;  /* z- */
  float		 xoffset		;  /* x-offset */
  float		 yoffset		;  /* y- */
  float		 zoffset		;  /* z- */
  float		 xmin, xmax		;  /* x-min/max */
  float		 ymin, ymax		;  /* y-min/max */
  float		 zmin, zmax		;  /* z-min/max */
  long       npolylinenodes ;  /* number of nodes in the poly line          */
  float     *polyline       ;  /* xyz nodes in the poly line                */

  long       npolygons      ;  /* total number of polygons                  */
  POLYGON  **polygon        ;  /* pointer to the polygon list               */

  long       part			;  /* number of polygon indices                 */
  long       index			;  /* number of polygon indices                 */
  char       colour			;  /* what colour? p(planes) r, g, b, y, m, c */
  float		 colour_red		;      /*  red  */
  float		 colour_green	;      /*  green  */
  float		 colour_blue	;      /*  blue  */
  int        visible		;  /* is the surface visible GLUT_NOT_VISIBLE/GLUT_VISIBLE */
  int        kind			;  /* is the surface STATIC or DYNAMIC */
  char		 label[LABEL_SIZE]		;  /* give the surface a label */
  
} SURF ;

typedef struct  LINE {
  long       npolylinenodes ;  /* number of nodes in the poly line          */
  float     *polyline       ;  /* xyz nodes in the poly line                */

  long		 retrace		; /* points to the latest entry to */
  
  long       part			;  /* number of lines in y direction             */
  long       index			;  /* number of points in x direction		    */
  char       colour			;  /* what colour? p(planes) r, g, b, y, m, c */
  float		 colour_red		;      /*  red  */
  float		 colour_green	;      /*  green  */
  float		 colour_blue	;      /*  blue  */
  char       linetype		;  /* what linetype? '-' (default line), '.' (points) */
  char		 label[LABEL_SIZE]		;  /* give the curve a label */
  float		 width			; /* width of line or points */
  float		 xscale			;  /* x-scale */
  float		 yscale			;  /* y- */
  float		 zscale			;  /* z- */
  float		 xoffset		;  /* x-offset */
  float		 yoffset		;  /* y- */
  float		 zoffset		;  /* z- */
  float		 xmin, xmax		;  /* x-min/max */
  float		 ymin, ymax		;  /* y-min/max */
  float		 zmin, zmax		;  /* z-min/max */
  int        visible		;  /* is the line visible 0/1 */
  int        kind			;  /* is the surface STATIC or DYNAMIC */
  char		 timeaxis			; /* indicates the time axis */
} LINE ;


typedef struct  TXTLINE {

  char		 label[LABEL_SIZE]		;  /* give the text object a label */
  char       colour			;  /* what colour? r, g, b, y, m, c, w, b*/
  int        visible		;  /* is the object visible 0/1 */
  char       linetype		;  /* what linetype? '-' (default line), '.' (points) */
  float		 width			; /* width of line or points */
  float		 xoffset		;  /* x-offset for positioning the display of the text*/
  float		 yoffset		;  /* y- */
  float		 zoffset		;  /* z- */
} TXTLINE ;



typedef struct BALL {

  int		 ball			;      /* an index to a tagged polygon   */
  char		 colour			;      /* single character spec */
  float		 colour_red		;      /*  ball colour  */
  float		 colour_green	;      /*  ball colour  */
  float		 colour_blue	;      /*  ball colour  */
  char		 type			;      /* s=sphere,t=teapot,b=box,c=cone,d=donut  */
  int        visible		;  		/* is the object visible 0/1 */
  float      size			;  		/* size of ball in pts  */
  char		 label[LABEL_SIZE]		;  /* give the ball a label */
  float      xval			;  		/* x-coordinate value  of the current position of the ball*/
  float      yval			;  		/* y-coordinate value  */
  float      zval			;  		/* z-coordinate value  */
  float		 xscale			;  		/* x-scale for converting xval to displayed text */
  float		 yscale			;  		/* y- */
  float		 zscale			;  		/* z- */
  float		 xoffset		;  		/* x-offset displayed text value */
  float		 yoffset		;  		/* y- */
  float		 zoffset		;  		/* z- */
  float		 txt_xoffset	;  		/* placement of the text */
  float		 txt_yoffset	;  		/* y- */
  float		 txt_zoffset	;  		/* z- */

} BALL ;




/* main.c								*/
#define LIMIT 5.0

extern int verbose, Num_curve, Num_kurve, Num_surface, Num_texts, sleepStateFlag;
extern float Glowr, Glowg, Glowb, ScaleIntensity;
extern float background_r, background_g, background_b, complement;
extern  float  sleepwake, sleep21, sleepREM, sleepREMslope, awake;
extern int Showlabels, fullscreen, simultrace, flat;
extern float Tracelight, trace, tracex;
extern float start_scan, width_scan, scan_incr;
extern float xorig, yorig, zorig, xlimit, ylimit, zlimit, zaxis, yaxis, xaxis;
extern int center_axis_flag, axis_markers_flag, lock_axis_flag, lock_axis_light_flag, lock_surface_light_flag, lock_markers_light_flag;

extern float grid_r, grid_g, grid_b, xlinlog, ylinlog, zlinlog ;
extern JUMP	*jumpPtr[];
extern int maxJumpcntr, Jumpcntr;

void setview(int x_orientation, int y_orientation);
void setlight_obj(int x_orientation, int y_orientation);
void setlight_2(int x_orientation, int y_orientation);
void setlight_3(int x_orientation, int y_orientation);

extern int newtitle;
extern char title[]; 
extern int winSize_x, winSize_y; 
extern int winPos_x, winPos_y; 

extern	int helpmenu, textdisp ;
extern	int	xtextstart, ytextstart ;

extern  float spinxlight;
extern  float spinylight;
extern  float spinxlight2;
extern  float spinylight2;
extern  float spinxlight3;
extern  float spinylight3;
extern  float spinxsurface;
extern  float spinysurface;

extern  float surface_red;
extern  float surface_green;
extern  float surface_blue;
extern  float surface_intsty;
extern  float surface_shadow;

extern  float delay, duration, cycles;
extern	int realtime_rate, usleep_time;

/* make_surface.c								*/
void make_surface 	      	( SURF **, int ) ;
void scope					(void);
void roll					(void);

void averaged_vertex_normals 	( SURF *surface, long p, 
			          float *n1, float *n2, float *n3       ) ; 
void surface_reset ( SURF *surface );
void data_struct ( SURF **surfacePtr, int Num_surface  );



/* display.c								*/

// void paint_polyline     	( SURF *surface 				) ;
void screentext 			( LINE **curvePtr, int Num_curve, TXTLINE **textPtr, int Num_texts) ;
void paint_curves         	( LINE **curvePtr, int Num_curve );
void paint_polygons         ( SURF **surfacePtr, int Num_surface, long type, int spinx, int spiny );
void paint_ball       	  	( LINE **curvePtr, BALL **ballPtr);
void calculate_polygon_vertex_normal ( SURF **surfacePtr, int Num_surface ) ;
void draw_axis          	( float zaxis, float yaxis, float xaxis, float grid_r, float grid_g, float grid_b, int offsetFlg, int axisFlg ) ;

extern void both(void);


/* fileio.c */
extern   float scalex, scaley, scalez, coords;
extern  float maxlimity, maxlimitz;
extern  int pdiv;
int read_array ( FILE *InFile, SURF **surfacePtr, LINE **curvePtr, BALL **ballPtr, TXTLINE **textPtr, int textin, int echo, int raw, char *Search_labelPtr, int Search_count, LINE **bufferPtrPtr); 
int print_header(FILE *OutStream, char key, char colour, char *label, int index , int part , float scalex, float scaley, float scalez, float offsetx, float offsety, float offsetz);



