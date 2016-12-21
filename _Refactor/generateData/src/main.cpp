#include "3DReconstruction.hpp"

int main(int argc, char* argv[])
{
	//
	// defining variables
	//
	Mat bgrImage;
	char command[50];

	string datasetName;

	cout << "give a name for the data you are saving"<<endl;
	cin >> datasetName;

	sprintf_s(command, "ConsoleExample.exe ");
	imageName = "U://My Pictures//RemotePhoto//IMG_0001.JPG"; // by default
	//imageName = "C://Users//jack//Pictures//RemotePhoto//IMG_0001.JPG"; // by default

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

	system(command);
	bgrImage = imread(imageName);
	resize(bgrImage, bgrImage, Size(), 0.3, 0.3, INTER_NEAREST);
	//bgrImage = bgrImage.colRange(200, 1000).rowRange(93, 687);
	cv::cvtColor(bgrImage, bgrImage, CV_BGR2GRAY);

	//set image to white to use as background to get the mask
	imgStripe = 255;
	imshow("imgStripe", imgStripe);
	waitKey(3);

	//get image from the camera
	system(command);
	bgrImage = imread(imageName);

	//bgrImage = imread("U://Desktop//githubCode//3DReconstruction//dataset//kettle//0.png");

	resize(bgrImage, bgrImage, Size(), 0.3, 0.3, INTER_NEAREST);
	//bgrImage = bgrImage.colRange(200, 1000).rowRange(93, 687);
	cv::cvtColor(bgrImage, bgrImage, CV_BGR2GRAY);

	imshow("WebCam", bgrImage);

	waitKey();

	cout << "calibration is done" << endl;

	//////////////////////////////////getting point cloud//////////////////////////////////
	createDataset(datasetName);

	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}

