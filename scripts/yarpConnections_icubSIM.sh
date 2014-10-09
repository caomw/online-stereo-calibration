#!/bin/sh

yarpview /online_stereo_calibration/left/out/view &
yarpview /online_stereo_calibration/right/out/view &
yarpview /online_stereo_calibration/disparity/out/view &
sleep 2
yarp connect /icubSim/head/state:o /online_stereo_calibration/head_encoders/state &
yarp connect /icubSim/cam/left /online_stereo_calibration/image/left &
yarp connect /icubSim/cam/right /online_stereo_calibration/image/right &
yarp connect /online_stereo_calibration/image/right/out /online_stereo_calibration/right/out/view &
yarp connect /online_stereo_calibration/image/left/out /online_stereo_calibration/left/out/view &
yarp connect /online_stereo_calibration/image/disparity/out /online_stereo_calibration/disparity/out/view &
