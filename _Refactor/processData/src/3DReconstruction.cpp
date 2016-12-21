#include "3DReconstruction.hpp"

int stripeSize = 1;
int distanceBetweenStripes = 8;
int nStripes = 0;
int pWidth = 1000, pHeight = 630;

double Dp, Ds, P, W, stripeScaleFactor = .8;
int Dp_slider = 61, Ds_slider = 40, P_slider = 61, W_slider = 56, stripeScaleFactor_slider = 40;
int Dp_slider_max = 100, Ds_slider_max = 100, P_slider_max = 100, W_slider_max = 100, stripeScaleFactor_slider_max = 100;

string imageName;

Mat maskImg;

Mat createMask(Mat &src)
{
	Mat src_gray, grad_y, abs_grad_y, grad;

	//![variables]
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	int morph_elem = 2;
	int morph_operator = 1;
	int morph_size = 6;
	int operation = morph_operator + 2; // Since MORPH_X : 2,3,4,5 and 6
	//![variables]

	//![reduce_noise]
	GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
	//![reduce_noise]

	//![convert_to_gray]
  	cvtColor( src, src_gray, COLOR_BGR2GRAY );
  	//![convert_to_gray]

	/// Gradient Y
	Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
	//![sobel]

	//![convert]
	convertScaleAbs( grad_y, abs_grad_y );
	//![convert]

	//![blend]
	/// Total Gradient (approximate)
	grad = abs_grad_y.clone();
	//![blend]
	
  	Mat element = getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
  	threshold( grad, grad, 60, 255,0 );

  	/// Apply the specified morphology operation
  	morphologyEx( grad, grad, operation, element );
  	
	imshow("CORRECT MASK",grad);
  	waitKey();

  	return grad;
}

//given a mask taken from the scene without any stripe, the function returns only the stripe in the image
Mat getStripe(Mat &maskImg, Mat &src)
{
	Mat src_gray, grad_y, abs_grad_y, grad;

	//Apply Sobel

	//![variables]
	int scale = 1;
  	int delta = 0;
  	int ddepth = CV_16S;
	int morph_elem = 1;
	int morph_operator = 4;
	int morph_size = 1;
	int operation = morph_operator + 2; // Since MORPH_X : 2,3,4,5 and 6
  	//![variables]

  	//![reduce_noise]
  	GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
  	//![reduce_noise]

  	/// Gradient Y
  	Sobel( src, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
  	//![sobel]

  	//![convert]
  	convertScaleAbs( grad_y, abs_grad_y );
  	//![convert]

  	grad = abs_grad_y.clone();
	
  	grad-=maskImg;

  	Mat abs_grad;
  	convertScaleAbs( grad, abs_grad );

  	Mat bin_grad;
  	threshold( abs_grad, bin_grad, 60, 255,0 );

  	Mat element = getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
  	/// Apply the specified morphology operation
  	morphologyEx( bin_grad, bin_grad, operation, element );

  	imshow("bin_grad",bin_grad);
	waitKey();
	
	return bin_grad;
}

void createPointCloud(Mat &maskImg)
{
	vector<double>x, y, z;
	int indexPicture = 0;

	for (int i = 0; i < pHeight - stripeSize; i += (distanceBetweenStripes + stripeSize))
	{
		Mat bgrImage;
		//take picture
		
		//ask camera for new picture
		std::ostringstream oss;
		oss << indexPicture++;

		bgrImage = imread(imageName+oss.str()+".png");

		if(bgrImage.channels()==3)
			cv::cvtColor(bgrImage, bgrImage, CV_BGR2GRAY);

		Mat originalImg = bgrImage.clone();
		
		Mat stripeOnlyImg = getStripe(maskImg, originalImg);

		double M = bgrImage.rows;
		double N = bgrImage.cols;

		Mat nonZeroCoordinates;
		
		//////DEBUG
		double Wn = W * (nStripes * stripeScaleFactor  - (double)i / (double)(distanceBetweenStripes + stripeSize));
		//double Wn = W * (nStripes  - 53);

		findNonZero(stripeOnlyImg, nonZeroCoordinates);

		for (int k = 0; k < nonZeroCoordinates.total(); k++) {

			double r = (double)nonZeroCoordinates.at<Point>(k).y;
			double c = (double)nonZeroCoordinates.at<Point>(k).x;

			double u = r - 0.5*(M + 1.);
			double h = c - 0.5*(N + 1.);

			//avoid adding background to pointcloud
			if((Dp - ((Dp*Ds) / (u*P*Dp + Wn)))> -140
				&& (Dp - ((Dp*Ds) / (u*P*Dp + Wn)))<130)
			{
				x.push_back(Dp - ((Dp*Ds) / (u*P*Dp + Wn)));
				y.push_back((h*P*Dp*Ds) / (u*P*Dp + Wn));
				z.push_back((Wn*Ds) / (u*P*Dp + Wn));	
	
			}
		}
	}

	string outFilename = "C://Users//jack//Dropbox//ULaval//1erSession//GIF7001//FinalProject//kettleWin2.pcd";

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
	Dp = 1000 + (double) Dp_slider/Dp_slider_max  * 200;
	cout<<"Dp: "<<Dp<<" Ds: "<<Ds<<" W: "<<W<<" P: "<<P<<" stripeScaleFactor: "<<stripeScaleFactor<<endl;
	createPointCloud(maskImg);
}

void on_trackbar_Ds( int, void* )
{
	Ds = (double) Ds_slider/Ds_slider_max * 300;
	cout<<"Dp: "<<Dp<<" Ds: "<<Ds<<" W: "<<W<<" P: "<<P<<" stripeScaleFactor: "<<stripeScaleFactor<<endl;
	createPointCloud(maskImg);
}

void on_trackbar_P( int, void* )
{
	P = (double) P_slider/P_slider_max / 1000;
	cout<<"Dp: "<<Dp<<" Ds: "<<Ds<<" W: "<<W<<" P: "<<P<<" stripeScaleFactor: "<<stripeScaleFactor<<endl;
	createPointCloud(maskImg);
}

void on_trackbar_W( int, void* )
{
	W = (double) W_slider/W_slider_max * 10;
	cout<<"Dp: "<<Dp<<" Ds: "<<Ds<<" W: "<<W<<" P: "<<P<<" stripeScaleFactor: "<<stripeScaleFactor<<endl;
	createPointCloud(maskImg);
}

void on_trackbar_stripeScaleFactor( int, void* )
{
	stripeScaleFactor = (double) stripeScaleFactor_slider/stripeScaleFactor_slider_max * 2;
	cout<<"Dp: "<<Dp<<" Ds: "<<Ds<<" W: "<<W<<" P: "<<P<<" stripeScaleFactor: "<<stripeScaleFactor<<endl;
	createPointCloud(maskImg);
}
