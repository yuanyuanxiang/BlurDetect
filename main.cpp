#include "blurDetect.h"

int main(int argc, const char *argv[])
{
	calcuXabs();
	const char *path = 1 == argc ? "image.jpg" : argv[1];
	Mat m;
	try {
		m = imread(path);
	} catch (...){ return -1; }

	double s = BlurDetect(m);
	printf("\"%s\" BlurDetect result: %f\n", path, s);
	s = ImagePsnr(m);
	printf("\"%s\" ImagePsnr result: %f\n", path, s);

	return 0;
}
