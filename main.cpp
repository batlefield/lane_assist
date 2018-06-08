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

Point2i prevLanePoints[8];

float carOffset = 0.0;
int carCenter = 282;
float pixelsPerMeter = 144.0;

//processes captured image
Mat process(Mat frame) {
    Mat transformed, colorMask, sobelMask, finalMask;
    Size frameSize = frame.size();


    //warpamo perspektivo da dobimo cesto na sliki - tako izracunamo radij crt
    Point2f inputQuad[] = {upperLeft, upperRight, lowerRight, lowerLeft};
    Point2f outputQuad[] = {Point2f(0, 0), Point2f(frameSize.width, 0), Point2f(frameSize.width, frameSize.height),
                            Point2f(0, frameSize.height)};

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

    //sestejemo bele piksle blocno po stolpcih
    int summed[8][width];
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < width; j++) {
            Mat sub(finalMask, Rect(j, i * (height / 8), 1, height / 8));
            summed[i][j] = countNonZero(sub);
        }
    }

    //najdemo levi in desni maksimum
    for (int i = 0; i < 8; i++) {
        float maxl = 0, maxr = 0;
        int maxli = 0, maxri = width - 1;
        for (int j = 0; j < width / 2; j++) {
            float l = summed[i][j];
            float r = summed[i][width - j - 1];
            if (l > maxl) {
                maxl = l;
                maxli = j;
            }
            if (r > maxr) {
                maxr = r;
                maxri = width - j;
            }
        }
        if (maxri < width / 2) maxri = width - 1;

        /*if(i > 0) {
            if(maxli == 0) {
                maxli = prevLanePoints[i - 1].x;
            }

            if(maxri == width - 1) {
                maxri = prevLanePoints[i - 1].y;
            }
        }*/
        lanePoints[i] = Point2i(maxli, maxri);
        //printf("%d, %d\n", maxli, maxri);
    }

    for (int i = 0; i < 8; i++) {
        prevLanePoints[i] = lanePoints[1];
    }

    cvtColor(finalMask, finalMask, CV_GRAY2BGR);

    for (int i = 0; i < 8; i++) {
        int ch = (i * (height / 8)) + (height / 16);
        circle(finalMask, Point2i(lanePoints[i].x, ch), 4, Scalar(0, 0, 255));
        circle(finalMask, Point2i(lanePoints[i].y, ch), 4, Scalar(0, 0, 255));
    }

    Mat result = Mat::zeros(finalMask.size(), frame.type());

    //narisemo crte
    for(int i = 1; i < 8; i++) {
        if(lanePoints[i - 1].x != 0 && lanePoints[i].x != 0) line(result, Point(lanePoints[i - 1].x, (i - 1) * (height / 8) + (height / 16)), Point(lanePoints[i].x, i * (height / 8) + (height / 16)), Scalar(255, 0, 0, 0), 20);
        if(lanePoints[i - 1].y != width - 1 && lanePoints[i].y != width - 1) line(result, Point(lanePoints[i - 1].y, (i - 1) * (height / 8) + (height / 16)), Point(lanePoints[i].y, i * (height / 8) + (height / 16)), Scalar(255, 0, 0, 0), 20);
    }

    //printf("%d\n", (lanePoints[7].y - lanePoints[7].x));

    if(lanePoints[7].x != 0 && lanePoints[7].y != width - 1) {
        carOffset = lanePoints[7].x + (lanePoints[7].y - lanePoints[7].x) / 2 - carCenter;
        carOffset /= pixelsPerMeter;
    }
    
    printf("%f\n", carOffset);

    Mat warpedBack;
    warpPerspective(result, warpedBack, transMatrix, frameSize, WARP_INVERSE_MAP, BORDER_TRANSPARENT);


    return frame + warpedBack;
}

int main(int argc, char **argv) {
    VideoCapture camera;
    if (argc > 1) {
        //posnetek
        camera = VideoCapture(argv[1]);
    } else {
        //kamera
        camera = VideoCapture(0);
    }

    Mat frame;


    while (true) {
        camera.read(frame);
        Mat detect;
        frame.copyTo(detect);
        Mat processed = process(detect);
        imshow("Camera", frame);
        imshow("Processed", processed);

        int c = waitKey(44);
        if (c == (int) 'q') {
            break;
        }

    }


    return 0;
}

