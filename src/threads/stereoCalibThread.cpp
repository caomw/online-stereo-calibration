#include "threads/stereoCalibThread.h"

stereoCalibThread::stereoCalibThread(stereoCalibThread_data _stereoCalibThread_data_):RateThread(int(_stereoCalibThread_data_.threadPeriod*1000.0)),
_imagesBase(_stereoCalibThread_data_._imagesBase_initial_parameters)
       {
            _stereoCalibThread_data = _stereoCalibThread_data_;
       }

bool stereoCalibThread::threadInit()
{
    //first data will be acquired
    first_data = true;

    //pass the calibrationStereoCameras_data values to the class variables
    minNumberFeatures = _stereoCalibThread_data._calibrationStereoCameras_data.minNumberFeatures;
    maxNumberFeatures = _stereoCalibThread_data._calibrationStereoCameras_data.maxNumberFeatures;
	numFixStateParams = _stereoCalibThread_data._calibrationStereoCameras_data.numFixStateParams;
	numMeasurements = _stereoCalibThread_data._calibrationStereoCameras_data.numMeasurements;
	encodersStateNoise = _stereoCalibThread_data._calibrationStereoCameras_data.encodersStateNoise;
	encodersTransitionNoise = _stereoCalibThread_data._calibrationStereoCameras_data.encodersTransitionNoise;
	featuresMeasurementsNoise = _stereoCalibThread_data._calibrationStereoCameras_data.featuresMeasurementsNoise;
	encodersMeasurementsNoise = _stereoCalibThread_data._calibrationStereoCameras_data.encodersMeasurementsNoise;
	desiredImageWidth = _stereoCalibThread_data._calibrationStereoCameras_data.desiredImageWidth;
	desiredImageHeight = _stereoCalibThread_data._calibrationStereoCameras_data.desiredImageHeight;
	baseline = _stereoCalibThread_data.baseline;

    //Initialize the filter
    _calibrationStereoCameras.Num_Fix_State_Params = numFixStateParams;
    _calibrationStereoCameras.NumEncodersMeasurements = numMeasurements;

    X = Mat::zeros(numFixStateParams,1,CV_64F);

    P = Mat::eye(numFixStateParams,numFixStateParams,CV_64F)*encodersStateNoise*encodersStateNoise;

    _calibrationStereoCameras.P_k = P;

    _calibrationStereoCameras.U_k = Mat::zeros(1,1,CV_64F);

    _calibrationStereoCameras.Pn = Mat::zeros(1,1,CV_64F);

    _calibrationStereoCameras.Q = Mat::eye(numFixStateParams,numFixStateParams,CV_64F)*encodersTransitionNoise*encodersTransitionNoise;
}

void stereoCalibThread::threadRelease()
{

}

void stereoCalibThread::run()
{
	ImageOf<PixelBgr> *yarpLeftImage = _stereoCalibThread_data.leftImagePort->read();
	ImageOf<PixelBgr> *yarpRightImage = _stereoCalibThread_data.rightImagePort->read();
	Bottle *yarpEncoders = _stereoCalibThread_data.encodersPort->read();

	if (yarpLeftImage!=NULL && yarpRightImage!=NULL && yarpEncoders!=NULL)
	{
	    _calibrationStereoCameras.Flag_Cameras_Measurements = false;
        _calibrationStereoCameras.Flag_Encoders_Measurements = false;

        //get images and encoders
        imagesBase_data _imagesBase_data = _imagesBase.get(yarpLeftImage, yarpRightImage);
        Mat _encoders = _encodersBase_icub.get(yarpEncoders);

        //get the encoders values and use only those from the eyes - all the virtual joints will have zero values
        Mat encoders = Mat::zeros(numMeasurements,1,CV_64F);
        encoders.at<double>(0,0) = _encoders.at<double>(4,0);
        encoders.at<double>(1,0) = _encoders.at<double>(5,0);

        //get the first data and initialize the system state
        if(first_data)
        {
            X.at<double>(0,0) = encoders.at<double>(0,0);
            X.at<double>(1,0) = encoders.at<double>(1,0);
            _calibrationStereoCameras.X_k = X.clone();

            image_resize_factor = (double)_imagesBase_data.originalLeftImage.cols/(double)desiredImageWidth;
            _calibrationStereoCameras.LeftCalibMat = _imagesBase_data.calibMatLeft/image_resize_factor;
            _calibrationStereoCameras.LeftCalibMat.at<double>(2,2) = 1;

            _calibrationStereoCameras.RightCalibMat = _imagesBase_data.calibMatRight/image_resize_factor;
            _calibrationStereoCameras.RightCalibMat.at<double>(2,2) = 1;

            first_data = false;
        }

        //resize the images
        Mat LeftImage, RightImage;
        resize(_imagesBase_data.rectifiedLeftImage, LeftImage, Size(desiredImageWidth,desiredImageHeight));
        resize(_imagesBase_data.rectifiedRightImage, RightImage, Size(desiredImageWidth,desiredImageHeight));

        //obtain the SIFT features
        featuresSIFT _featuresSIFT;
        std::vector<Feature> FeaturesLeft;
        std::vector<Feature> FeaturesRight;
        vector<Point2f> FeaturesLeftPt;
        vector<Point2f> FeaturesRightPt;

        _featuresSIFT.Apply(LeftImage, RightImage, FeaturesLeft, FeaturesRight, maxNumberFeatures);

        for(int k=0; k<FeaturesLeft.size(); k++)
        {
            FeaturesLeftPt.push_back(FeaturesLeft[k].Point);
            FeaturesRightPt.push_back(FeaturesRight[k].Point);
        }

        _calibrationStereoCameras.NumPoints = FeaturesLeft.size();

        if(_calibrationStereoCameras.NumPoints >= minNumberFeatures)
        {
            _calibrationStereoCameras.Flag_Cameras_Measurements = true;
        }
        else
        {
            _calibrationStereoCameras.Flag_Cameras_Measurements = false;
        }

        _calibrationStereoCameras.Flag_Encoders_Measurements = true;

        //Prediction
		_calibrationStereoCameras.Filter_Prediction();

		//Measurements
		Mat Z(_calibrationStereoCameras.NumEncodersMeasurements*_calibrationStereoCameras.Flag_Encoders_Measurements + 4*_calibrationStereoCameras.NumPoints*
        _calibrationStereoCameras.Flag_Cameras_Measurements,1,CV_64F);
		Mat Rvec = Mat::zeros(Z.rows, 1,CV_64F);

		if(_calibrationStereoCameras.Flag_Encoders_Measurements){

		    Mat Z_Ekplus1 = Z(Range(0, _calibrationStereoCameras.NumEncodersMeasurements),Range::all());
			Mat R_Ekplus1 = Rvec(Range(0, _calibrationStereoCameras.NumEncodersMeasurements),Range::all());

			encoders.clone().copyTo(Z_Ekplus1);
			R_Ekplus1 += encodersMeasurementsNoise;
		}

		//if there are measurements for the cameras
		if(_calibrationStereoCameras.Flag_Cameras_Measurements){

			Mat Z_FLKplus1 = Z(Range(_calibrationStereoCameras.NumEncodersMeasurements, _calibrationStereoCameras.NumEncodersMeasurements+
                            2*_calibrationStereoCameras.NumPoints),Range::all());
			Mat Z_FRKplus1 = Z(Range(_calibrationStereoCameras.NumEncodersMeasurements+2*_calibrationStereoCameras.NumPoints,
                            _calibrationStereoCameras.NumEncodersMeasurements+4*_calibrationStereoCameras.NumPoints),Range::all());

			for(int j=0; j<_calibrationStereoCameras.NumPoints; j++){

				Z_FLKplus1.at<double>(2*j,0) = FeaturesLeft[j].Point.x;
				Z_FLKplus1.at<double>(2*j+1,0) = FeaturesLeft[j].Point.y;

				Z_FRKplus1.at<double>(2*j,0) = FeaturesRight[j].Point.x;
				Z_FRKplus1.at<double>(2*j+1,0) = FeaturesRight[j].Point.y;
			}

			Mat R_F = Rvec(Range(_calibrationStereoCameras.NumEncodersMeasurements,
                        _calibrationStereoCameras.NumEncodersMeasurements+4*_calibrationStereoCameras.NumPoints), Range::all());
			R_F += featuresMeasurementsNoise;

		}//*/

		Mat R = Mat::eye(Rvec.rows,Rvec.rows,CV_64F);
		for(int j=0; j<Rvec.rows; j++)
			R.at<double>(j,j) = Rvec.at<double>(j,0)*Rvec.at<double>(j,0);

		_calibrationStereoCameras.Rn_explicit = Mat();
		_calibrationStereoCameras.Rn_implicit = R.clone();

		if(_calibrationStereoCameras.Flag_Encoders_Measurements && _calibrationStereoCameras.Flag_Cameras_Measurements)
            _calibrationStereoCameras.Filter_Update(Mat(), Z.clone());//*/

        X = _calibrationStereoCameras.X_k.clone();
        P = _calibrationStereoCameras.P_k.clone();

		Mat Offsets = Mat::zeros(_calibrationStereoCameras.NumEncodersMeasurements, 1,CV_64F);
		Offsets.at<double>(0,0) = X.at<double>(0,0);
		Offsets.at<double>(1,0) = X.at<double>(1,0);
		Offsets.at<double>(2,0) = X.at<double>(2,0);
		Offsets.at<double>(4,0) = X.at<double>(3,0);

		Mat RealEncoders = encoders-Offsets;

        Kinematics_icubEyes AuxKCE;
        KinTransforms AuxKinTr;
        AuxKCE.Apply(RealEncoders, AuxKinTr);
        Mat R_est(3,3,CV_64F);
        Mat t_est(3,1,CV_64F);

        for(int kkr=0; kkr<3; kkr++)
        {
            t_est.at<double>(kkr, 0) = AuxKinTr.LeftCamRefFrame_To_RightCamRefFrame.at<double>(kkr,3)*baseline;

            for(int kkc=0; kkc<3; kkc++)
            {
                R_est.at<double>(kkr, kkc) = AuxKinTr.LeftCamRefFrame_To_RightCamRefFrame.at<double>(kkr,kkc);
            }
        }
        Mat rot_est;
        Rodrigues(R_est,rot_est);

        //Write the data back
        mutex.wait();
            _stereoCalibThread_output_data._imagesBase_data = _imagesBase_data;
            _stereoCalibThread_output_data.R_LeftToRight = R_est;
            _stereoCalibThread_output_data.T_LeftToRight = t_est;
        mutex.post();
	}

}

stereoCalibThread_output_data stereoCalibThread::getData()
{
    stereoCalibThread_output_data stereoCalibThread_output_data_aux;

    mutex.wait();

        stereoCalibThread_output_data_aux._imagesBase_data = _stereoCalibThread_output_data._imagesBase_data;
        stereoCalibThread_output_data_aux.R_LeftToRight = _stereoCalibThread_output_data.R_LeftToRight;
        stereoCalibThread_output_data_aux.T_LeftToRight = _stereoCalibThread_output_data.T_LeftToRight;

    mutex.post();

    return stereoCalibThread_output_data_aux;//*/
}
