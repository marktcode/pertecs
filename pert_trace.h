
#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif


#define INTEGER	2
#define	FLOAT	4


#define	VISIBLE 1
#define	NOTVISIBLE 0
#define DYNAMIC	1

#include "spod.h"







extern	double Glowr, Glowg, Glowb, ScaleIntensity;
extern	double background_r, background_g, background_b, complement;
extern	float start_scan, width_scan, scan_incr;
extern	float xorig, yorig, zorig, xlimit, ylimit, zlimit, zaxis, yaxis, xaxis;
extern	float grid_r, grid_g, grid_b; // grid colour

extern	SURF *surfacePtr[] ; // maximum number of surfaces
extern	LINE *curvePtr[] ; // maximum number of graphs
extern	BALL *ballPtr[] ; // maximum no of balls
extern	TXTLINE *textPtr[] ;  // maximum no of text lines
extern	JUMP *jumpPtr[];  // number of views

extern	int maxJumpcntr, Jumpcntr;
extern	int Num_curve, Num_kurve;
extern	int	p; // initialised to empty value for traces
extern	int pdiv; // counts number of curves loaded
