#include <stdio.h>
#include <gd.h>
#include <err.h>
#include <omp.h>
int main(int argc, char *argv[]) {
	int nt = 0;
	int tid, tmp, red, green, blue, color, x, h, y, w;
	tmp = red = green = blue = color = x = h = y = w = 0;
	char *iname  = NULL;
	char *oname = NULL;
	gdImagePtr img;
	FILE *fp = {0};

	if(argc != 3) {
		printf("Error: format : object_file input.png output.png");
		exit(-1);
	} else {
		iname = argv[1];
		oname = argv[2];
	}

	if((fp = fopen(iname, "r")) == NULL) {
		printf("Error : fopen : %s", iname);
		exit(-1);
	} else {
		img = gdImageCreateFromPng(fp);
		w = gdImageSX(img);
		h = gdImageSY(img);
	}
	printf("Enter number of threads: ");
	scanf("%d",  &nt);

	double t = omp_get_wtime();

	#pragma omp parallel for private(y, color, red, blue, green) schedule(dynamic, 100) num_threads(nt)/*schedule(dynamic, 50) schedule(guided, 50)*/
	for(x = 0;x < w;x++)
		for(y = 0;y < h;y++) {
			tid =  omp_get_thread_num();

			color = gdImageGetPixel(img, x, y);
			red = gdImageRed(img, color);
			green = gdImageGreen(img, color);
			blue = gdImageBlue(img, color);
			tmp = (red+green+blue)/3;
			red = green = blue = tmp;

			//color = gdImageColorAllocate(img, tmp, tmp, tmp);
			//gdImageSetPixel(img, x, y, color);
			if(tid == 0) {
				color = gdImageColorAllocate(img, red, 0, 0);
				gdImageSetPixel(img, x, y, color);
			}
			if(tid == 1) {
				color = gdImageColorAllocate(img, 0, green, 0);
				gdImageSetPixel(img, x, y, color);
			}
			if(tid == 2) {
				color = gdImageColorAllocate(img, 0, 0, blue);
				gdImageSetPixel(img, x, y, color);
			}
			if(tid == 3) {
				color = gdImageColorAllocate(img, 0, 0, 0);
				gdImageSetPixel(img, x, y, color);
			}
		}
	t = omp_get_wtime() - t;
	printf("Image size: (%d, %d)\n", w, h);
	printf("\ntime taken : %lf threads : %d\n", t, nt);
	if((fp = fopen(oname, "w")) == NULL) {
		printf("error : fopen : %s", oname);
		exit(-1);
	} else {
		gdImagePng(img, fp);
		fclose(fp);
	}
	gdImageDestroy(img);
	return 0;
}
