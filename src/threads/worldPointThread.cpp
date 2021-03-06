#include "threads/worldPointThread.h"

worldPointThread::worldPointThread(disparityThread *_disparityThread_, worldPointThread_data _worldPointThread_data_):RateThread(int(_worldPointThread_data_.threadPeriod*1000.0))
{
    _disparityThread = _disparityThread_;
    _worldPointThread_data = _worldPointThread_data_;
}

bool worldPointThread::threadInit()
{
    return true;
}

void worldPointThread::threadRelease()
{

}

void worldPointThread::run()
{
    //RPC TEST
    Bottle cmd;
    Bottle response;
    _worldPointThread_data.rpcPointRequestPort->read(cmd,true);

    disparityThread_output_data _disparityThread_output_data = _disparityThread->getData();
    Mat Q = _disparityThread_output_data.Q;
    Mat disparityValues = _disparityThread_output_data.disparityValues;
    Mat Tr_LeftCamToLeftPan = _disparityThread_output_data.Tr_LeftCamToLeftPan;
    Mat Tr_RightCamToRightPan = _disparityThread_output_data.Tr_RightCamToRightPan;

    //get the coordinates from the rpc port
    int BottleSize = cmd.size();
    std::vector<double> DataVec;

    if(BottleSize == 2) //point from left image only
    {
        for(int i=0; i<BottleSize; i++)
        {
            DataVec.push_back(cmd.get(i).asDouble());
        }

        Mat rawPoint = Mat(DataVec).clone();
        double x = rawPoint.at<double>(0,0);
        double y = rawPoint.at<double>(1,0);

        Mat Point = Mat::ones(4,1,CV_64F);
        Point.at<double>(0,0) = x;
        Point.at<double>(1,0) = y;
        Point.at<double>(2,0) = -disparityValues.at<double>(y,x);

        Mat triPoint = Q*Point;
        triPoint = triPoint/triPoint.at<double>(3,0);

        Mat leftPan_triPoint = Tr_LeftCamToLeftPan*triPoint;

        if(isnan(leftPan_triPoint.at<double>(0,0)) || isnan(leftPan_triPoint.at<double>(1,0)) || isnan(leftPan_triPoint.at<double>(2,0)))
        {
            response.addDouble(0.0);
            response.addDouble(0.0);
            response.addDouble(0.0);
        }
        else
        {
            response.addDouble(leftPan_triPoint.at<double>(0,0));
            response.addDouble(leftPan_triPoint.at<double>(1,0));
            response.addDouble(leftPan_triPoint.at<double>(2,0));
        }

        _worldPointThread_data.rpcPointRequestPort->reply(response);//*/
    }
}
