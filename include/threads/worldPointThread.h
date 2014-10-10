#ifndef _WORLD_POINT_THREAD_H_
#define _WORLD_POINT_THREAD_H_

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "images/imageDisparity.h"
#include "disparityThread.h"

using namespace std;
using namespace cv;
using namespace yarp::os;
using namespace yarp::sig;

struct worldPointThread_data
{
    RpcServer *rpcPointRequestPort;
    double threadPeriod;
};

class worldPointThread:public RateThread {
protected:

    //Define classes and structures
    disparityThread *_disparityThread;
    worldPointThread_data _worldPointThread_data;

public:
    worldPointThread(disparityThread *_disparityThread_, worldPointThread_data _worldPointThread_data_);

    bool threadInit();

    void threadRelease();

    void run();
};

#endif
