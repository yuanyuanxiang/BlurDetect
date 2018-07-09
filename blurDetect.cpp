#include "blurDetect.h"

static int X_abs[512];


void calcuXabs()
{
	for (int i = 0; i < 256; ++i)
	{
		// x = -255, -254, -253, ..., 0
		X_abs[i] = 255 - i;
		// x = 0, 1, 2, ..., 255
		X_abs[255 + i] = i;
	}
}


double BlurDetect(const Mat& img_gray)
{
	const int w_s = 1; // 宽度开始点
	const int h_s = 1; // 高度开始点
	const int w_b = img_gray.cols - 1; // 宽度结束点
	const int h_b = img_gray.rows - 1; // 高度结束点

	const int width = img_gray.cols;
	const int height = img_gray.rows;
	const int rowlen = img_gray.step[0];
	const int w_e = w_s + w_b >= width  ? width -  1 : w_s + w_b;
	const int h_e = h_s + h_b >= height ? height - 1 : h_s + h_b;
	const uchar* pst1 = img_gray.data + (h_s-1) * rowlen;
	const uchar* pst2 = img_gray.data + h_s     * rowlen;
	const uchar* pst3 = img_gray.data + (h_s+1) * rowlen;

	int sum_all = 0;
	for (int i = h_s; i < h_e; i += 2, pst1 += 2*rowlen, pst2 += 2*rowlen, pst3 += 2*rowlen)
	{
		for (int j = w_s; j < w_e; j += 2)
		{
			// 3行 e6 e7 e8
			// 2行 e4 e0 e5
			// 1行 e1 e2 e3
			const uchar t = 255 -  *(pst2+j); //中心
			sum_all += X_abs[*(pst1+j-1) + t] + X_abs[*(pst1+j)	 + t]	+ X_abs[*(pst1+j+1) + t];
			sum_all += X_abs[*(pst2+j-1) + t]							+ X_abs[*(pst2+j+1) + t];
			sum_all += X_abs[*(pst3+j-1) + t] + X_abs[*(pst3+j)	 + t]	+ X_abs[*(pst3+j+1) + t];
		}
	}
	int size = 2 * (h_e-h_s) * (w_e-w_s);
	double ret = (double)sum_all / size;

	return ret;
}
