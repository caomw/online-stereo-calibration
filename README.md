# online-stereo-calibration

An online stereo calibration module for the iCub. This system calibrates the eyes of the iCub in an online manner and performs stereo reconstruction during active vision (version and vergence).

## Environment

All the functionality was tested on machines configured with:

+ Ubuntu 12.04 LTS
+ YARP 2.3.63
+ OpenCV 2.4.9

## Download and Setup

Open a terminal and navigate to the folder ($YOUR_FOLDER) where you want to download the package.

Now, run:

	git clone https://github.com/vislab-tecnico-lisboa/online-stereo-calibration.git

In ($YOUR_FOLDER/online-stereo-calibration/data) you have a config file stereocalib.ini where you can change the intrinsic parameters of your iCub cameras. You can also change the YARP ports names. The stereocalib_icubSIM.ini is already prepared to be used with the iCub Simulator.

## Compilation

Go to ($YOUR_FOLDER/online-stereo-calibration) and do:

	mkdir build
	cd build
	cmake ..
	make

## Running

At this point everything is set to successfuly run the module. Go to ($YOUR_FOLDER/online-stereo-calibration/build) and run:

	./onlineStereoCalibration --from $PATH_TO_YOUR_CONFIG_INI_FILE

The system is ready and waiting for the YARP connections to be made. The system creates 3 input YARP ports, 3 output YARP ports and 1 input-output YARP port (default names defined in the original config file):

	(input) online_stereo_calibration/image/left - receives the raw images from the left camera (before image rectification)
	(input) online_stereo_calibration/image/right - receives the raw images from the right camera (before image rectification)
	(input) online_stereo_calibration/head_encoders/state - receives the head encoders	
	(ouput) online_stereo_calibration/image/left/out - has the stereo rectified image from the left camera
	(ouput) online_stereo_calibration/image/right/out - has the stereo rectified image from the right camera
	(ouput) online_stereo_calibration/image/disparity/out - has the disparity image (in the left camera reference frame)
	(input-output) online_stereo_calibration/point_request/rpc - receives an image point in the format [U V] taken from the rectified left image (online_stereo_calibration/image/left/out) and sends the 3D coordinates of the point in the format [X Y Z], as seen in the left camera reference frame 

The YARP connections should be made, in a different terminal, as follows (example using default YARP ports names defined in the original config file):

1) - to start the online stereo calibration

	yarp connect icub/cam/left online_stereo_calibration/image/left
	yarp connect icub/cam/right online_stereo_calibration/image/right
	yarp connect icub/head/state:o /head_encoders/state

2) - to see the output rectified images and the disparity images

	yarpview /online_stereo_calibration/left/out/view &
	yarpview /online_stereo_calibration/right/out/view &
	yarpview /online_stereo_calibration/disparity/out/view &
	yarp connect online_stereo_calibration/image/left/out /online_stereo_calibration/left/out/view
	yarp connect online_stereo_calibration/image/right/out /online_stereo_calibration/right/out/view
	yarp connect online_stereo_calibration/image/disparity/out /online_stereo_calibration/disparity/out/view

3) - to get the 3D coordinates of an image point from the rectified left image

	yarp rpc online_stereo_calibration/point_request/rpc
	(example)
	160 120
	Response: -0.248432 0.289807 0.666966


Note: for the points 1) and 2) you can run the scripts located in ($YOUR_FOLDER/online-stereo-calibration/scripts), depending if you are using the real iCub or the iCub Simulator.
