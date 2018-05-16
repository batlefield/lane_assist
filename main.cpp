//
// Created by peter on 5/16/18.
//
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;


//processes cpatured image
Mat process(Mat frame) {

    return frame;
}

int main(int argc, char **argv) {
    string calibration_file("../camera.yaml");
    VideoCapture camera;
    if(argc > 1) {
        //posnetek
        camera = VideoCapture(argv[1]);
    } else {
        //kamera
        camera = VideoCapture(0);
        camera.set(CV_CAP_PROP_CONTRAST, 0.2);
    }

    Mat frame, rectified, intrinsics, distortion, map1, map2;

    FileStorage fs(calibration_file, FileStorage::READ);
    fs["intrinsics"] >> intrinsics;
    fs["distortion"] >> distortion;

    camera.read(frame);

    Mat camera_matrix = getOptimalNewCameraMatrix(intrinsics, distortion, frame.size(), 1);

    initUndistortRectifyMap(intrinsics, distortion, Mat(), camera_matrix, frame.size(), CV_16SC2, map1, map2);

    while(true) {
        camera.read(frame);
        remap(frame, rectified, map1, map2, INTER_LINEAR);
        Mat detect;
        rectified.copyTo(detect);
        process(detect);
        rectified.push_back(detect);
        imshow("Camera", rectified);

        if(waitKey(33) >= 0) {
            break;
        }
    }



    return 0;
}
