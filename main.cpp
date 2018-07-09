#include "blurDetect.h"

int main(int argc, const char *argv[])
{
	calcuXabs();
	const char *path = 1 == argc ? "image.jpg" : argv[1];
	Mat m;
	try {
		m = imread(path);
	} catch (...){ return -1; }
	if (1 != m.channels())
		cvtColor(m, m, CV_RGB2GRAY);
	double s = BlurDetect(m);
	printf("\"%s\" BlurDetect result: %f\n", path, s);

	return 0;
}
