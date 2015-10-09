//opencv
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>

#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;
// Global variables
Mat frame; //current frame
Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
Mat fgMaskColored;
Ptr<BackgroundSubtractorMOG2> pMOG2; //MOG2 Background subtractor
int keyboard; //input from keyboard
void help();
void processVideo(char* videoFilename);
void processImages(char* firstFrameFilename);

int main(int argc, char* argv[])
{

    //check for the input parameter correctness
    cout << argc << endl;
    if(argc != 3) {
        cerr <<"Incorret input list" << endl;
        cerr <<"exiting..." << endl;
        return EXIT_FAILURE;
    }
    //create GUI windows
    namedWindow("Frame");
    namedWindow("FG Mask MOG 2");
    //create Background Subtractor objects
    pMOG2 = createBackgroundSubtractorMOG2(500, 16, false); //MOG2 approach
    if(strcmp(argv[1], "-vid") == 0) {
        //input data coming from a video
        processVideo(argv[2]);
    }
    else {
        //error in reading input parameters
        cerr <<"Please, check the input parameters." << endl;
        cerr <<"Exiting..." << endl;
        return EXIT_FAILURE;
    }
    //destroy GUI windows
    destroyAllWindows();
    return EXIT_SUCCESS;
}
void processVideo(char* videoFilename) {
    //create the capture object
    VideoCapture capture(videoFilename);
    if(!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open video file: " << videoFilename << endl;
        exit(EXIT_FAILURE);
    }
    double th = 16;
    int history = 500;
    double learning = 0.0;
    float min_area = 0.0;
    //read input data. ESC or 'q' for quitting
    while((char)keyboard != 27 ){
        //read the current frame
        if(!capture.read(frame)) {
            cerr << "Unable to read next frame." << endl;
            cerr << "Exiting..." << endl;
            exit(EXIT_FAILURE);
        }
        if ((char)keyboard == 'a') th -= 10;
        else if ((char)keyboard == 's') th += 10;
        else if ((char)keyboard == 'y') learning -= 0.1;
        else if ((char)keyboard == 'x') learning += 0.1;
        else if ((char)keyboard == 'q') history -= 10;
        else if ((char)keyboard == 'w') history += 10;
        else if ((char)keyboard == 'e') min_area -= 5.f;
        else if ((char)keyboard == 'r') min_area += 5.f;
        pMOG2->setVarThreshold(th);
        pMOG2->setHistory(history);
        //update the background model
        pMOG2->apply(frame, fgMaskMOG2, learning);
        //get the frame number and write it on the current frame
        stringstream ss;
        rectangle(frame, cv::Point(10, 2), cv::Point(100,20),
                  cv::Scalar(255,255,255), -1);
        ss << capture.get(CAP_PROP_POS_FRAMES);
        string frameNumberString = ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
        //show the current frame and the fg masks
        fgMaskColored = frame.clone();
        for (int r = 0; r < fgMaskMOG2.rows; r ++){
            for (int c = 0; c < fgMaskMOG2.cols; c ++){
                if (fgMaskMOG2.at<uchar>(Point(c, r)) == 0){
                    fgMaskColored.at<Vec3b>(Point(c, r)) = Vec3b(255, 255, 0);
                }
            }
        }
        /*vector<KeyPoint> keypoints;
        SimpleBlobDetector::Params params;
        params.blobColor = 255;
        params.minArea = min_area;
        params.maxArea = 10000000.0f;
        params.filterByColor = true;
        params.filterByArea = true;
        Ptr<SimpleBlobDetector> detector;
        detector = SimpleBlobDetector::create(params);
        detector->detect(fgMaskMOG2, keypoints);
        Mat im_keypoints;
        drawKeypoints(fgMaskMOG2, keypoints, im_keypoints);
        if (keypoints.size() > 0 )
            cout << keypoints[0].pt.x << "      " << keypoints[0].pt.x <<endl;
        cout << "Current settings, threshold = " << th << "     history = " << history << "     learning coef. = " << learning << endl;
        cout << "min area = " << params.minArea << "    max area = " << params.maxArea << endl;*/
        imshow("Frame", frame);
        imshow("FG Mask MOG 2", fgMaskColored);

        //get the input from the keyboard
        keyboard = waitKey( 30 );
    }
    //delete capture object
    capture.release();
}
