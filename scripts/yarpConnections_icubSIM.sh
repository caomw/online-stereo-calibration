#!/bin/sh

yarpview /moutinho_stereo_calib/image_left_v &
yarpview /moutinho_stereo_calib/image_right_v &
yarpview /moutinho_stereo_calib/image_disparity_v &
sleep 2
yarp connect /icubSim/head/state:o /moutinho_stereo_calib/head_encoders/state &
yarp connect /icubSim/cam/left /moutinho_stereo_calib/image/left &
yarp connect /icubSim/cam/right /moutinho_stereo_calib/image/right &
yarp connect /moutinho_stereo_calib/image/right/out /moutinho_stereo_calib/image_right_v &
yarp connect /moutinho_stereo_calib/image/left/out /moutinho_stereo_calib/image_left_v &
yarp connect /moutinho_stereo_calib/image/disparity/out /moutinho_stereo_calib/image_disparity_v &
