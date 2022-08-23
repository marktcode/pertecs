/* mkfilter -- given n, compute recurrence relation
   to implement Butterworth, Bessel or Chebyshev filter of order n
   A.J. Fisher, University of York   <fisher@minster.york.ac.uk>
   September 1992 */
   
/* modified to wrk in with pertecs MRT 2008 */

/* Main module */

#include <stdio.h>
#include <stdlib.h>
//#include <complex.h>

#include <math.h>
#include "pertecs.h"
#include "mkfilter.h"

#define opt_be 0x0001	/* -Be		Bessel cheracteristic	       */
#define opt_bu 0x0002	/* -Bu		Butterworth characteristic     */
#define opt_ch 0x0004	/* -Ch		Chebyshev characteristic       */

#define opt_lp 0x0008	/* -Lp		low-pass		       */
#define opt_hp 0x0010	/* -Hp		high-pass		       */
#define opt_bp 0x0020	/* -Bp		band-pass		       */

#define opt_a  0x0040	/* -a		alpha value		       */
#define opt_e  0x0100	/* -e		execute filter		       */
#define opt_l  0x0200	/* -l		just list filter parameters    */
#define opt_o  0x0400	/* -o		order of filter		       */
#define opt_p  0x0800	/* -p		specified poles only	       */
#define opt_w  0x1000	/* -w		don't pre-warp		       */


static int order, numpoles;
static double raw_alpha1, raw_alpha2;
static struct complex dc_gain, fc_gain, hf_gain;
static uint opts;	/* option flag bits */

static double warped_alpha1, warped_alpha2, chebrip;
static uint polemask;
static bool optsok;

static struct complex spoles[MAXPOLES];
static struct complex zpoles[MAXPOLES], zzeros[MAXPOLES];
static double xcoeffs[MAXPOLES+1], ycoeffs[MAXPOLES+1];

static struct complex bessel_poles[] =
  { /* table produced by /usr/fisher/bessel --	N.B. only one member of each C.Conj. pair is listed */
    { -1.000000e+00,  0.000000e+00 },	 { -8.660254e-01, -5.000000e-01 },    { -9.416000e-01,	0.000000e+00 },
    { -7.456404e-01, -7.113666e-01 },	 { -9.047588e-01, -2.709187e-01 },    { -6.572112e-01, -8.301614e-01 },
    { -9.264421e-01,  0.000000e+00 },	 { -8.515536e-01, -4.427175e-01 },    { -5.905759e-01, -9.072068e-01 },
    { -9.093907e-01, -1.856964e-01 },	 { -7.996542e-01, -5.621717e-01 },    { -5.385527e-01, -9.616877e-01 },
    { -9.194872e-01,  0.000000e+00 },	 { -8.800029e-01, -3.216653e-01 },    { -7.527355e-01, -6.504696e-01 },
    { -4.966917e-01, -1.002509e+00 },	 { -9.096832e-01, -1.412438e-01 },    { -8.473251e-01, -4.259018e-01 },
    { -7.111382e-01, -7.186517e-01 },	 { -4.621740e-01, -1.034389e+00 },    { -9.154958e-01,	0.000000e+00 },
    { -8.911217e-01, -2.526581e-01 },	 { -8.148021e-01, -5.085816e-01 },    { -6.743623e-01, -7.730546e-01 },
    { -4.331416e-01, -1.060074e+00 },	 { -9.091347e-01, -1.139583e-01 },    { -8.688460e-01, -3.430008e-01 },
    { -7.837694e-01, -5.759148e-01 },	 { -6.417514e-01, -8.175836e-01 },    { -4.083221e-01, -1.081275e+00 },
  };

static struct complex cmone = { -1.0, 0.0 };
static struct complex czero = {	 0.0, 0.0 };
static struct complex cone  = {	 1.0, 0.0 };
static struct complex ctwo  = {	 2.0, 0.0 };
static struct complex chalf = {	 0.5, 0.0 };

//extern double pow(), tan(), atof(), fabs(), hypot(), atan2(), sqrt();
//extern double sinh(), cosh(), asinh();

extern struct complex evaluate();	/* from complex.o */

//forward uint decodeopts();
//forward double getfarg();

extern struct complex ccsqrt(), ccexp(), cconj(), cadd(), csub(), cmul(), cdiv();
#define cneg(z) csub(czero, z)


static usage()
  { fprintf(stderr, "Usage: mkfilter [-Be | -Bu | -Ch <r>] [-Lp | -Hp | -Bp] [-p <n1> <n2> ...] [-elw]");
    fprintf(stderr, "-o <order> -a <alpha1> [ <alpha2> ]\n");
    fprintf(stderr, "  -Be, Bu     = Bessel, Butterworth\n");
    fprintf(stderr, "  -Ch <r>     = Chebyshev (r dB ripple)\n");
    fprintf(stderr, "  -Lp, Hp, Bp = low-pass, high-pass, band-pass\n");
    fprintf(stderr, "  -p          = use listed poles only (ni = 0 .. order-1)\n");
    fprintf(stderr, "  -e          = execute filter from stdin to stdout\n");
    fprintf(stderr, "  -l          = just list <order> parameters\n");
    fprintf(stderr, "  -w          = don't pre-warp frequencies\n");
    fprintf(stderr, "  order = 1..%d;  alpha = f(corner)/f(sample)\n", MAXORDER);
    exit(1);
  }


static uint decodeopts(s) char *s;
  { uint m = 0;
    unless (*(s++) == '-') usage();
    if (seq(s,"Be")) m |= opt_be;
    else if (seq(s,"Bu")) m |= opt_bu;
    else if (seq(s, "Ch")) m |= opt_ch;
    else if (seq(s, "Lp")) m |= opt_lp;
    else if (seq(s, "Hp")) m |= opt_hp;
    else if (seq(s, "Bp")) m |= opt_bp;
    else
      { until (*s == '\0')
	  { char c = *(s++);
	    switch (c)
	      { default:    usage();
		case 'a':   m |= opt_a; break;
		case 'e':   m |= opt_e; break;
		case 'l':   m |= opt_l; break;
		case 'o':   m |= opt_o; break;
		case 'p':   m |= opt_p; break;
		case 'w':   m |= opt_w; break;
	      }
	  }
      }
    return m;
  }

static double getfarg(s) char *s;
  { if (s == NULL) usage();
    return atof(s);
  }

static int getiarg(s) char *s;
  { if (s == NULL) usage();
    return atoi(s);
  }




static opterror(msg, p1, p2) char *msg; int p1, p2;
  { fprintf(stderr, "mkfilter: "); fprintf(stderr, msg, p1, p2); putc('\n', stderr);
    optsok = FALSE;
  }





static setdefaults()
  { unless (opts & opt_p) polemask = -1;
    unless (opts & opt_bp) raw_alpha2 = raw_alpha1;
  }

static choosepole(z) struct complex z;
  { if (z.re < 0.0)
      { if (polemask & 1) spoles[numpoles++] = z;
	polemask >>= 1;
      }
  }

static compute_s() /* compute S-plane poles for prototype LP filter */
  { numpoles = 0;
    if (opts & opt_be)
      { /* Bessel filter */
	int i;
	int p = (order*order)/4; /* ptr into table */
	if (order & 1) choosepole(bessel_poles[p++]);
	for (i=0; i < order/2; i++)
	  { choosepole(bessel_poles[p]);
	    choosepole(cconj(bessel_poles[p]));
	    p++;
	  }
      }
    if (opts & (opt_bu | opt_ch))
      { /* Butterworth filter */
	int i;
	for (i=0; i < 2*order; i++)
	  { struct complex s;
	    s.re = 0.0; s.im = (order & 1) ? (i*PI) / order : ((i+0.5)*PI) / order;
	    choosepole(ccexp(s));
	  }
      }
    if (opts & opt_ch)
      { /* modify for Chebyshev (p. 136 DeFatta et al.) */
	double rip, eps, y; int i;
	if (chebrip >= 0.0)
	  { fprintf(stderr, "mkfilter: Chebyshev ripple is %g dB; must be .lt. 0.0\n", chebrip);
	    exit(1);
	  }
	rip = pow(10.0, -chebrip / 10.0);
	eps = sqrt(rip - 1.0);
	y = asinh(1.0 / eps) / (double) order;
	if (y <= 0.0)
	  { fprintf(stderr, "mkfilter: bug: Chebyshev y=%g; must be .gt. 0.0\n", y);
	    exit(1);
	  }
	for (i=0; i < numpoles; i++)
	  { spoles[i].re *= sinh(y);
	    spoles[i].im *= cosh(y);
	  }
      }
  }


static normalize()
  { struct complex w1, w2; int i;
    /* for bilinear transform, perform pre-warp on alpha values */
    if (opts & opt_w)
      { warped_alpha1 = raw_alpha1;
	warped_alpha2 = raw_alpha2;
      }
    else
      { warped_alpha1 = tan(PI * raw_alpha1) / PI;
	warped_alpha2 = tan(PI * raw_alpha2) / PI;
      }
    w1.re = TWOPI * warped_alpha1; w1.im = 0.0;
    w2.re = TWOPI * warped_alpha2; w2.im = 0.0;
    /* transform prototype into appropriate filter type (lp/hp/bp) */
    switch (opts & (opt_lp + opt_hp + opt_bp))
      { case opt_lp:
	    for (i=0; i < numpoles; i++) spoles[i] = cmul(spoles[i], w1);
	    break;

	case opt_hp:
	    for (i=0; i < numpoles; i++) spoles[i] = cdiv(w1, spoles[i]);
	    /* also N zeros at (0,0) */
	    break;

	case opt_bp:
	  { struct complex w0, bw;
	    w0 = ccsqrt(cmul(w1, w2));
	    bw = csub(w2, w1);
	    for (i=0; i < numpoles; i++)
	      { struct complex hba, temp;
		hba = cmul(chalf, cmul(spoles[i], bw));
		temp = cdiv(w0, hba);
		temp = ccsqrt(csub(cone, cmul(temp, temp)));
		spoles[i] = cmul(hba, cadd(cone, temp));
		spoles[numpoles+i] = cmul(hba, csub(cone, temp));
	      }
	    /* also N zeros at (0,0) */
	    numpoles *= 2;
	    break;
	  }
      }
  }

static compute_z() /* given S-plane poles, compute Z-plane poles */
  { int i;
    for (i=0; i < numpoles; i++)
      { /* use bilinear transform */
	struct complex top, bot;
	top = cadd(ctwo, spoles[i]);
	bot = csub(ctwo, spoles[i]);
	zpoles[i] = cdiv(top, bot);
	switch (opts & (opt_lp + opt_hp + opt_bp))
	  { case opt_lp:    zzeros[i] = cmone; break;
	    case opt_hp:    zzeros[i] = cone; break;
	    case opt_bp:    zzeros[i] = (i & 1) ? cone : cmone; break;
	  }
      }
  }

static multin(w, coeffs) struct complex w; struct complex coeffs[];
  { /* multiply factor (z-w) into coeffs */
    struct complex nw; int i;
    nw = cneg(w);
    for (i=numpoles; i >= 1; i--)
      coeffs[i] = cadd(cmul(nw, coeffs[i]), coeffs[i-1]);
    coeffs[0] = cmul(nw, coeffs[0]);
  }

static expand(pz, coeffs) struct complex pz[], coeffs[];
  { /* compute product of poles or zeros as a polynomial of z */
    int i;
    coeffs[0] = cone;
    for (i=0; i < numpoles; i++) coeffs[i+1] = czero;
    for (i=0; i < numpoles; i++) multin(pz[i], coeffs);
    /* check computed coeffs of z^k are all real */
    for (i=0; i < numpoles+1; i++)
      { if (fabs(coeffs[i].im) > FEPS)
	  { fprintf(stderr, "mkfilter: coeff of z^%d is not real; poles are not complex conjugates\n", i);
	    exit(1);
	  }
      }
  }

static expandpoly() /* given Z-plane poles & zeros, compute top & bot polynomials in Z, and then recurrence relation */
  { struct complex topcoeffs[MAXPOLES+1], botcoeffs[MAXPOLES+1];
    struct complex st, zfc; int i;
    expand(zzeros, topcoeffs);
    expand(zpoles, botcoeffs);
    dc_gain = evaluate(topcoeffs, botcoeffs, numpoles, cone);
    st.re = 0.0; st.im = TWOPI * 0.5 * (raw_alpha1 + raw_alpha2); /* "jwT" for centre freq. */
    zfc = ccexp(st);
    fc_gain = evaluate(topcoeffs, botcoeffs, numpoles, zfc);
    hf_gain = evaluate(topcoeffs, botcoeffs, numpoles, cmone);
    for (i=0; i <= numpoles; i++)
      { xcoeffs[i] = topcoeffs[i].re / botcoeffs[numpoles].re;
	ycoeffs[i] = -(botcoeffs[i].re / botcoeffs[numpoles].re);
      }
  }


static execute()
  { double xv[MAXPOLES+1], yv[MAXPOLES+1]; int i;
    unless (xcoeffs[numpoles] == 1.0)
      { fprintf(stderr, "mkfilter: bug in execute (%g)\n", xcoeffs[numpoles]);
	exit(1);
      }
    for (i=0; i <= numpoles; i++) xv[i] = yv[i] = 0.0;
    for (;;)
      { double x; int j;
	int ni = scanf("%lg", &x);
	if (ni < 0) return; /* eof */
	if (ni == 0)
	  { fprintf(stderr, "mkfilter: scanf error\n");
	    exit(1);
	  }
	for (j=0; j < numpoles; j++)
	  { xv[j] = xv[j+1];
	    yv[j] = yv[j+1];
	  }
	xv[numpoles] = yv[numpoles] = x;
	for (j=0; j < numpoles; j++) yv[numpoles] += (xcoeffs[j] * xv[j]) + (ycoeffs[j] * yv[j]);
	printf("%14.10f\n", yv[numpoles]);
      }
  }

static printcmdline(argv) char *argv[];
  { int k = 0;
    until (argv[k] == NULL)
      { if (k > 0) putchar(' ');
	fputs(argv[k++], stdout);
      }
    putchar('\n');
 }

static printgain(str, gain) char *str; struct complex gain;
  { double r = hypot(gain.im, gain.re);
    printf("gain at %s:   mag = %15.9e", str, r);
    if (r > FEPS) printf("   phase = %14.10f pi", atan2(gain.im, gain.re) / PI);
    putchar('\n');
  }

static prcomplex(z) struct complex z;
  { printf("%14.10f + j %14.10f", z.re, z.im);
  }

static printrat_s()
  { int i;
    printf("S-plane zeros:\n");
    switch (opts & (opt_lp + opt_hp + opt_bp))
      { case opt_lp:
	    printf("\tnone\n");
	    break;

	case opt_hp:
	    putchar('\t'); prcomplex(czero); printf("\t%d times\n", numpoles);
	    break;

	case opt_bp:
	    putchar('\t'); prcomplex(czero); printf("\t%d times\n", numpoles/2);
	    break;
      }
    putchar('\n');
    printf("S-plane poles:\n");
    for (i=0; i < numpoles; i++) { putchar('\t'); prcomplex(spoles[i]); putchar('\n'); }
    putchar('\n');
  }

static printrat_z() /* print rational form of H(z) */
  { int i;
    printf("Z-plane zeros:\n");
    switch (opts & (opt_lp + opt_hp + opt_bp))
      { case opt_lp:
	    putchar('\t'); prcomplex(cmone); printf("\t%d times\n", numpoles);
	    break;

	case opt_hp:
	    putchar('\t'); prcomplex(cone); printf("\t%d times\n", numpoles);
	    break;

	case opt_bp:
	    putchar('\t'); prcomplex(cone); printf("\t%d times\n", numpoles/2);
	    putchar('\t'); prcomplex(cmone); printf("\t%d times\n", numpoles/2);
	    break;
      }
    putchar('\n');
    printf("Z-plane poles:\n");
    for (i=0; i < numpoles; i++) { putchar('\t'); prcomplex(zpoles[i]); putchar('\n'); }
    putchar('\n');
  }

static printrecurrence() /* given (real) Z-plane poles & zeros, compute & print recurrence relation */
  { int i;
    printf("Recurrence relation:\n");
    printf("y[n] = ");
    for (i=0; i < numpoles+1; i++)
      { if (i > 0) printf("     + ");
	printf("(%3g * x[n-%2d])\n", xcoeffs[i], numpoles-i);
      }
    putchar('\n');
    for (i=0; i < numpoles; i++)
      { printf("     + (%14.10f * y[n-%2d])\n", ycoeffs[i], numpoles-i);
      }
    putchar('\n');
  }

static printfilter()
  { printf("raw alpha1    = %14.10f\n", raw_alpha1);
    printf("warped alpha1 = %14.10f\n", warped_alpha1);
    printf("raw alpha2    = %14.10f\n", raw_alpha2);
    printf("warped alpha2 = %14.10f\n", warped_alpha2);
    printgain("dc    ", dc_gain);
    printgain("centre", fc_gain);
    printgain("hf    ", hf_gain);
    putchar('\n');
    printrat_s();
    printrat_z();
    printrecurrence();
  }



void mkfilter(struct xml_data pertecs_data)
{
	uint m;
	int  i,k;
		
	for (i=0; i < pertecs_data.build.build_size; i++) { 
	
		if ( pertecs_data.Modules[i]->type == FILTER) {
			opts = polemask = 0;
			order = pertecs_data.Modules[i]->buff_sz / 4 ;
			opts = m = 0;

			if (!strcmp(pertecs_data.Modules[i]->ParamPtr[0]->label,"Be")) opts |= opt_be;
			else if (!strcmp(pertecs_data.Modules[i]->ParamPtr[0]->label,"Bu")) opts |= opt_bu;
			else if (!strcmp(pertecs_data.Modules[i]->ParamPtr[0]->label, "Ch")) opts |= opt_ch;
			
			if (!strcmp(pertecs_data.Modules[i]->ParamPtr[1]->label, "Lp")) opts |= opt_lp;
			else if (!strcmp(pertecs_data.Modules[i]->ParamPtr[1]->label, "Hp")) opts |= opt_hp;
			else if (!strcmp(pertecs_data.Modules[i]->ParamPtr[1]->label, "Bp")) opts |= opt_bp;
	
		
		
			if (opts & opt_ch) chebrip = pertecs_data.Modules[i]->ParamPtr[2]->value ;
			raw_alpha1 = pertecs_data.Modules[i]->ParamPtr[3]->value / 2.00 ;  // assume normalised frequency: 1.0 correxponds to (sample-rate / 2.0)
			raw_alpha2 = pertecs_data.Modules[i]->ParamPtr[4]->value / 2.00 ;  // normalised frequency
		   
		   opts |= opt_o; // effectively marks the order entry true
		   opts |= opt_a; // marks the corner frequencies

		
			optsok = TRUE;
	
			m = opts & (opt_be | opt_bu | opt_ch); 
			unless (m == opt_be || m == opt_bu || m == opt_ch) {
				opterror(" specify [Be][Bu][Ch]");
				exit(1);
			}
		
			m = opts & (opt_lp | opt_hp | opt_bp);
			unless (m == opt_lp || m == opt_hp || m == opt_bp) {
				opterror("specify [Lp][Hp][Bp]");
				exit(1);
			}

/*			unless (order >= 1 && order <= MAXORDER)
			opterror("order must be in range 1 .. %d", MAXORDER);
			if (opts & opt_p)
			  { 
				m = (1 << order) - 1; // "order" bits set 
				if ((polemask & ~m) != 0)
				opterror("order=%d, so args to -p must be in range 0 .. %d", order, order-1);
			  }
*/
		

			setdefaults();
			compute_s();
			normalize();
			compute_z();
			expandpoly();
// amend the gain accordingly

			if (!strcmp(pertecs_data.Modules[i]->ParamPtr[1]->label, "Lp")) pertecs_data.Modules[i]->ParamPtr[GAIN]->value = pertecs_data.Modules[i]->ParamPtr[GAIN]->value/dc_gain.re ;
			else if (!strcmp(pertecs_data.Modules[i]->ParamPtr[1]->label, "Hp")) pertecs_data.Modules[i]->ParamPtr[GAIN]->value = pertecs_data.Modules[i]->ParamPtr[GAIN]->value/hf_gain.re ;
			else if (!strcmp(pertecs_data.Modules[i]->ParamPtr[1]->label, "Bp")) pertecs_data.Modules[i]->ParamPtr[GAIN]->value = pertecs_data.Modules[i]->ParamPtr[GAIN]->value/fc_gain.re ;
	//		else pertecs_data.Modules[i]->ParamPtr[GAIN]->value = 1.0;


			for (k=0; k < order; k++) { 
				pertecs_data.Modules[i]->input_buffPtr[2 * order + k] = xcoeffs[k]; 
				pertecs_data.Modules[i]->input_buffPtr[3 * order + k] = ycoeffs[k];
			}
			
//			printfilter();

		}
	}
}

