#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

/// Global variables

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";

Mat maskImg;
bool created_mask = false;

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{
  /// Reduce noise with a kernel 3x3
  blur( src_gray, detected_edges, Size(3,3) );

  /// Canny detector
  Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

  /// Using Canny's output as a mask, we display our result
  dst = Scalar::all(0);

  src.copyTo( dst, detected_edges);
  imshow( window_name, dst );
 }

 Mat Sobel(Mat &src)
 {
  //![variables]
  Mat src_gray;
  Mat grad;
  const char* window_name = "Sobel Demo - Simple Edge Detector";
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  //![variables]

  //![reduce_noise]
  GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
  //![reduce_noise]

  //![convert_to_gray]
  cvtColor( src, src_gray, COLOR_BGR2GRAY );
  //![convert_to_gray]

  //![sobel]
  /// Generate grad_x and grad_y
  Mat grad_x, grad_y;
  Mat abs_grad_x, abs_grad_y;

  /// Gradient X
  //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
  Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );

  /// Gradient Y
  //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
  Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
  //![sobel]

  //![convert]
  convertScaleAbs( grad_x, abs_grad_x );
  convertScaleAbs( grad_y, abs_grad_y );
  //![convert]

  //![blend]
  /// Total Gradient (approximate)
  addWeighted( abs_grad_x, 0., abs_grad_y, 1., 0, grad );
  //![blend]

  Mat gradWithMask, abs_gradWithMask, bin_gradWithMask;
  if(created_mask)
  {
  	grad.copyTo(gradWithMask);
  	imshow("FINAL MASK",maskImg);
  	gradWithMask-=maskImg;
  	convertScaleAbs( gradWithMask, abs_gradWithMask );

  	threshold( gradWithMask, bin_gradWithMask, 60, 255,0 );
  	


  	int morph_elem = 1;
  	int morph_operator = 4;
	int morph_size = 1;
  	// Since MORPH_X : 2,3,4,5 and 6
  	int operation = morph_operator + 2;
  	Mat element = getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
  	/// Apply the specified morphology operation
  	morphologyEx( bin_gradWithMask, bin_gradWithMask, operation, element );

  	abs_gradWithMask = bin_gradWithMask.clone();

  	imshow("bin_gradWithMask",bin_gradWithMask);
  	imwrite("bin_gradWithMask.png",bin_gradWithMask);
  	
  }  	
  else
  {
  	created_mask = true;
  	grad.copyTo(gradWithMask);
  	convertScaleAbs( gradWithMask, abs_gradWithMask );

  	int morph_elem = 2;
  	int morph_operator = 1;
	int morph_size = 6;
  	// Since MORPH_X : 2,3,4,5 and 6
  	int operation = morph_operator + 2;

  	Mat element = getStructuringElement( morph_elem, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
  	threshold( abs_gradWithMask, bin_gradWithMask, 60, 255,0 );
  	/// Apply the specified morphology operation
  	morphologyEx( bin_gradWithMask, bin_gradWithMask, operation, element );
  	imshow("CREATEDMASK",bin_gradWithMask);

  	abs_gradWithMask = bin_gradWithMask.clone();
  }
  	
  //![display]
  imshow("maskImg",maskImg);
  imshow( window_name, abs_gradWithMask );
  imwrite("abs_gradWithMask.png",abs_gradWithMask);
  waitKey(0);
  //![display]

  return abs_gradWithMask;
}


/** @function main */
int main( int argc, char** argv )
{
	int indexPicture=0;
	
	bool hasImage = true;
	/// Load an image

	maskImg = imread( "/home/jack/Dropbox/ULaval/1erSession/GIF7001/FinalProject/dataset/kettle/0.png" );
	maskImg = Sobel(maskImg);

	do
	{
		std::ostringstream oss;
		oss << indexPicture++;

		src = imread( "/home/jack/Dropbox/ULaval/1erSession/GIF7001/FinalProject/dataset/kettle/"+oss.str()+".png" );

		Sobel(src);

		if( !src.data )
		{ hasImage = false; }

		/// Create a matrix of the same type and size as src (for dst)
		dst.create( src.size(), src.type() );

		/// Convert the image to grayscale
		cvtColor( src, src_gray, CV_BGR2GRAY );

		/// Create a window
		namedWindow( window_name, CV_WINDOW_AUTOSIZE );

		/// Create a Trackbar for user to enter threshold
		createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );

		/// Show the image
		CannyThreshold(0, 0);

		/// Wait until user exit program by pressing a key
		waitKey(0);
	}while(hasImage);

	return 0;
}