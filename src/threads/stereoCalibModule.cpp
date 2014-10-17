#include "threads/stereoCalibModule.h"

    double stereoCalibModule::getPeriod()
    {
        return 0.01; //every 10ms
    }
    bool stereoCalibModule::configure(yarp::os::ResourceFinder &rf)
    {
        Bottle ports_root = rf.findGroup("ports_root");
        moduleName = ports_root.find("name").asString();
        setName(moduleName.c_str());

        //Port names
        Bottle input_port_names = rf.findGroup("input_port_names");
        leftImagePortName = "/" + moduleName + (String) input_port_names.find("left_image_port").asString();
        rightImagePortName = "/" + moduleName + (String) input_port_names.find("right_image_port").asString();
        encodersPortName = "/" + moduleName + (String) input_port_names.find("encoders_port").asString();
        rpcPointRequestPortName = "/" + moduleName + (String) input_port_names.find("rpc_point_request_port").asString();

        Bottle output_port_names = rf.findGroup("output_port_names");
        leftImageOutPortName = "/" + moduleName + (String) output_port_names.find("left_image_out_port").asString();
        rightImageOutPortName = "/" + moduleName + (String) output_port_names.find("right_image_out_port").asString();
        disparityImageOutPortName = "/" + moduleName + (String) output_port_names.find("disparity_image_out_port").asString();

        // open ports
        if (!leftImagePort.open(
                leftImagePortName.c_str()))
        {
            cout << getName() << ": unable to open port"
            << leftImagePortName << endl;
            return false;
        }
        _stereoCalibThread_data.leftImagePort = &leftImagePort;


        if (!rightImagePort.open(
                rightImagePortName.c_str()))
        {
            cout << getName() << ": unable to open port"
            << rightImagePortName << endl;
            return false;
        }
        _stereoCalibThread_data.rightImagePort= &rightImagePort;


        if (!encodersPort.open(
                encodersPortName.c_str()))
        {
            cout << getName() << ": unable to open port"
            << encodersPortName << endl;
            return false;
        }
        _stereoCalibThread_data.encodersPort = &encodersPort;

        if (!rpcPointRequestPort.open(
                rpcPointRequestPortName.c_str()))
        {
            cout << getName() << ": unable to open port"
            << rpcPointRequestPortName << endl;
            return false;
        }
        _worldPointThread_data.rpcPointRequestPort = &rpcPointRequestPort;

        if (!leftImageOutPort.open(
                leftImageOutPortName.c_str()))
        {
            cout << getName() << ": unable to open port"
            << leftImageOutPortName << endl;
            return false;
        }
        _disparityThread_data.leftImageOutPort = &leftImageOutPort;

        if (!rightImageOutPort.open(
                rightImageOutPortName.c_str()))
        {
            cout << getName() << ": unable to open port"
            << rightImageOutPortName << endl;
            return false;
        }
        _disparityThread_data.rightImageOutPort = &rightImageOutPort;//*/

        if (!disparityImageOutPort.open(
                disparityImageOutPortName.c_str()))
        {
            cout << getName() << ": unable to open port"
            << disparityImageOutPortName << endl;
            return false;
        }
        _disparityThread_data.disparityImageOutPort = &disparityImageOutPort;//*/

        // Rate threads period
        _stereoCalibThread_data.threadPeriod = rf.check("threadPeriod", Value(0.001),
            "Control rate thread period key value(double) in seconds ").asDouble();
        _disparityThread_data.threadPeriod = _stereoCalibThread_data.threadPeriod;
        _worldPointThread_data.threadPeriod = _stereoCalibThread_data.threadPeriod;

        //Calibration parameters from left camera
        Bottle left_camera_intrinsics = rf.findGroup("left_camera_intrinsics");
        _stereoCalibThread_data._imagesBase_initial_parameters.left_resx = left_camera_intrinsics.find("resx").asInt();
        _stereoCalibThread_data._imagesBase_initial_parameters.left_resy = left_camera_intrinsics.find("resy").asInt();
        _stereoCalibThread_data._imagesBase_initial_parameters.left_cx = left_camera_intrinsics.find("cx").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.left_cy = left_camera_intrinsics.find("cy").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.left_fx = left_camera_intrinsics.find("fx").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.left_fy = left_camera_intrinsics.find("fy").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.left_k1 = left_camera_intrinsics.find("k1").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.left_k2 = left_camera_intrinsics.find("k2").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.left_k3 = left_camera_intrinsics.find("k3").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.left_k4 = left_camera_intrinsics.find("k4").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.left_k5 = left_camera_intrinsics.find("k5").asDouble();

        //Calibration parameters from right camera
        Bottle right_camera_intrinsics = rf.findGroup("right_camera_intrinsics");
        _stereoCalibThread_data._imagesBase_initial_parameters.right_resx = right_camera_intrinsics.find("resx").asInt();
        _stereoCalibThread_data._imagesBase_initial_parameters.right_resy = right_camera_intrinsics.find("resy").asInt();
        _stereoCalibThread_data._imagesBase_initial_parameters.right_cx = right_camera_intrinsics.find("cx").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.right_cy = right_camera_intrinsics.find("cy").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.right_fx = right_camera_intrinsics.find("fx").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.right_fy = right_camera_intrinsics.find("fy").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.right_k1 = right_camera_intrinsics.find("k1").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.right_k2 = right_camera_intrinsics.find("k2").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.right_k3 = right_camera_intrinsics.find("k3").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.right_k4 = right_camera_intrinsics.find("k4").asDouble();
        _stereoCalibThread_data._imagesBase_initial_parameters.right_k5 = right_camera_intrinsics.find("k5").asDouble();

        _disparityThread_data._imagesBase_initial_parameters = _stereoCalibThread_data._imagesBase_initial_parameters;

        //FIlter parameters
        Bottle filter_parameters = rf.findGroup("filter_parameters");
        _stereoCalibThread_data._calibrationStereoCameras_data.matchingThreshold = filter_parameters.find("matching_threshold").asDouble();
        _stereoCalibThread_data._calibrationStereoCameras_data.minNumberFeatures = filter_parameters.find("minimum_number_of_features").asInt();
        _stereoCalibThread_data._calibrationStereoCameras_data.maxNumberFeatures = filter_parameters.find("maximum_number_of_features").asInt();
        _stereoCalibThread_data._calibrationStereoCameras_data.numFixStateParams = filter_parameters.find("number_of_fixed_state_params").asInt();
        _stereoCalibThread_data._calibrationStereoCameras_data.numMeasurements = filter_parameters.find("number_of_meausurements").asInt();
        _stereoCalibThread_data._calibrationStereoCameras_data.encodersStateNoise = filter_parameters.find("encoders_state_noise").asDouble();
        _stereoCalibThread_data._calibrationStereoCameras_data.encodersTransitionNoise = filter_parameters.find("encoders_transition_noise").asDouble();
        _stereoCalibThread_data._calibrationStereoCameras_data.featuresMeasurementsNoise = filter_parameters.find("features_measurements_noise").asDouble();
        _stereoCalibThread_data._calibrationStereoCameras_data.encodersMeasurementsNoise = filter_parameters.find("encoders_measurements_noise").asDouble();
        _stereoCalibThread_data._calibrationStereoCameras_data.desiredImageWidth = filter_parameters.find("desired_image_w").asInt();
        _stereoCalibThread_data._calibrationStereoCameras_data.desiredImageHeight = filter_parameters.find("desired_image_h").asInt();

        //DIsparity map parameters
        Bottle disparity_parameters = rf.findGroup("disparity_parameters");
        _stereoCalibThread_data._imageDisparity_initial_parameters.numberOfDisparities = disparity_parameters.find("number_of_disparities").asInt();
        _stereoCalibThread_data._imageDisparity_initial_parameters.preFilterCap = disparity_parameters.find("pre_filter_cap").asInt();
        _stereoCalibThread_data._imageDisparity_initial_parameters.SADWindowSize = disparity_parameters.find("SAD_window_size").asInt();
        _stereoCalibThread_data._imageDisparity_initial_parameters.P1 = disparity_parameters.find("P1").asInt();
        _stereoCalibThread_data._imageDisparity_initial_parameters.P2 = disparity_parameters.find("P2").asInt();
        _stereoCalibThread_data._imageDisparity_initial_parameters.minDisparity = disparity_parameters.find("min_disparity").asInt();
        _stereoCalibThread_data._imageDisparity_initial_parameters.uniquenessRatio = disparity_parameters.find("uniqueness_ratio").asInt();
        _stereoCalibThread_data._imageDisparity_initial_parameters.speckleWindowSize = disparity_parameters.find("speckle_window_size").asInt();
        _stereoCalibThread_data._imageDisparity_initial_parameters.speckleRange = disparity_parameters.find("speckle_range").asInt();
        _stereoCalibThread_data._imageDisparity_initial_parameters.disp12MaxDiff = disparity_parameters.find("disp12_max_diff").asInt();

        _disparityThread_data._imageDisparity_initial_parameters = _stereoCalibThread_data._imageDisparity_initial_parameters;

        Bottle baseline_value = rf.findGroup("baseline_value");
        _stereoCalibThread_data.baseline = baseline_value.find("baseline").asDouble();

        /* Create the control rate thread */
        _stereoCalibThread = new stereoCalibThread(_stereoCalibThread_data);
        _disparityThread = new disparityThread(_stereoCalibThread, _disparityThread_data);
        _worldPointThread = new worldPointThread(_disparityThread, _worldPointThread_data);

        /* Starts the thread */
        if (!_stereoCalibThread->start()) {
            delete _stereoCalibThread;
            return false;
        }

        if (!_disparityThread->start()) {
            delete _disparityThread;
            return false;
        }

        if (!_worldPointThread->start()) {
            delete _worldPointThread;
            return false;
        }

        return true;
    }
    bool stereoCalibModule::updateModule()
    {
        return true;
    }


    bool stereoCalibModule::interruptModule()
    {
        cout << "Interrupting your module, for port cleanup" << endl;

        leftImagePort.interrupt();
        rightImagePort.interrupt();
        encodersPort.interrupt();

        return true;
    }
    bool stereoCalibModule::close()
    {
        /* optional, close port explicitly */
        cout << "Calling close function\n";

        _stereoCalibThread->stop();
        delete _stereoCalibThread;
        leftImagePort.close();
        rightImagePort.close();
        encodersPort.close();

        return true;
    }

