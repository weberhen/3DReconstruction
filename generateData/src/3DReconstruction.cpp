#include "3DReconstruction.hpp"

#include <iostream>

int stripeSize = 1;
int distanceBetweenStripes = 8;
int nStripes = 0;
int pWidth = 1000, pHeight = 630;
string imageName;

Mat maskImg;

double Dp, Ds, P, W, stripeScaleFactor = .8;
int Dp_slider = 61, Ds_slider = 40, P_slider = 61, W_slider = 56, stripeScaleFactor_slider = 40;
int Dp_slider_max = 100, Ds_slider_max = 100, P_slider_max = 100, W_slider_max = 100, stripeScaleFactor_slider_max = 100;

void createDataset(string datasetName)
{
	int indexPicture = 0;

	Mat imgStripe(pHeight, pWidth, CV_8U);

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

		cv::imshow("imgStripe", imgStripe);

		//take picture

		char command[50];
		sprintf_s(command, "ConsoleExample.exe ");
		//ask camera for new picture
		system(command);
		waitKey(30);

		bgrImage = imread(imageName);

		resize(bgrImage, bgrImage, Size(), 0.3, 0.3, INTER_NEAREST);
		//bgrImage = bgrImage.colRange(200, 1000).rowRange(93, 687);
		cv::cvtColor(bgrImage, bgrImage, CV_BGR2GRAY);

		stringstream ss;
		ss << indexPicture++;

		imwrite("U://Documents//_codes//3DReconstruction//_Refactor//dataset//presentation_dataset//"+ss.str()+".png",bgrImage);

		waitKey(3);
	}

}

