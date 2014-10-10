#ifndef _IMAGEDISPARITY_H_
#define _IMAGEDISPARITY_H_

#include <opencv/cv.h>

struct Disparity{

	cv::Mat DisparityValues;
	cv::Mat DisparityImage;
};

struct imageDisparity_initial_parameters{

    int numberOfDisparities;
	int preFilterCap;
	int SADWindowSize;
	int P1;
	int P2;
	int minDisparity;
	int uniquenessRatio;
	int speckleWindowSize;
	int speckleRange;
	int disp12MaxDiff;
};

class imageDisparity
{

    int numberOfDisparities;
	int preFilterCap;
	int SADWindowSize;
	int P1;
	int P2;
	int minDisparity;
	int uniquenessRatio;
	int speckleWindowSize;
	int speckleRange;
	int disp12MaxDiff;
	bool fullDP;

public:

    imageDisparity(imageDisparity_initial_parameters _imageDisparity_initial_parameters);
	Disparity Apply(cv::Mat Image_Left, cv::Mat Image_Right);
};


#endif
