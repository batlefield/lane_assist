//
// Created by peter on 5/16/18.
//
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;


//processes cpatured image
Mat process(Mat frame) {
    
    return frame;
}

int main(int argc, char **argv) {
    VideoCapture camera;
    if(argc > 1) {
        //posnetek
        camera = VideoCapture(argv[1]);
    } else {
        //kamera
        camera = VideoCapture(0);
        camera.set(CV_CAP_PROP_CONTRAST, 0.2);
    }

    Mat frame;

    while(true) {
        camera.read(frame);
        Mat detect;
        frame.copyTo(detect);
        process(detect);
        frame.push_back(detect);
        imshow("Camera", frame);

        if(waitKey(33) >= 0) {
            break;
        }
    }



    return 0;
}
