#define LOADINGFROMDATASET

#ifndef LOADINGFROMDATASET
	#include "stdafx.h"
#endif
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <string>
#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;

#define USINGLINUX

int stripeSize = 1;
int distanceBetweenStripes = 8;
int nStripes = 0;
int pWidth = 1000, pHeight = 630;
string imageName;

Mat maskImg;

double Dp, Ds, P, W;
int Dp_slider=61, Ds_slider=1, P_slider=10, W_slider=56;
int Dp_slider_max=100, Ds_slider_max=100, P_slider_max=100, W_slider_max=100;

void createPointCloud(Mat &maskImg)
{
	vector<double>x, y, z;
	int indexPicture = 0;

	for (int i = 0; i < pHeight - stripeSize; i += (distanceBetweenStripes + stripeSize))
	{
		Mat bgrImage;

#ifndef LOADINGFROMDATASET
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
#endif
				
		//take picture
		
		//ask camera for new picture
#ifndef LOADINGFROMDATASET
		system(command);
		waitKey(30);
#endif
#ifdef LOADINGFROMDATASET
		std::ostringstream oss;
		oss << indexPicture++;
		bgrImage = imread(imageName+oss.str()+".png");
#else
		bgrImage = imread(imageName);
#endif

#ifndef LOADINGFROMDATASET		
		resize(bgrImage, bgrImage, Size(), 0.3, 0.3, INTER_NEAREST);
		bgrImage = bgrImage.colRange(200,1000).rowRange(93,687);
#endif
		cv::cvtColor(bgrImage, bgrImage, CV_BGR2GRAY);

		/*stringstream ss;
		ss << indexPicture++;
		imwrite(ss.str()+".png",bgrImage);*/

		Mat canny_img;
		/// Reduce noise with a kernel 3x3
		blur( bgrImage, canny_img, Size(3,3) );

		int kernel_size = 3;
		//// Canny detector
		Canny( canny_img, canny_img, 10, 30, kernel_size );
		Mat stripeOnlyImg;

		//creating stripe mask
		Mat stripeMaskImg(bgrImage.size(),CV_8UC1,Scalar(0));
		int stripeMaskSize = 50;
		stripeMaskImg.rowRange(max(0,i-stripeMaskSize),min(bgrImage.rows-1,i+stripeMaskSize)) = 1;
		
		//combining mask of background with stripe mask
		Mat combinedMaskImg(bgrImage.size(),CV_8UC1,Scalar(0));
		maskImg.copyTo(combinedMaskImg,stripeMaskImg);
#ifndef LOADINGFROMDATASET
		imshow("New Mask",combinedMaskImg);
		imshow("canny_img", canny_img);
#endif
		canny_img.copyTo(stripeOnlyImg, combinedMaskImg);
		

		int morph_size = 0;
		int morph_operator = 0;
		int operation = morph_operator + 2;
		int morph_elem = 2;

		Mat element = getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );

		/// Apply the specified morphology operation
		morphologyEx( stripeOnlyImg, stripeOnlyImg, operation, element );

#ifndef LOADINGFROMDATASET
		imshow("STRIPEONLYIMG",stripeOnlyImg);
		waitKey(3);
#endif

		double M = bgrImage.rows;
		double N = bgrImage.cols;

		Mat nonZeroCoordinates;
		
		//////DEBUG
		double Wn = W * (nStripes * .8  - (double)i / (double)(distanceBetweenStripes + stripeSize));
		//double Wn = W * (nStripes  - 53);

		//find the line
		//bitwise_not(stripeOnlyImg, stripeOnlyImg);
		
		findNonZero(stripeOnlyImg, nonZeroCoordinates);

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

			//cout<<"Dp: "<<Dp<<" "<<" (Dp*Ds): "<<(Dp*Ds)<<" "<<"(u*P*Dp + Wn): "<<(u*P*Dp + Wn)<<endl;

			/*cout<<"r: "<<r<<" c: "<<c<<" u: "<<u<<" h: "<<h<<" M: "<<M<<" N: "<<N<<" Dp: "<<
			Dp<<" Ds: "<<Ds<<" P: "<<P<<" Wn: "<<Wn<<" x: "<<x.at(x.size()-1)<<" y: "
			<<y.at(x.size()-1)<<" z: "<<z.at(x.size()-1)<<endl;*/


		}
#ifndef LOADINGFROMDATASET
		waitKey(3);
#endif
	}

#ifndef USINGLINUX
	string outFilename = "C://Users//jack//Dropbox//ULaval//1erSession//GIF7001//FinalProject//face3dpointcloud.pcd";
#else
	string outFilename = "face3dpointcloud.pcd"; 
#endif


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

	imshow("ImageTrackbar",maskImg);
	waitKey();

}

void on_trackbar_Dp( int, void* )
{
	Dp = (double) Dp_slider/Dp_slider_max  * 2000;
	cout<<"Dp: "<<Dp<<" Ds: "<<Ds<<" W: "<<W<<" P: "<<P<<endl;
	createPointCloud(maskImg);
}

void on_trackbar_Ds( int, void* )
{
	Ds = (double) Ds_slider/Ds_slider_max * 300;
	cout<<"Dp: "<<Dp<<" Ds: "<<Ds<<" W: "<<W<<" P: "<<P<<endl;
	createPointCloud(maskImg);
}

void on_trackbar_P( int, void* )
{
	P = (double) P_slider/P_slider_max / 1000;
	cout<<"Dp: "<<Dp<<" Ds: "<<Ds<<" W: "<<W<<" P: "<<P<<endl;
	createPointCloud(maskImg);
}

void on_trackbar_W( int, void* )
{
	W = (double) W_slider/W_slider_max * 10;
	cout<<"Dp: "<<Dp<<" Ds: "<<Ds<<" W: "<<W<<" P: "<<P<<endl;
	createPointCloud(maskImg);
}

int main(int argc, char* argv[])
{
	//
	// defining variables
	//
	Mat bgrImage;
	char command[50];
	
#ifndef LOADINGFROMDATASET
	sprintf_s(command, "ConsoleExample.exe ");
	imageName = "U://My Pictures//RemotePhoto//IMG_0001.JPG"; // by default
#else
	#ifndef USINGLINUX
		imageName = "C://Users//jack//Dropbox//ULaval//1erSession//GIF7001//FinalProject//dataset//kettle//"; // by default
	#else
		imageName = "/home/jack/Dropbox/ULaval/1erSession/GIF7001/FinalProject/dataset/kettle/"; 
	#endif
#endif



	char test;
	bool testValues = false;

	cout << "use test values? y/n" << endl;
	cin >> test;
	if (test == 'y')
		testValues = true;

	

	if (!testValues)
	{
		cout << "Projector width dimension (pWidth):" << endl;
		cin >> pWidth;
		cout << "Projector height dimension (pHeight):" << endl;
		cin >> pHeight;
	}

	
	Mat imgStripe(pHeight, pWidth, CV_8UC1, Scalar(255));

	//
	// project stripes
	//
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
	
#ifndef LOADINGFROMDATASET
	system(command);
	bgrImage = imread(imageName);
	resize(bgrImage, bgrImage, Size(), 0.3, 0.3, INTER_NEAREST);
	bgrImage = bgrImage.colRange(200,1000).rowRange(93,687);
	cv::cvtColor(bgrImage, bgrImage, CV_BGR2GRAY);
#else
	bgrImage = imread(imageName+"0.png");
#endif
		
	//double projectionHeight = 30;
	
	//
	//get system parameters
	//
	// Dp: Distance between the projector and the system origin 
	// Ds: Distance between the camera and the projector  
	// P:  Pixel size on the sensor plane of the camera  
	// W:  Width between successive stripes on the calibration plane 
	// k:  Radial lens distortion coefficient 

	Dp = 49;
	Ds = 4.5;
	double Dh = 24.25;
	double Dv = 15.5;
	
	if (!testValues)
	{
		imshow("IMAGE FOR CALIBRATION", bgrImage);
		waitKey();

		cout << "Distance between the projector and the system origin in inches(Dp):" << endl;
		cin >> Dp;

		cout << "Distance between the camera and the projector in inches(Ds):" << endl;
		cin >> Ds;

		cout << "The horizontal distance (size of a stripe in inches) (Dh):" << endl;
		cin >> Dh;

		cout << "The vertical distance (distance between first and last stripe) (Dv):" << endl;
		cin >> Dv;
	}
	
	P = Dh/(bgrImage.cols*Dp);
	W = Dv/nStripes;
	
	//inches to cm
	Dp = Dp*25.4;
	Ds = Ds*25.4;
	W = W*25.4;

	

	cout<<"Dp: "<<Dp<<" Ds: "<<Ds<<" W: "<<W<<" P: "<<P<<endl;

	//set image to white to use as background to get the mask
	imgStripe = 255; 
	imshow("imgStripe", imgStripe);
	waitKey(3);

	//get image from the camera
#ifndef LOADINGFROMDATASET
	system(command);
	bgrImage = imread(imageName);
#endif

#ifndef USINGLINUX
	bgrImage = imread("C://Users//jack//Dropbox//ULaval//1erSession//GIF7001//FinalProject//dataset//kettle//0.png");
#else
	bgrImage = imread("/home/jack/Dropbox/ULaval/1erSession/GIF7001/FinalProject/dataset/kettle/0.png"); 
#endif
	
	
#ifndef LOADINGFROMDATASET
	resize(bgrImage, bgrImage, Size(), 0.3, 0.3, INTER_NEAREST);
	bgrImage = bgrImage.colRange(200,1000).rowRange(93,687);
	cv::cvtColor(bgrImage, bgrImage, CV_BGR2GRAY);
#endif

	Mat canny_img;
	/// Reduce noise with a kernel 3x3
	blur( bgrImage, canny_img, Size(3,3) );

	int kernel_size = 3;
	/// Canny detector
	
	
	Canny( canny_img, maskImg, 10, 30, kernel_size );
	bitwise_not(maskImg, maskImg);
	cout << canny_img.size() << endl;
	
#ifndef LOADINGFROMDATASET
	//show images (canny mask and grayscale)
	imshow("Mask", maskImg);
	imshow("WebCam", bgrImage);
	
	waitKey();
#endif
	
	cout<<"calibration is done"<<endl;

	/// Create Windows
 	namedWindow("ImageTrackbar", 1);

	createTrackbar( "Dp", "ImageTrackbar", &Dp_slider, Dp_slider_max, on_trackbar_Dp );
	createTrackbar( "Ds", "ImageTrackbar", &Ds_slider, Ds_slider_max, on_trackbar_Ds );
	createTrackbar( "P", "ImageTrackbar", &P_slider, P_slider_max, on_trackbar_P );
	createTrackbar( "W", "ImageTrackbar", &W_slider, W_slider_max, on_trackbar_W );

 	/// Show some stuff
 	on_trackbar_Dp( Dp_slider, 0 );
 	on_trackbar_Ds( Ds_slider, 0 );
 	on_trackbar_P( P_slider, 0 );
 	on_trackbar_W( W_slider, 0 );

	//////////////////////////////////getting point cloud//////////////////////////////////
	createPointCloud(maskImg);
	
	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}

