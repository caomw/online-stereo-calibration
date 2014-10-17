#include "threads/disparityThread.h"

disparityThread::disparityThread(stereoCalibThread *_stereoCalibThread_, disparityThread_data _disparityThread_data_):RateThread(int(_disparityThread_data_.threadPeriod*1000.0)),
_imageDisparity(_disparityThread_data_._imageDisparity_initial_parameters), _imagesBase(_disparityThread_data_._imagesBase_initial_parameters)
       {
            _stereoCalibThread = _stereoCalibThread_;
            _disparityThread_data = _disparityThread_data_;
       }

bool disparityThread::threadInit()
{
    return true;
}

void disparityThread::threadRelease()
{

}

void disparityThread::run()
{
    stereoCalibThread_output_data _stereoCalibThread_output_data = _stereoCalibThread->getData();

    if(_stereoCalibThread_output_data.R_LeftToRight.data && _stereoCalibThread_output_data.T_LeftToRight.data)
    {
        Disparity Disp;

        _imagesBase.stereoRectify(_stereoCalibThread_output_data._imagesBase_data, _stereoCalibThread_output_data.R_LeftToRight, _stereoCalibThread_output_data.T_LeftToRight);
        Mat Q = _stereoCalibThread_output_data._imagesBase_data.Q;

        Disp = _imageDisparity.Apply(_stereoCalibThread_output_data._imagesBase_data.stereoRectifiedLeftImage, _stereoCalibThread_output_data._imagesBase_data.stereoRectifiedRightImage);

        //save data to be sent out
        mutex.wait();
            _disparityThread_output_data.disparityValues = Disp.DisparityValues;
            _disparityThread_output_data.Q = Q;
            _disparityThread_output_data.Tr_LeftCamToLeftPan = _stereoCalibThread_output_data.Tr_LeftCamToLeftPan;
            _disparityThread_output_data.Tr_RightCamToRightPan = _stereoCalibThread_output_data.Tr_RightCamToRightPan;
        mutex.post();

        //send images to the ports
        ImageOf<PixelBgr> leftImageOut;
        ImageOf<PixelBgr> rightImageOut;
        ImageOf<PixelMono> disparityImageOut;

        IplImage il(_stereoCalibThread_output_data._imagesBase_data.stereoRectifiedLeftImage);
        IplImage *IL = &(il);

        IplImage ir(_stereoCalibThread_output_data._imagesBase_data.stereoRectifiedRightImage);
        IplImage *IR = &(ir);

        IplImage id(Disp.DisparityImage);
        IplImage *ID = &(id);

        leftImageOut.wrapIplImage(IL);
        rightImageOut.wrapIplImage(IR);
        disparityImageOut.wrapIplImage(ID);

        _disparityThread_data.leftImageOutPort->prepare() = leftImageOut;
        _disparityThread_data.leftImageOutPort->write();

        _disparityThread_data.rightImageOutPort->prepare() = rightImageOut;
        _disparityThread_data.rightImageOutPort->write();

        _disparityThread_data.disparityImageOutPort->prepare() = disparityImageOut;
        _disparityThread_data.disparityImageOutPort->write();//*/
    }
}

disparityThread_output_data disparityThread::getData()
{
    disparityThread_output_data _disparityThread_output_data_aux;
    mutex.wait();
        _disparityThread_output_data_aux.disparityValues = _disparityThread_output_data.disparityValues;
        _disparityThread_output_data_aux.Q = _disparityThread_output_data.Q;
        _disparityThread_output_data_aux.Tr_LeftCamToLeftPan = _disparityThread_output_data.Tr_LeftCamToLeftPan;
        _disparityThread_output_data_aux.Tr_RightCamToRightPan = _disparityThread_output_data.Tr_RightCamToRightPan;
    mutex.post();

    return _disparityThread_output_data_aux;
}
