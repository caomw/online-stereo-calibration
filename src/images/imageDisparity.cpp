#include <opencv/cv.h>
#include "iCub/images/imageDisparity.h"
#include <iostream>

using namespace std;
using namespace cv;

imageDisparity::imageDisparity(imageDisparity_initial_parameters _imageDisparity_initial_parameters)
{
    numberOfDisparities = _imageDisparity_initial_parameters.numberOfDisparities;
	preFilterCap = _imageDisparity_initial_parameters.preFilterCap;
	SADWindowSize = _imageDisparity_initial_parameters.SADWindowSize;
	P1 = _imageDisparity_initial_parameters.P1;
	P2 = _imageDisparity_initial_parameters.P2;
	minDisparity = _imageDisparity_initial_parameters.minDisparity;
	uniquenessRatio = _imageDisparity_initial_parameters.uniquenessRatio;
	speckleWindowSize = _imageDisparity_initial_parameters.speckleWindowSize;
	speckleRange = _imageDisparity_initial_parameters.speckleRange;
	disp12MaxDiff = _imageDisparity_initial_parameters.disp12MaxDiff;

}

Disparity imageDisparity::Apply(cv::Mat Image_Left, cv::Mat Image_Right){

	//The output containing the 3D points and the disparity image
	Disparity DispResult;

	//Disparity Method
	StereoSGBM SBM;

	SBM.numberOfDisparities = numberOfDisparities;
	SBM.preFilterCap = preFilterCap;
	SBM.SADWindowSize = SADWindowSize;
	SBM.P1 = P1;
	SBM.P2 = P2;
	SBM.minDisparity = minDisparity;
	SBM.uniquenessRatio = uniquenessRatio;
	SBM.speckleWindowSize = speckleWindowSize;
	SBM.speckleRange = speckleRange;
	SBM.disp12MaxDiff = disp12MaxDiff;
	SBM.fullDP = true;

	Mat Disparity;
	SBM(Image_Left, Image_Right, Disparity);

	Mat DisparityValues = Mat::zeros(Disparity.rows,Disparity.cols,CV_64F);

	for (int r=0; r<Disparity.rows; r++){
		for (int c=0; c<Disparity.cols; c++){

            DisparityValues.at<double>(r,c) = -double(Disparity.at<short>(r,c))/16;

		}
	}

	Mat DisparityImage;
	Disparity.convertTo(DisparityImage, CV_8U, 255/(SBM.numberOfDisparities*16.));

	DispResult.DisparityValues = DisparityValues.clone();
	DispResult.DisparityImage  = DisparityImage;//*/

	return DispResult;
}
