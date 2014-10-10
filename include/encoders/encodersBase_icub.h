#ifndef _ENCODERS_BASE_ICUB_H_
#define _ENCODERS_BASE_ICUB_H_

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "encoders/encodersBase.h"

using namespace std;
using namespace cv;
using namespace yarp::os;
using namespace yarp::sig;

class encodersBase_icub : public encodersBase
{
    public:
        Mat get(Bottle *rawEncoders);
};

#endif
