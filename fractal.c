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

// Structures
struct Color {
  int r;
  int g;
  int b;
};
struct Coord {
  double re;
  double im;
};

// Constants
#define ITERATIONS 250
#define DIFF_MAX 2

// Print Usage Information
void help() {
  printf("Usage: fractal -o=w,h [-m=x,y[,a,b]] [-d=d] [-j=x,y]\n");
  printf(" -o: Dimensions of the Output Image (w = width, h = height)\n");
  printf(" -m: Complex Boundaries ([x,y] = maximum, [a,b] = minimum)\n");
  printf("     If this option is not specified, defaults to 2,2,-2,-2.\n");
  printf("     If only [x,y] is specified, a = -x and b = -y\n");
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

// Use a Bezier Curve to Generate a Pixel Colour
struct Color bezier_colcurve(double p, struct Color start, struct Color ctl, struct Color end) {
  struct Color result = {0,0,0};
  if (p < 0) {
    return result;
  }
  result.r = (int)((1.0-p)*(1.0-p)*(double)start.r + 2.0*(1.0-p)*p*(double)ctl.r + p*p*(double)end.r);
  result.g = (int)((1.0-p)*(1.0-p)*(double)start.g + 2.0*(1.0-p)*p*(double)ctl.g + p*p*(double)end.g);
  result.b = (int)((1.0-p)*(1.0-p)*(double)start.b + 2.0*(1.0-p)*p*(double)ctl.b + p*p*(double)end.b);
  return result;
}

// Raise a Complex Number to a Power
struct Coord degree(struct Coord z, unsigned int deg) {
  struct Coord result = {1.0,0.0};
  if (deg == 0) {
    return result;
  }
  result.re = z.re;
  result.im = z.im;
  for (unsigned int i = 1; i < deg; i++) {
    struct Coord temp = {0,0};
    temp.re = (result.re * z.re) - (result.im * z.im);
    temp.im = (result.re * z.im) + (result.im * z.re);
    result = temp;
  }
  return result;
}

// Calculate the Colouring Parameter for the Specified Pixel (Julia Set)
double julia(int deg, struct Coord z, struct Coord c) {
  unsigned int it_count = 0;
  while (it_count <= ITERATIONS && ((z.re * z.re)+(z.im * z.im)) <= (DIFF_MAX * DIFF_MAX)) {
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

double mandelbrot(int deg, struct Coord z, struct Coord c) {
  unsigned int it_count = 0;
  c = z;
  while (it_count <= ITERATIONS && ((z.re * z.re)+(z.im * z.im)) <= (DIFF_MAX * DIFF_MAX)) {
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

// Parse Args & Write File
int main(int argc, char *argv[]) {
  // Variables
  unsigned int width = 0;
  unsigned int height = 0;
  unsigned int degree = 2; // default is 2, lower degrees are boring
  double (*fractal)(int, struct Coord, struct Coord) = mandelbrot;
  struct Coord c = {0.0,0.0};
  struct Coord max = {0.0,0.0};
  struct Coord min = {0.0,0.0};

  if (argc <= 1) {
    help();
    return 0;
  }

  // Parse Command-Line Args
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      continue;
    }
    switch(argv[i][1]) {
    case 'o':
      if (!sscanf(argv[i],"-o=%u,%u",&width,&height)) {
        printf(" ! Output Image Dimension Error\n");
        return 1;
      }
      break;
    case 'd':
      if (!sscanf(argv[i], "-d=%u",&degree) || degree < 2) {
        printf(" ! Degree Must Be 2 or Greater\n");
        return 1;
      }
      break;
    case 'j':
      if (!sscanf(argv[i], "-j=%lf,%lf",&(c.re),&(c.im))) {
        printf(" ! Could Not Set Julia Set Coordinates\n");
        return 1;
      }
      fractal = julia;
      break;
    case 'm':
      {int args = sscanf(argv[i], "-m=%lf,%lf,%lf,%lf",&(max.re),&(max.im),&(min.re),&(min.im));
      if (args != 2 && args != 4) {
        printf(" ! Boundary Format Incorrect\n");
        return 1;
      }
      if (args == 2) {
        min.re = 0.0 - max.re;
        min.im = 0.0 - max.im;
      }}
      break;
    case 'h':
      help();
      return 0;
    }
  }

  // Open File & Write Header
  FILE* outfile = fopen("fractal.ppm", "w+");
  fprintf(outfile, "P6 %d %d 255\n", width, height);

  // Determine Increment
  double const inc_re = (max.re - min.re) / width;
  double const inc_im = (max.im - min.im) / height;

  // Write Pixel Values
  for (unsigned int x = 0; x < width; x++) {
    for (unsigned int y = 0; y < height; y++) {
      // Calculate Complex Coordinate & Pixel Colour
      struct Coord z = {0,0};
      z.re = (y * inc_re) + min.re;
      z.im = (x * inc_im) + min.im;
      double p = (*fractal)(degree, z, c);
      struct Color s={0,0,0},c={0,127,255},e={255,255,255};
      struct Color pix = bezier_colcurve(p, s, c, e);
      fprintf(outfile, "%c%c%c", pix.r, pix.g, pix.b);
    }
  }

  fclose(outfile);
  return 0;
}

