#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* *** JULIA SET GENERATOR ***
 * generates a ppm image of a
 * 'julia set'. the output
 * resolution and test iterations
 * are specified at compile-time.
 * may attempt to implement colour.
 * calculates -2 <= re <= 1,
 * -1 <= im <= 1
 */

/* initial point, determines which set is generated */
#define DEF_RE -0.3
#define DEF_IM 0.7

/* dimensions of output image, in pixels */
#define H_SIZE 1920
#define V_SIZE 1080

/* number of iterations per pixel */
#define ITERATIONS 250

/* 'upper bound' for 'escape test' */
#define DIFF_MAX 2

void help() {
		/* print help */
		printf("Options:\n");
		printf(" -o<width>,<height> (TBC) Dimensions of Output Image in Pixels\n");
		printf(" -i<real>,<imaginary> (TBC) Initial Value for the Set\n");
}

int main(int argc, char* argv[]) {
	int hsize = 0;
	int vsize = 0;
	double c_re = 0;
	double c_im = 0;

	if (argc > 1) {
		/* parse arguments */
		for (int i = 1; i < argc; i++) {
			if (argv[i][0] == '-') {
				switch(argv[i][1]) {
					case 'o':
						if(!sscanf(argv[i],"-o%d,%d",&hsize,&vsize)) {
							hsize = H_SIZE;
							vsize = V_SIZE;
							printf("Default Output Size\n");
						}
						break;
					case 'i':
						if(!sscanf(argv[i],"-i%lf,%lf",&c_re,&c_im)){
							c_re = DEF_RE;
							c_im = DEF_IM;
							printf("Default Initial Value\n");
						}
						break;
					default:
						help();
						return 0;
				}
			} else {
				help();
				return 0;
			}
		}
	} else {
		/* set default values */
		hsize = H_SIZE;
		vsize = V_SIZE;
		c_re = DEF_RE;
		c_im = DEF_IM;
	}

	/* open a file
	 * create if does not exist, overwrite if does */
	FILE* outfile = fopen("julia.ppm", "w+");

	/* write ppm header */
	fprintf(outfile, "P6 %d %d 255\n", hsize, vsize);

	/* set constraints for output calculations */
	const double max_re = 1.6; // maximum real coordinate
	const double max_im = 1.2; // maximum imaginary coordinate
	const double min_re = -1.6; // minimum real coordinate
	const double min_im = -1.2; // minimum imaginary coordinate
	const double inc_re = (max_re-min_re) / hsize; // real increment
	const double inc_im = (max_im-min_im) / vsize; // imaginary increment
	const double diff_max = DIFF_MAX*DIFF_MAX;

	/* iterate through pixels and generate */
	for (int im = 0; im < vsize; im++) {
		for (int re = 0; re < hsize; re++) {
			int it_count = 0; // count iterations
			double z_im = (im*inc_im)+min_im; // imaginary coordinate
			double z_re = (re*inc_re)+min_re; // real coordinate
			while (it_count <= ITERATIONS && ((z_re*z_re)+(z_im*z_im)) <= diff_max) {
				it_count++;
				/* iterate values */
				double tmp_re = (z_re*z_re)-(z_im*z_im);
				double tmp_im = (2*z_re*z_im);
				z_re = tmp_re + c_re;
				z_im = tmp_im + c_im;
			}
			if (it_count >= ITERATIONS) {
				fprintf(outfile, "%c%c%c", 0, 0, 0);
			} else {
				int r = 255-((ITERATIONS*it_count)%255);
				int g = 255-((ITERATIONS*it_count)%255);
				int b = 255-((ITERATIONS*it_count)%255);
				fprintf(outfile, "%c%c%c", r/2, g/2, b);
			}
		}
	}

	fclose(outfile);
	return 0;
}