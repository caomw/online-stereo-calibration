#include "iCub/images/imagesBase.h"

ProjectiveCamera::ProjectiveCamera(unsigned int resx, unsigned int resy, double fx, double fy, double cx, double cy, double k1, double k2, double k3, double k4, double k5)
{
	res.width  = resx;
	res.height = resy;

	intrinsics = Mat::zeros(3,3,CV_64F);
	intrinsics.at<double>(0,0) = fx;
	intrinsics.at<double>(1,1) = fy;
	intrinsics.at<double>(0,2) = cx;
	intrinsics.at<double>(1,2) = cy;
	intrinsics.at<double>(2,2) = 1;

	distortion = Mat::zeros(5,1,CV_64F);
	distortion.at<double>(0,0)=k1;
	distortion.at<double>(1,0)=k2;
	distortion.at<double>(2,0)=k3;
	distortion.at<double>(3,0)=k4;
	distortion.at<double>(4,0)=k5;
}

imagesBase::imagesBase(imagesBase_initial_parameters _imagesBase_initial_parameters_):
projectiveCameraLeft(_imagesBase_initial_parameters_.left_resx, _imagesBase_initial_parameters_.left_resy, _imagesBase_initial_parameters_.left_fx, _imagesBase_initial_parameters_.left_fy,
                     _imagesBase_initial_parameters_.left_cx, _imagesBase_initial_parameters_.left_cy, _imagesBase_initial_parameters_.left_k1, _imagesBase_initial_parameters_.left_k2,
                     _imagesBase_initial_parameters_.left_k3, _imagesBase_initial_parameters_.left_k4, _imagesBase_initial_parameters_.left_k5),
projectiveCameraRight(_imagesBase_initial_parameters_.right_resx, _imagesBase_initial_parameters_.right_resy, _imagesBase_initial_parameters_.right_fx, _imagesBase_initial_parameters_.right_fy,
                     _imagesBase_initial_parameters_.right_cx, _imagesBase_initial_parameters_.right_cy, _imagesBase_initial_parameters_.right_k1, _imagesBase_initial_parameters_.right_k2,
                     _imagesBase_initial_parameters_.right_k3, _imagesBase_initial_parameters_.right_k4, _imagesBase_initial_parameters_.right_k5)
{
    _imagesBase_initial_parameters = _imagesBase_initial_parameters_;

    Mat P1_left = cv::getOptimalNewCameraMatrix(projectiveCameraLeft.intrinsics, projectiveCameraLeft.distortion, projectiveCameraLeft.res, 0.0);

	cv::initUndistortRectifyMap(projectiveCameraLeft.intrinsics, projectiveCameraLeft.distortion, cv::Mat(), P1_left, projectiveCameraLeft.res, CV_16SC2, rectificationMap1_Left, rectificationMap2_Left);
	_imagesBase_data.calibMatLeft = P1_left;

    Mat P1_right = cv::getOptimalNewCameraMatrix(projectiveCameraRight.intrinsics, projectiveCameraRight.distortion, projectiveCameraRight.res, 0.0);

	cv::initUndistortRectifyMap(projectiveCameraRight.intrinsics, projectiveCameraRight.distortion, cv::Mat(), P1_right, projectiveCameraRight.res, CV_16SC2, rectificationMap1_Right, rectificationMap2_Right);
    _imagesBase_data.calibMatRight = P1_right;
}
imagesBase_data imagesBase::get(ImageOf<PixelBgr> *rawLeftImage, ImageOf<PixelBgr> *rawRightImage)
{
    //left image
    IplImage *Iplimage_left;
	Iplimage_left = (IplImage*)rawLeftImage->getIplImage();
	_imagesBase_data.originalLeftImage = Mat(Iplimage_left);

	//right image
    IplImage *Iplimage_right;
	Iplimage_right = (IplImage*)rawRightImage->getIplImage();
	_imagesBase_data.originalRightImage = Mat(Iplimage_right);

	//Rectify the images
	imagesBase::rectify(_imagesBase_data);

	return _imagesBase_data;
};

void imagesBase::rectify(imagesBase_data &_imagesBase_data_)
{
    cv::remap(_imagesBase_data_.originalLeftImage, _imagesBase_data_.rectifiedLeftImage, rectificationMap1_Left, rectificationMap2_Left, cv::INTER_LINEAR);
    cv::remap(_imagesBase_data_.originalRightImage, _imagesBase_data_.rectifiedRightImage, rectificationMap1_Right, rectificationMap2_Right, cv::INTER_LINEAR);
};

void imagesBase::stereoRectify(imagesBase_data &_imagesBase_data_, Mat R, Mat T)
{

    cv::Mat R1(3,3,CV_64F), R2(3,3,CV_64F);
	cv::stereoRectify(_imagesBase_data_.calibMatLeft, Mat::zeros(5,1,CV_64F), _imagesBase_data_.calibMatRight, Mat::zeros(5,1,CV_64F),
					  Size(_imagesBase_data_.rectifiedLeftImage.cols, _imagesBase_data_.rectifiedLeftImage.rows), R, T, R1, R2, _imagesBase_data_.stereoCalibMatLeft,
                   _imagesBase_data_.stereoCalibMatRight, _imagesBase_data_.Q, CV_CALIB_ZERO_DISPARITY,0,
                   Size(_imagesBase_data_.rectifiedLeftImage.cols, _imagesBase_data_.rectifiedLeftImage.rows));

	cv::initUndistortRectifyMap(_imagesBase_data_.calibMatLeft, Mat::zeros(5,1,CV_64F), R1, _imagesBase_data_.stereoCalibMatLeft,
                             Size(_imagesBase_data_.rectifiedLeftImage.cols, _imagesBase_data_.rectifiedLeftImage.rows), CV_16SC2, stereoRectificationMap1_Left, stereoRectificationMap2_Left);
	cv::initUndistortRectifyMap(_imagesBase_data_.calibMatRight, Mat::zeros(5,1,CV_64F), R2, _imagesBase_data_.stereoCalibMatRight,
                             Size(_imagesBase_data_.rectifiedRightImage.cols, _imagesBase_data_.rectifiedRightImage.rows), CV_16SC2, stereoRectificationMap1_Right, stereoRectificationMap2_Right);

	cv::remap(_imagesBase_data_.rectifiedLeftImage, _imagesBase_data_.stereoRectifiedLeftImage, stereoRectificationMap1_Left, stereoRectificationMap2_Left, cv::INTER_LINEAR);
	cv::remap(_imagesBase_data_.rectifiedRightImage, _imagesBase_data_.stereoRectifiedRightImage, stereoRectificationMap1_Right, stereoRectificationMap2_Right, cv::INTER_LINEAR);
}
