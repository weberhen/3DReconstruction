#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
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
	Mat bgrImage;
	char command[50];
	sprintf_s(command, "ConsoleExample.exe ");
	string imageName("U://My Pictures//RemotePhoto//IMG_0001.JPG"); // by default
	char test;
	bool testValues = false;
	cout << "use test values? y/n" << endl;
	cin >> test;
	if (test == 'y')
		testValues = true;
	//project stripes
	int pWidth = 1000, pHeight = 630;

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
	//capture >> bgrImage;
	
	system(command);
	bgrImage = imread(imageName);
	resize(bgrImage, bgrImage, Size(), 0.3, 0.3, INTER_NEAREST);
	bgrImage = bgrImage.colRange(200,1000).rowRange(93,687);
	cv::cvtColor(bgrImage, bgrImage, CV_BGR2GRAY);
	imshow("IMAGE FOR CALIBRATION",bgrImage);
	waitKey();
	double projectionHeight = 30;
	/*if (!testValues)
	{
		cout << "Height of projection (in mm):" << endl;
		cin >> projectionHeight;
	}*/

	//get system parameters
	// Dp: Distance between the projector and the system origin 
	// Ds: Distance between the camera and the projector  
	// P:  Pixel size on the sensor plane of the camera  
	// W:  Width between successive stripes on the calibration plane 
	// k:  Radial lens distortion coefficient 

	double Dp = 49;
	double Ds = 4.5;
	double Dh = 24.25;
	double Dv = 15.5;
	
	if (!testValues)
	{
		cout << "Distance between the projector and the system origin in inches(Dp):" << endl;
		cin >> Dp;

		cout << "Distance between the camera and the projector in inches(Ds):" << endl;
		cin >> Ds;

		cout << "The horizontal distance (size of a stripe in inches) (Dh):" << endl;
		cin >> Dh;

		cout << "The vertical distance (distance between first and last stripe) (Dv):" << endl;
		cin >> Dv;
	}
	
	double P = Dh/(bgrImage.cols*Dp);
	double W = Dv/nStripes;
	
	//inches to cm
	Dp = Dp*25.4;
	Ds = Ds*25.4;
	W = W*25.4;

	//set image to white to use as background to get the mask
	imgStripe = 255; 
	imshow("imgStripe", imgStripe);
	waitKey(3);

	//get image from the camera
	system(command);
	bgrImage = imread(imageName);

	resize(bgrImage, bgrImage, Size(), 0.3, 0.3, INTER_NEAREST);
	bgrImage = bgrImage.colRange(200,1000).rowRange(93,687);
	cv::cvtColor(bgrImage, bgrImage, CV_BGR2GRAY);

	Mat canny_img;
	/// Reduce noise with a kernel 3x3
	blur( bgrImage, canny_img, Size(3,3) );

	int kernel_size = 3;
	/// Canny detector
	
	Mat maskImg;
	Canny( canny_img, maskImg, 10, 30, kernel_size );
	bitwise_not(maskImg, maskImg);

	//show images (canny mask and grayscale)
	imshow("Mask", maskImg);
	imshow("WebCam", bgrImage);
	
	waitKey();
	
	cout<<"calibration is done"<<endl;

	vector<double>x, y, z;
	int indexPicture = 0;

	for (int i = 0; i < pHeight - stripeSize; i += (distanceBetweenStripes + stripeSize))
	{
		Mat bgrImage;

		imgStripe = 255;

		//project every stripe once at a time
		for (int k = 0; k<stripeSize; k++)
		{
			for (int j = 0; j < pWidth; j++)
			{
				imgStripe.at<uchar>(i + k, j) = 0;
				//imgStripe.at<uchar>(30*(distanceBetweenStripes + stripeSize) + k, j) = 0;
			}
		}

		imshow("imgStripe", imgStripe);
				
		//take picture
		
		//ask camera for new picture
		system(command);
		waitKey(30);
		bgrImage = imread(imageName);
		resize(bgrImage, bgrImage, Size(), 0.3, 0.3, INTER_NEAREST);
		bgrImage = bgrImage.colRange(200,1000).rowRange(93,687);
		cv::cvtColor(bgrImage, bgrImage, CV_BGR2GRAY);
		
		stringstream ss;
		ss << indexPicture++;
		imwrite(ss.str()+".png",bgrImage);

		Mat canny_img;
		/// Reduce noise with a kernel 3x3
		blur( bgrImage, canny_img, Size(3,3) );

		int kernel_size = 3;
		//// Canny detector
		Canny( canny_img, canny_img, 10, 30, kernel_size );
		Mat stripeOnlyImg;

		Mat stripeMaskImg(bgrImage.size(),CV_8UC1,Scalar(0));
		int stripeMaskSize = 100;
		stripeMaskImg.rowRange(max(0,i-stripeMaskSize),min(bgrImage.rows-1,i+stripeMaskSize)) = 1;
		Mat combinedMaskImg(bgrImage.size(),CV_8UC1,Scalar(0));
		maskImg.copyTo(combinedMaskImg,stripeMaskImg);
		imshow("New Mask",combinedMaskImg);
		canny_img.copyTo(stripeOnlyImg, combinedMaskImg);
		

		int morph_size = 0;
		int morph_operator = 0;
		int operation = morph_operator + 2;
		int morph_elem = 2;

		Mat element = getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );

		/// Apply the specified morphology operation
		morphologyEx( stripeOnlyImg, stripeOnlyImg, operation, element );

		imshow("STRIPEONLYIMG",stripeOnlyImg);
		waitKey(3);

		double M = bgrImage.rows;
		double N = bgrImage.cols;

		Mat nonZeroCoordinates;
		
		double Wn = W * (nStripes * 20 - (double)i / (double)(distanceBetweenStripes + stripeSize));

		//find the line
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

