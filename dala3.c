/* dala3 --- plot an HPGL mandala                           2011-10-19 */
/* Copyright (c) 2011 John Honniball, Froods Software Development      */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "hpgllib.h"

double lissajous (double x0, double y0, double side, double f1, double f2, double theta, int npts);
double diamondsquare (double x0, double y0, double side);
double zigzagring (double x0, double y0, double r1, double r2, int npts, int incr, int flag);
double radials (double x0, double y0, double inner, double length, int n);
void ringoboxes (double x0, double y0, double radius, int nboxes, int ninner);
double ringofcircles (double x0, double y0, double radius, int ncirc);
double sqwavering (double x0, double y0, double radius, double len, int nwaves);
double ellipsering (double x0, double y0, double a, double b, int nell);
void ellipse (double x0, double y0, double a, double b, double theta);


int main (int argc, char * const argv[])
{
   int opt;
   double xc, yc;
   double maxx, maxy;
   double height;
   double radius;
   
   while ((opt = getopt (argc, argv, "no:p:s:t:v:")) != -1) {
      switch (opt) {
      case 'n':
      case 'o':
      case 'p':
      case 's':
      case 't':
      case 'v':
         plotopt (opt, optarg);
         break;
      default: /* '?' */
         fprintf (stderr, "Usage: %s [-p pen] [-s <size>] [-t title]\n",
                  argv[0]);
         fprintf (stderr, "       <size> ::= A1 | A2 | A3 | A4 | A5\n");
//       exit (EXIT_FAILURE);
      }
   }

   plotbegin (0);

   getplotsize (&maxx, &maxy);
   
   xc = maxx / 2.0;
   yc = maxy / 2.0;
   
   height = maxy;

   /* Draw square border */
   rectangle (xc - (height / 2.0), 0.0, xc + (height / 2.0), maxy);

   /* Draw central Lissajous curve */
   radius = lissajous (xc, yc, 38.0 * 40.0, 3.0, 4.0, 0.0, 144);
   
   /* Plot ring of ellipses around Lissajous */
   circle (xc, yc, radius);
   
   radius = ellipsering (xc, yc, radius + (1.0 * 40.0), radius + (9.0 * 40.0), 3);
   
   circle (xc, yc, radius + (1.0 * 40.0));
   circle (xc, yc, radius + (51.0 * 40.0));
   
   /* Plot radial lines */
   radius = radials (xc, yc, radius + (1.0 * 40.0), 5.0 * 40.0, 63);
   
   /* Plot two rings of zigzags */
   radius = zigzagring (xc, yc, radius, radius + (20.0 * 40.0), 63, 4, 0);
   radius = zigzagring (xc, yc, radius, radius + (25.0 * 40.0), 63, 8, 1);
   
   /* Plot ring of boxes within boxes */
   ringoboxes (xc, yc, radius + (1.0 * 40.0), 48, 3);
   
   radius = ringofcircles (xc, yc, radius + (12.0 * 40.0), 96);
   
   radius = sqwavering (xc, yc, radius + (1.0 * 40.0), 10.0 * 40.0, 24);
   radius = sqwavering (xc, yc, radius + (1.0 * 40.0), 10.0 * 40.0, 25);
   
   plotend ();
   
   return (0);
}


double lissajous (double x0, double y0, double side, double f1, double f2, double theta, int npts)
{
   double t;
   double t1, t2;
   double delta;
   double sintheta, costheta;
   double x, y;
   double r;
   int i;
   
   side /= 2.0;

   delta = (2.0 * M_PI) / (double)npts;

   r = side;
   
   sintheta = sin (theta);
   costheta = cos (theta);
   
   for (i = 0; i <= npts; i++) {
      t = (double)i * delta;
      
      t1 = t * f1;
      t2 = t * f2;
      
      x = (r * cos (t1) * costheta) - (r * sin (t2) * sintheta);
      y = (r * cos (t1) * sintheta) + (r * sin (t2) * costheta);
      
      if (i == 0)
         moveto (x0 + x, y0 + y);
      else
         lineto (x0 + x, y0 + y);
   }
   
   /* Return radius of circumscribing circle */
   return (sqrt ((side * side) * 2.0));
}


double diamondsquare (double x0, double y0, double side)
{
   side /= 2.0;
   
   /* Plot outer square */
   rectangle (x0 - side, y0 - side, x0 + side, y0 + side);
   
   /* Plot inner diagonal square */
   moveto (x0, y0 - side);
   lineto (x0 + side, y0);
   lineto (x0, y0 + side);
   lineto (x0 - side, y0);
   lineto (x0, y0 - side);
   
   /* Return radius of circumscribing circle */
   return (sqrt ((side * side) * 2.0));
}


double zigzagring (double x0, double y0, double r1, double r2, int npts, int incr, int flag)
{
   int i;
   double x1[128], y1[128];
   double x2[128], y2[128];
   double theta1, theta2;
   double delta;
   int n1, n2;
   
   delta = (2.0 * M_PI) / (double)npts;
   
   for (i = 0; i < npts; i++) {
      if (flag) {
         theta2 = delta * (double)i;
         theta1 = (delta * (double)i) + (delta / 2.0);
      }
      else {
         theta1 = delta * (double)i;
         theta2 = (delta * (double)i) + (delta / 2.0);
      }
      
      x1[i] = (cos (theta1) * r1) + x0;
      y1[i] = (sin (theta1) * r1) + y0;
      
      x2[i] = (cos (theta2) * r2) + x0;    
      y2[i] = (sin (theta2) * r2) + y0;
   }
   
   moveto (x1[0], y1[0]);
   n1 = 0;
   n2 = incr / 2;
   
   for (i = 0; i < npts; i++) {
      lineto (x1[n1], y1[n1]);
      lineto (x2[n2], y2[n2]);
      
      n1 = (n1 + incr) % npts;
      n2 = (n2 + incr) % npts;
   }
   
   lineto (x1[0], y1[0]);
   
   return (r2);
}


double radials (double x0, double y0, double inner, double length, int n)
{
   int i;
   double theta;
   double delta;
   double xvec, yvec;
   double x1, y1;
   double x2, y2;
   
   delta = (2.0 * M_PI) / (double)n;
   
   for (i = 0; i < n; i++) {
      theta = (double)i * delta;
      xvec = cos (theta);
      yvec = sin (theta);
      
      x1 = xvec * inner;
      y1 = yvec * inner;
      
      x2 = xvec * (inner + length);
      y2 = yvec * (inner + length);
      
      if (i & 1) {
         moveto (x0 + x1, y0 + y1);
         lineto (x0 + x2, y0 + y2);
      }
      else {
         moveto (x0 + x2, y0 + y2);
         lineto (x0 + x1, y0 + y1);
      }
   }
   
   return (inner + length);
}


void ringoboxes (double x0, double y0, double radius, int nboxes, int ninner)
{
   int i, j, k;
   double side, s2;
   double theta;
   double delta;
   double s, c;
   double x[4], y[4];
   double rx[4], ry[4];
   double inc;
   
   side = (2.0 * M_PI * radius) / (double)nboxes;
   side *= 0.8;
   s2 = side / 2.0;
   inc = s2 / (double)ninner;
   
   delta = (2.0 * M_PI) / (double)nboxes;

   for (i = 0; i < nboxes; i++) {
      theta = (double)i * delta;
      s = sin (theta);
      c = cos (theta);
      
      for (k = 0; k < ninner; k++) {
         /* Set up a square */
         x[0] = -s2;
         y[0] = -s2;
         
         x[1] = s2;
         y[1] = -s2;
         
         x[2] = s2;
         y[2] = s2;
         
         x[3] = -s2;
         y[3] = s2;
         
         /* Shrink, rotate and translate square */
         for (j = 0; j < 4; j++) {
            if (x[j] < 0)
               x[j] += k * inc;
            else
               x[j] -= k * inc;

            if (y[j] < 0)
               y[j] += k * inc;
            else
               y[j] -= k * inc;

            rx[j] = (x[j] * c) - (y[j] * s);
            ry[j] = (x[j] * s) + (y[j] * c);
            
            rx[j] += x0 + (c * (radius + s2));
            ry[j] += y0 + (s * (radius + s2));
         }
         
         /* Draw the rotated square */
         moveto (rx[0], ry[0]);
         lineto (rx[1], ry[1]);
         lineto (rx[2], ry[2]);
         lineto (rx[3], ry[3]);
         lineto (rx[0], ry[0]);
      }
   }
}


double ringofcircles (double x0, double y0, double radius, int ncirc)
{
   double delta;
   double theta;
   double r2;
   double x, y;
   int i;
   
   delta = (M_PI * 2.0) / (double)ncirc;
   
   /* Compute radius as if centres are on circumference of 'radius' */
   r2 = radius * sin (delta);
   
   /* Increase radius by radius of small circles */
   radius += r2;
   
   /* Recompute radius of smaller circles */
   r2 = radius * sin (delta);

   for (i = 0; i < ncirc; i++) {
      theta = (double)i * delta;
      
      x = radius * cos (theta);
      y = radius * sin (theta);
      
      circle2 (x0 + x, y0 + y, r2, 10.0);
   }
   
   return (radius + r2);
}


double sqwavering (double x0, double y0, double radius, double len, int nwaves)
{
   double delta;
   double theta;
   double degrees;
   double xvec, yvec;
   double x1, y1;
   double x2, y2;
   int i;
   
   /* Number of half-waves; must be even */
   nwaves *= 2;
   
   delta = (2.0 * M_PI) / (double)nwaves;
   degrees = 360.0 / (double)nwaves;

   for (i = 0; i < nwaves; i++) {
      theta = (double)i * delta;
      xvec = cos (theta);
      yvec = sin (theta);
      
      x1 = xvec * radius;
      y1 = yvec * radius;
      
      x2 = xvec * (radius + len);
      y2 = yvec * (radius + len);
      
      if (i == 0)
         moveto (x0 + x2, y0 + y2);

      if (i & 1)
         lineto (x0 + x2, y0 + y2);
      else
         lineto (x0 + x1, y0 + y1);

      arc (x0, y0, degrees);
   }
   
   return (radius + len);
}


double ellipsering (double x0, double y0, double a, double b, int nell)
{
   double delta;
   int i;
   
   delta = M_PI / (double)nell;

   for (i = 0; i < nell; i++)
      ellipse (x0, y0, a, b, delta * i);
      
   if (a > b)
      return (a);
   else
      return (b);
}


void ellipse (double x0, double y0, double a, double b, double theta)
{
   double t;
   double delta;
   double sintheta, costheta;
   double x, y;
   int npts = 72;
   int i;
   
   delta = (2 * M_PI) / (double)npts;

   sintheta = sin (theta);
   costheta = cos (theta);
   
   for (i = 0; i <= npts; i++) {
      t = (double)i * delta;
      
      x = (a * cos (t) * costheta) - (b * sin (t) * sintheta);
      y = (a * cos (t) * sintheta) + (b * sin (t) * costheta);
      
      if (i == 0)
         moveto (x0 + x, y0 + y);
      else
         lineto (x0 + x, y0 + y);
   }
}
