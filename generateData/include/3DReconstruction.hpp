#include "stdafx.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <string>
#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;

extern int stripeSize;
extern int distanceBetweenStripes;
extern int nStripes;
extern int pWidth, pHeight;
extern string imageName;

extern Mat maskImg;

extern double Dp, Ds, P, W, stripeScaleFactor;
extern int Dp_slider, Ds_slider, P_slider, W_slider, stripeScaleFactor_slider;
extern int Dp_slider_max, Ds_slider_max, P_slider_max, W_slider_max, stripeScaleFactor_slider_max;

void createDataset(string datasetName);

