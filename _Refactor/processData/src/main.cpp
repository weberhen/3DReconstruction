#include "3DReconstruction.hpp"

int main(int argc, char* argv[])
{
	imageName = "C://Users//jack//Documents//_codes//3DReconstruction//dataset//kettle//"; // by default

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

	Mat bgrImage = imread(imageName + "0.png");

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

	P = Dh / (bgrImage.cols*Dp);
	W = Dv / nStripes;

	//inches to cm
	Dp = Dp*25.4;
	Ds = Ds*25.4;
	W = W*25.4;

	cout << "Dp: " << Dp << " Ds: " << Ds << " W: " << W << " P: " << P << endl;

	//good results
	//Dp: 1020 Ds: 120 W: 5.62429 P: 0.00065
	Dp = 1030;

	//set image to white to use as background to get the mask
	imgStripe = 255;

	imshow("imgStripe", imgStripe);
	waitKey(3);

	//get image from the camera
	bgrImage = imread("C://Users//jack//Documents//_codes//3DReconstruction//dataset//kettle//0.png");

	maskImg = createMask(bgrImage);

	cout << "calibration is done" << endl;

	/// Create Windows
	namedWindow("ImageTrackbar", 1);

	createTrackbar("Dp", "ImageTrackbar", &Dp_slider, Dp_slider_max, on_trackbar_Dp);
	createTrackbar("Ds", "ImageTrackbar", &Ds_slider, Ds_slider_max, on_trackbar_Ds);
	createTrackbar("P", "ImageTrackbar", &P_slider, P_slider_max, on_trackbar_P);
	createTrackbar("W", "ImageTrackbar", &W_slider, W_slider_max, on_trackbar_W);
	createTrackbar("SCF", "ImageTrackbar", &stripeScaleFactor_slider, stripeScaleFactor_slider_max, on_trackbar_stripeScaleFactor);

	/// Show some stuff
	on_trackbar_Dp(Dp_slider, 0);
	on_trackbar_Ds(Ds_slider, 0);
	on_trackbar_P(P_slider, 0);
	on_trackbar_W(W_slider, 0);

	//////////////////////////////////getting point cloud//////////////////////////////////
	createPointCloud(maskImg);

	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}

