#include "encoders/encodersBase_icub.h"

Mat encodersBase_icub::get(Bottle *rawEncoders)
{
     //convert to EncodersVal
    int BottleSize = rawEncoders->size();
    std::vector<double> DataVec;

    for(int i=0; i<BottleSize; i++)
    {
        DataVec.push_back(rawEncoders->get(i).asDouble());
    }

    Mat rawEncoders_mat = Mat(DataVec).clone();
    Mat Encoders = Mat::ones(rawEncoders_mat.rows, 1, CV_64F);

    //set all signs and values right
    double NeckTilt		= rawEncoders_mat.at<double>(0,0);
	double NeckSwing	= rawEncoders_mat.at<double>(1,0);
	double NeckPan		= rawEncoders_mat.at<double>(2,0);
	double EyesTilt		= rawEncoders_mat.at<double>(3,0);
	double LeftEyePan	= (rawEncoders_mat.at<double>(4,0))+rawEncoders_mat.at<double>(5,0)/2;
	double RightEyePan	= (rawEncoders_mat.at<double>(4,0))-rawEncoders_mat.at<double>(5,0)/2;

	Encoders.at<double>(0,0) = NeckTilt*CV_PI/180;
	Encoders.at<double>(1,0) = NeckSwing*CV_PI/180;
	Encoders.at<double>(2,0) = NeckPan*CV_PI/180;
	Encoders.at<double>(3,0) = EyesTilt*CV_PI/180;
	Encoders.at<double>(4,0) = LeftEyePan*CV_PI/180;
	Encoders.at<double>(5,0) = RightEyePan*CV_PI/180;

	return Encoders;
}
