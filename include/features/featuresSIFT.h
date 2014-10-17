#ifndef _FEATURES_SIFT_H_
#define _FEATURES_SIFT_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>

struct Feature{

	//For an image feature
	cv::Point2f Point;
	cv::Mat Descriptor;
	double Quality;

	//For a 3D Feature
	cv::Point3f Position;//x, y, and z
	cv::Point3f Orientation;//theta_x, theta_y and theta_z
	unsigned int FeatureType; //0 - cube; 1 - cylinder; 2 - step; etc...

};

class featuresSIFT
{
	public:

		///Constructor
		featuresSIFT(void);

		/// Acquire NumFeat features in the image Img
		void Apply(const cv::Mat &Img1, const cv::Mat &Img2, std::vector<Feature> &Features1, std::vector<Feature> &Features2, int maximumNumberFeatures, double matchingThreshold);

};

#endif
