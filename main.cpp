//
// Created by peter on 5/16/18.
//
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

using namespace std;
using namespace cv;


//processes captured image
Mat process(Mat frame) {
    Mat processed;
    Size frameSize = frame.size();


    //warpamo perspektivo da dobimo cesto na sliki - tako izracunamo radij crt
    Point2f inputQuad[] = {Point2f(315, 300), Point2f(435, 300), Point2f(760, 440), Point2f(0, 480)};
    Point2f outputQuad[] = {Point2f(0, 0), Point2f(frameSize.width, 0), Point2f(frameSize.width, frameSize.height), Point2f(0, frameSize.height)};

    Mat transMatrix = getPerspectiveTransform(inputQuad, outputQuad);

    warpPerspective(frame, processed, transMatrix, frameSize);

    

    return processed;
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
        Mat processed = process(detect);
        imshow("Camera", rectified);
        imshow("Processed", processed);

        if(waitKey(33) >= 0) {
            break;
        }
    }



    return 0;
}

