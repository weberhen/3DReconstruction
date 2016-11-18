//// My first OpenCV code
////
//
//#include "stdafx.h"
//#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <iostream>
//#include <string>
//using namespace cv;
//using namespace std;
//int main(int argc, char** argv)
//{
//  // create a string, i.e. an array  of 50 char
//  char command[50];
//
//  // this will 'fill' the string command with the right stuff,
//  // assuming myFile and convertedFile are strings themselves
//  sprintf_s(command, "ConsoleExample.exe ");
//
//  // system call
//  for (int i = 0; i < 2; i++) {
//      system(command);
//      string imageName("C://Users//jack//Pictures//RemotePhoto//IMG_0001.JPG"); // by default
//      //sprintf_s(command, "del /f  C://Users//jack//Pictures//RemotePhoto//IMG_0001.JPG");
//      if (argc > 1)
//      {
//          imageName = argv[1];
//      }
//      Mat image;
//      image = imread(imageName.c_str(), IMREAD_COLOR); // Read the file
//      resize(image, image, Size(), 0.3, 0.3, INTER_NEAREST);
//      if (image.empty())                      // Check for invalid input
//      {
//          cout << "Could not open or find the image" << std::endl;
//          return -1;
//      }
//      namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
//      imshow("Display window", image);                // Show our image inside it.
//      cvtColor(image, image, CV_BGR2GRAY);
//      waitKey(0); // Wait for a keystroke in the window
//
//      Mat stripe_img(image.size(), CV_8U, Scalar(0));
//      for (int i = 1; i < image.rows - 1; i++)
//      {
//          for (int j = 0; j < image.cols; j++)
//          {
//              if (image.at<uchar>(i, j)>image.at<uchar>(i - 1, j) &&
//                  image.at<uchar>(i, j)>image.at<uchar>(i + 1, j))
//              {
//                  stripe_img.at<uchar>(i, j) = 255;
//              }
//          }
//      }
//      imshow("stripes", stripe_img);
//      waitKey();
//  }
//  return 0;
//}

//#include "opencv2/videoio/videoio.hpp"
//#include "opencv2/highgui.hpp"
//#include "opencv2/imgproc.hpp"
#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>

#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;

//#define USINGKINECT 1
#define TESTING 1

int main(int argc, char* argv[])
{

    char test;
    bool testValues = false;
    cout << "use test values? y/n" << endl;
    cin >> test;
    if (test == 'y')
        testValues = true;
    //project stripes
    int pWidth = 200, pHeight = 200;

    if (!testValues)
    {
        cout << "Projector width dimension (pWidth):" << endl;
        cin >> pWidth;
        cout << "Projector height dimension (pHeight):" << endl;
        cin >> pHeight;
    }

    int stripeSize = 1;
    int distanceBetweenStripes = 8;
    int nStripes = 0;

    Mat imgStripe(pHeight, pWidth, CV_8UC1, Scalar(255));
    for (int i = 0; i < pHeight - stripeSize; i += (distanceBetweenStripes + stripeSize))
    {
        for (int k = 0; k<stripeSize; k++)
        {
            for (int j = 0; j < pWidth; j++)
            {
                imgStripe.at<uchar>(i + k, j) = 0;
            }
        }
        nStripes++;
    }
    imshow("imgStripe", imgStripe);
    waitKey();

    double projectionHeight = 30;
    if (!testValues)
    {
        cout << "Height of projection (in mm):" << endl;
        cin >> projectionHeight;
    }

    //get system parameters
    // Dp: Distance between the projector and the system origin 
    // Ds: Distance between the camera and the projector  
    // P:  Pixel size on the sensor plane of the camera  
    // W:  Width between successive stripes on the calibration plane 
    // k:  Radial lens distortion coefficient 

    double Dp = 790.;
    double Ds = 61.;
    double P = 0.0006;
    double W = 5.08;

    if (!testValues)
    {
        cout << "Distance between the projector and the system origin in mm(Dp):" << endl;
        cin >> Dp;

        cout << "Distance between the camera and the projector in mm(Ds):" << endl;
        cin >> Ds;

        cout << "Pixel size on the sensor plane of the camera in mm(P):" << endl;
        cin >> P;

        cout << "Width between successive stripes on the calibration plane in mm(W):" << endl;
        cin >> W;
    }

    //get image from the camera
#ifdef USINGKINECT
    VideoCapture capture(CV_CAP_OPENNI); // I have tried with capture(0), capture(1)
#else
    //VideoCapture capture(0);
#endif

    /*if (!(capture.isOpened())) {
        cout << "OPENCV:Cannot open the WebCamera" << endl;
        return -1;
    }*/
    char command[50];
    sprintf_s(command, "ConsoleExample.exe ");

    cout << "Place the camera in the correct position" << endl;
    for (;;)
    {
        Mat bgrImage;

#ifdef USINGKINECT
        capture.grab();
        capture.retrieve(bgrImage, CV_CAP_OPENNI_GRAY_IMAGE);
#else
        //capture >> bgrImage;
        string imageName("C://Users//jack//Pictures//RemotePhoto//IMG_0001.JPG"); // by default
        system(command);
        bgrImage = imread(imageName);
        resize(bgrImage, bgrImage, Size(), 0.3, 0.3, INTER_NEAREST);
        cv::cvtColor(bgrImage, bgrImage, CV_BGR2GRAY);
#endif        
        imshow("WebCam", bgrImage);

        if (waitKey(30) >= 0)
            break;
    }
    vector<double>x, y, z;

    for (int i = 0; i < pHeight - stripeSize; i += (distanceBetweenStripes + stripeSize))
    {
        Mat bgrImage;

        imgStripe = 255;

        for (int k = 0; k<stripeSize; k++)
        {
            for (int j = 0; j < pWidth; j++)
            {
                imgStripe.at<uchar>(i + k, j) = 0;
            }
        }

        imshow("imgStripe", imgStripe);

#ifdef USINGKINECT
        capture.grab();
        capture.retrieve(bgrImage, CV_CAP_OPENNI_GRAY_IMAGE);
#ifdef TESTING
        //bgrImage = imgStripe.clone();
        bgrImage = imread("stripes.jpg");
        resize(bgrImage, bgrImage, Size(), 0.5, 0.5, INTER_NEAREST);
        cv::cvtColor(bgrImage, bgrImage, CV_BGR2GRAY);
        threshold(bgrImage, bgrImage, 127, 255, THRESH_BINARY);
        imshow("bgrImage", bgrImage);
        waitKey();
#endif
#else   
        //capture >> bgrImage;
        cv::cvtColor(bgrImage, bgrImage, CV_BGR2GRAY);

#endif
        double M = bgrImage.rows;
        double N = bgrImage.cols;

        Mat nonZeroCoordinates;


        double Wn = W * (nStripes * 20 - (double)i / (double)(distanceBetweenStripes + stripeSize));

        bitwise_not(bgrImage, bgrImage);
        findNonZero(bgrImage, nonZeroCoordinates);

        for (int k = 0; k < nonZeroCoordinates.total(); k++) {

            //cout << "Zero#" << i << ": " << nonZeroCoordinates.at<Point>(i).x << ", " << 
            //nonZeroCoordinates.at<Point>(i).y << endl;          

            double r = (double)nonZeroCoordinates.at<Point>(k).y;
            double c = (double)nonZeroCoordinates.at<Point>(k).x;

            double u = r - 0.5*(M + 1.);
            double h = c - 0.5*(N + 1.);

            x.push_back(Dp - ((Dp*Ds) / (u*P*Dp + Wn)));
            y.push_back((h*P*Dp*Ds) / (u*P*Dp + Wn));
            z.push_back((Wn*Ds) / (u*P*Dp + Wn));

            //cout<<Dp<<" "<<(Dp*Ds)<<" "<<(u*P*Dp + Wn)<<endl;

            //cout<<"r: "<<r<<" c: "<<c<<" u: "<<u<<" h: "<<h<<" M: "<<M<<" N: "<<N<<" Dp: "<<
            //Dp<<" Ds: "<<Ds<<" P: "<<P<<" Wn: "<<Wn<<" x: "<<x.at(x.size()-1)<<" y: "
            //<<y.at(x.size()-1)<<" z: "<<z.at(x.size()-1)<<endl;


        }

        imshow("WebCam", bgrImage);
        waitKey(100);
    }


    string outFilename = "face3dpointcloud.pcd";
    ofstream outFile(outFilename.c_str());

    if (!outFile)
    {
        cerr << "Error opening output file: " << outFilename << "!" << endl;
        exit(1);
    }

    outFile << "# .PCD v.7 - Point Cloud Data file format" << endl;
    outFile << "VERSION .7" << endl;
    outFile << "FIELDS x y z" << endl;
    outFile << "SIZE 4 4 4 " << endl;
    outFile << "TYPE F F F " << endl;
    outFile << "COUNT 1 1 1 " << endl;
    outFile << "WIDTH " << x.size() << endl;
    outFile << "HEIGHT 1" << endl;
    outFile << "VIEWPOINT 0 0 0 1 0 0 0" << endl;
    outFile << "POINTS " << x.size() << endl;
    outFile << "DATA ascii" << endl;

    for (int i = 0; i < x.size(); i++)
        outFile << x[i] << " " << y[i] << " " << z[i] << endl;

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}

