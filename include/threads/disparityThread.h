#ifndef _DISPARITY_THREAD_H_
#define _DISPARITY_THREAD_H_

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "images/imageDisparity.h"
#include "stereoCalibThread.h"

using namespace std;
using namespace cv;
using namespace yarp::os;
using namespace yarp::sig;

struct disparityThread_data
{
    double threadPeriod;
    imageDisparity_initial_parameters _imageDisparity_initial_parameters;
    imagesBase_initial_parameters _imagesBase_initial_parameters;
    BufferedPort<ImageOf<PixelBgr> > *leftImageOutPort;
    BufferedPort<ImageOf<PixelBgr> > *rightImageOutPort;
    BufferedPort<ImageOf<PixelMono> > *disparityImageOutPort;
};

struct disparityThread_output_data
{
    Mat disparityValues;
    Mat Q;
};

class disparityThread:public RateThread {
protected:

    //Define classes and structures
    stereoCalibThread *_stereoCalibThread;
    imageDisparity _imageDisparity;
    imagesBase _imagesBase;
    disparityThread_output_data _disparityThread_output_data;
    disparityThread_data _disparityThread_data;

    //Semaphores
    Semaphore mutex;

    int startupContextId;

public:
    disparityThread(stereoCalibThread *_stereoCalibThread_, disparityThread_data _disparityThread_data_);

    bool threadInit();

    void threadRelease();

    void run();

    disparityThread_output_data getData();

};

#endif
