#ifndef _STEREO_CALIB_THREAD_H_
#define _STEREO_CALIB_THREAD_H_

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "iCub/encoders/encodersBase_icub.h"
#include "iCub/images/imagesBase.h"
#include "iCub/images/imageDisparity.h"
#include "iCub/filter/calibrationStereoCameras.h"
#include "iCub/filter/Kinematics.h"
#include "iCub/features/featuresSIFT.h"

using namespace std;
using namespace cv;
using namespace yarp::os;
using namespace yarp::sig;

struct stereoCalibThread_data
{
    BufferedPort<ImageOf<PixelBgr> > *leftImagePort;
    BufferedPort<ImageOf<PixelBgr> > *rightImagePort;
    BufferedPort<Bottle> *encodersPort;
    double threadPeriod;
    imagesBase_initial_parameters _imagesBase_initial_parameters;
    calibrationStereoCameras_data _calibrationStereoCameras_data;
    imageDisparity_initial_parameters _imageDisparity_initial_parameters;
    double baseline;
};

struct stereoCalibThread_output_data
{
    imagesBase_data _imagesBase_data;
    Mat R_LeftToRight;
    Mat T_LeftToRight;
};

class stereoCalibThread:public RateThread {
protected:

    //Define classes and structures
    stereoCalibThread_data _stereoCalibThread_data;
    encodersBase_icub _encodersBase_icub;
    imagesBase _imagesBase;
    calibrationStereoCameras _calibrationStereoCameras;
    stereoCalibThread_output_data _stereoCalibThread_output_data;

    //Semaphores
    Semaphore mutex;

    int startupContextId;

    //SYstem state and convariance matrix
    Mat X, P;

    //variable to acquire data for the first time
    bool first_data;

    //resize factor to reduce the images dimensions
    double image_resize_factor;

    //all the data contained in the calibrationStereoCameras_data struct
    int minNumberFeatures;
    int maxNumberFeatures;
	int numFixStateParams;
	int numMeasurements;
	double encodersStateNoise;
	double encodersTransitionNoise;
	double featuresMeasurementsNoise;
	double encodersMeasurementsNoise;
	int desiredImageWidth;
	int desiredImageHeight;
	double baseline;

public:
    stereoCalibThread(stereoCalibThread_data _stereoCalibThread_data_);

    bool threadInit();

    void threadRelease();

    void run();

    stereoCalibThread_output_data getData();
};

#endif
