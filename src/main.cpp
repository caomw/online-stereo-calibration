#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "threads/stereoCalibThread.h"
#include "threads/stereoCalibModule.h"

using namespace std;
using namespace cv;
using namespace yarp::os;
using namespace yarp::sig;

int main(int argc, char * argv[])
{

    Network yarp;
    if(! yarp.checkNetwork() ) {
        fprintf(stdout,"Error: yarp server does not seem available\n");
        return -1;
    }

    stereoCalibModule module;

    ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultConfigFile("../conf/stereocalib.ini");
    rf.configure(argc, argv);
    module.runModule(rf);

    return 0;
}
