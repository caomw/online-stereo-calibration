#ifndef _STEREO_CALIB_MODULE_H_
#define _STEREO_CALIB_MODULE_H_

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "stereoCalibThread.h"
#include "disparityThread.h"
#include "worldPointThread.h"
#include "images/imagesBase.h"

using namespace std;
using namespace cv;
using namespace yarp::os;
using namespace yarp::sig;

class stereoCalibModule: public RFModule {
    string moduleName;

    string leftImagePortName;
    string rightImagePortName;
    string encodersPortName;
    string rpcPointRequestPortName;
    string leftImageOutPortName;
    string rightImageOutPortName;
    string disparityImageOutPortName;

    BufferedPort<ImageOf<PixelBgr> > leftImagePort;
    BufferedPort<ImageOf<PixelBgr> > rightImagePort;
    BufferedPort<Bottle> encodersPort;
    RpcServer rpcPointRequestPort;
    BufferedPort<ImageOf<PixelBgr> > leftImageOutPort;
    BufferedPort<ImageOf<PixelBgr> > rightImageOutPort;
    BufferedPort<ImageOf<PixelMono> > disparityImageOutPort;

    double threadPeriod;

    stereoCalibThread *_stereoCalibThread;
    stereoCalibThread_data _stereoCalibThread_data;

    disparityThread *_disparityThread;
    disparityThread_data _disparityThread_data;

    worldPointThread *_worldPointThread;
    worldPointThread_data _worldPointThread_data;

public:
    double getPeriod();
    bool configure(yarp::os::ResourceFinder &rf);
    bool updateModule();
    bool interruptModule();
    bool close();
};

#endif
