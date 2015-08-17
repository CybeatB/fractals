#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* *** FRACTAL RENDERER ***
 * Generates a .ppm image of
 * a fractal. So far, generators
 * exist for Mandelbrot and Julia
 * sets of an arbitrary degree.
 * Note, however, that higher
 * degrees require much more
 * computation time. */

#define MAX_RE 2.0
#define MIN_RE -2.0
#define MAX_IM 2.0
#define MIN_IM -2.0
#define ITERATIONS 250
#define DIFF_MAX 2

void help() {
  printf("Usage: fractal -o=w,h [-d=d] [-j=x,y]\n");
  printf(" -o: Dimensions of the Output Image (w = width, h = height)\n");
  printf(" -d: 'Order' of the Mandelbrot/Julia Set (d must be >= 2, defaults to 2)\n");
  printf(" -j: Generate a Julia Set with the Given Initial Coordinates\n");
  printf("     If this option is not specified, generates a Mandelbrot Set\n");
  printf("Examples:\n");
  printf(" fractal -o=1920,1080\n");
  printf(" - Generates a 1920x1080 image of a Mandelbrot Set.\n");
  printf(" fractal -o=500,500 -j=-0.3,0.7\n");
  printf(" - Generates a 500x500 image of the degree-2 Julia Set [-0.3, 0.7]\n");
  printf(" fractal -o=500,500 -d=4\n");
  printf(" - Generates a 500x500 image of the degree-4 Mandelbrot Set\n");
}

struct Color {
  int r;
  int g;
  int b;
}

struct Coord {
  double re;
  double im;
}

struct Color bezier_colcurve(double p, struct Color start, struct Color ctl, struct Color end) {
  struct Color result = {0,0,0};
  if (p <= 0) {
    return result;
  }
  result.r = (int)((1.0-p)*(1.0-p)*start.r + 2.0*(1.0-p)*p*ctl.r + p*p*end.r);
  result.g = (int)((1.0-p)*(1.0-p)*start.g + 2.0*(1.0-p)*p*ctl.g + p*p*end.g);
  result.b = (int)((1.0-p)*(1.0-p)*start.b + 2.0*(1.0-p)*p*ctl.b + p*p*end.b);
  return result;
}

struct Coord degree(struct Coord z, unsigned int deg) {
  struct Coord result = {0.0,0.0};
  for (unsigned int i = 0; i < deg; i++) {
    result.re = (z.re * z.re) - (z.im * z.im);
    result.im = (2.0 * z.re * z.im);
  }
  return result;
}

double julia(int deg, struct Coord z, struct Coord c, int w, int h, FILE* outfile) {
  const double inc_re = (MAX_RE - MIN_RE) / w;
  const double inc_im = (MAX_IM - MIN_IM) / h;
  unsigned int it_count = 0;
  while (it_count <= ITERATIONS && ((z_re * z_re)*(z_im * z_im)) <= (DIFF_MAX * DIFF_MAX)) {
    it_count++;
    z = degree(z, deg);
    z.re += c.re;
    z.im += c.im;
  }
  if (it_count >= ITERATIONS) {
    return -1.0;
  }
  return fabs(fmod((it_count - log((z.re * z.re)+(z.im * z.im))) / ITERATIONS, 1));
}

int main(int argc, char *argv[]) {
  // TODO: Parse Command-Line Args
  for (
  return 0;
}
