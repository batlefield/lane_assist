//
// Created by peter on 5/16/18.
//
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;


Point2f upperLeft(260, 236);
Point2f upperRight(376, 236);
Point2f lowerRight(633, 348);
Point2f lowerLeft(67, 353);

//processes captured image
Mat process(Mat frame) {
    Mat transformed, colorMask, sobelMask, finalMask;
    Size frameSize = frame.size();


    //warpamo perspektivo da dobimo cesto na sliki - tako izracunamo radij crt
    Point2f inputQuad[] = {upperLeft, upperRight, lowerRight, lowerLeft};
    Point2f outputQuad[] = {Point2f(0, 0), Point2f(frameSize.width, 0), Point2f(frameSize.width, frameSize.height), Point2f(0, frameSize.height)};

    Mat transMatrix = getPerspectiveTransform(inputQuad, outputQuad);

    warpPerspective(frame, transformed, transMatrix, frameSize);

    //ustvarimo barvno masko
    inRange(transformed, Scalar(150, 150, 150), Scalar(255, 255, 255), colorMask);

    //ustvarimo masko odvodov
    Sobel(transformed, sobelMask, colorMask.depth(), 1, 0);

    //se znebimo malo motenj
    erode(sobelMask, sobelMask, Mat(), Point(-1, -1), 1);
    dilate(sobelMask, sobelMask, Mat(), Point(-1, -1), 5);

    //sestavimo masko in jo thresholdamo
    cvtColor(sobelMask, sobelMask, CV_BGR2GRAY);
    bitwise_and(sobelMask, colorMask, finalMask);
    threshold(finalMask, finalMask, 40, 255, THRESH_BINARY);


    int width = finalMask.size().width;
    int height = finalMask.size().height;

    Point2i lanePoints[8];

    int summed[8][width];
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < width; j++) {
            Mat sub(finalMask, Rect(j, i * (height / 8), 1, height / 8));
            summed[i][j] = countNonZero(sub);
        }
    }


    for(int i = 0; i < 8; i++) {
        float maxl = 0, maxr = 0;
        int maxli = 0, maxri = width - 1;
        for(int j = 0; j < width / 2; j++) {
            float l = summed[i][j];
            float r = summed[i][width - j - 1];
            if(l > maxl) {
                maxl = l;
                maxli = j;
            }
            if(r > maxr) {
                maxr = r;
                maxri = width - j;
            }
        }
        lanePoints[i] = Point2i(maxli, maxri);
        //printf("%d, %d\n", maxli, maxri);
    }


    cvtColor(finalMask, finalMask, CV_GRAY2BGR);

    for(int i = 0; i < 8; i++) {
        int ch = (i * (height / 8)) + (height / 16);
        circle(finalMask, Point2i(lanePoints[i].x, ch), 4, Scalar(0, 0, 255));
        circle(finalMask, Point2i(lanePoints[i].y, ch), 4, Scalar(0, 0, 255));
    }

    return finalMask;
}

int main(int argc, char **argv) {
    VideoCapture camera;
    if(argc > 1) {
        //posnetek
        camera = VideoCapture(argv[1]);
    } else {
        //kamera
        camera = VideoCapture(0);
    }

    Mat frame;


    while(true) {
        camera.read(frame);
        Mat detect;
        frame.copyTo(detect);
        Mat processed = process(detect);
        imshow("Camera", frame);
        imshow("Processed", processed);

        int c = waitKey(-1);
        if(c == (int)'q') {
            break;
        }

    }


    return 0;
}

