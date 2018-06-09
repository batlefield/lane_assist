#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat rotate(Mat src, double angle) {
    Point2f p(src.cols / 2., src.rows/2.);
    Mat r = getRotationMatrix2D(p, angle, 1.0);
    Mat dst;
    warpAffine(src, dst, r, src.size());
    return dst;
}

int main() {

    VideoCapture camera(0); // ID of the camera

    if (!camera.isOpened()) {
        cout << "Unable to access camera" << endl;
        return -1;
    }

    Mat frame;

    cout << "Framerate: " << camera.get(CV_CAP_PROP_FPS) << endl;

    int frame_width = camera.get(CV_CAP_PROP_FRAME_WIDTH);
    int frame_height = camera.get(CV_CAP_PROP_FRAME_HEIGHT);

    VideoWriter writer("/home/pi/v1.avi", CV_FOURCC('P', 'I', 'M', '1'), 30, Size(frame_width, frame_height), true);

    while (true) {

        camera.read(frame);
        //frame = rotate(frame, 180.0);
        writer.write(frame);
        //imshow("Camera", frame);

        if (waitKey(33) >= 0)
            break;
    }

    return 0;
}
