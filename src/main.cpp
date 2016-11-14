#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

int main( int argc, char* argv[] )
{

    //project stripes
    int pWidth, pHeight;
    cout<<"Projector width dimension (pWidth):"<<endl;
    cin >> pWidth;
    cout<<"Projector height dimension (pHeight):"<<endl;
    cin >> pHeight;

    int stripeSize = 4;
    int distanceBetweenStripes=4;

    Mat imgStripe(pHeight,pWidth,CV_8UC1, Scalar(255));
    for (int i = 0; i < pHeight-stripeSize; i+=(distanceBetweenStripes+stripeSize))
    {
        for(int k=0;k<stripeSize;k++)
        {
            for (int j = 0; j < pWidth; j++)
            {
                imgStripe.at<uchar>(i+k,j) = 0;
            }    
        }
    }
    imshow("imgStripe",imgStripe);
    waitKey();

    double projectionHeight;
    cout<<"Height of projection (in mm):"<<endl;
    cin >> projectionHeight;
    
    //get system parameters
    // Dp: Distance between the projector and the system origin 
    // Ds: Distance between the camera and the projector  
    // P:  Pixel size on the sensor plane of the camera  
    // W:  Width between successive stripes on the calibration plane 
    // k:  Radial lens distortion coefficient 

    double Dp;
    cout<<"Distance between the projector and the system origin in mm(Dp):"<<endl;
    cin >> Dp;

    double Ds;
    cout<<"Distance between the camera and the projector in mm(Ds):"<<endl;
    cin >> Ds;

    double P;
    cout<<"Pixel size on the sensor plane of the camera in mm(P):"<<endl;
    cin >> P;

    double W;
    cout<<"Width between successive stripes on the calibration plane in mm(W):"<<endl;
    cin >> W;

    //get image from the camera

    VideoCapture capture(CV_CAP_OPENNI); // I have tried with capture(0), capture(1)

    if(!(capture.isOpened())){
        cout << "OPENCV:Cannot open the WebCamera" << endl;
        return -1;
    }

    cout<<"Place the camera in the correct position"<<endl;
    for(;;)
    {
        Mat bgrImage;

        capture.grab();

        capture.retrieve( bgrImage, CV_CAP_OPENNI_GRAY_IMAGE );
        
        imshow("WebCam", bgrImage);
        
        if(waitKey(30) >= 0) 
            break;
    }

    for (int i = 0; i < pHeight-stripeSize; i+=(distanceBetweenStripes+stripeSize))
    {
        Mat bgrImage;

        imgStripe = 255;

        for(int k=0;k<stripeSize;k++)
        {
            for (int j = 0; j < pWidth; j++)
            {
                imgStripe.at<uchar>(i+k,j) = 0;
            }    
        }

        imshow("imgStripe",imgStripe);
        
        capture.grab();
        capture.retrieve( bgrImage, CV_CAP_OPENNI_GRAY_IMAGE );
        imshow("WebCam",bgrImage);
        waitKey(100);

    }

    //get line and calculate point cloud
    
    
    

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}


/*// My first OpenCV code
//

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <pcl/visualization/cloud_viewer.h>

#include <iostream>
#include <fstream>
#include <string>
using namespace cv;
using namespace std;
int main(int argc, char** argv)
{

    //project stripes
    int pWidth, pHeight;
    cout<<"Projector width dimension (pWidth):"<<endl;
    cin >> pWidth;
    cout<<"Projector height dimension (pHeight):"<<endl;
    cin >> pHeight;

    int stripeSize = 4;
    int distanceBetweenStripes=4;

    Mat imgStripe(pHeight,pWidth,CV_8UC1, Scalar(255));
    for (int i = 0; i < pHeight-stripeSize; i+=(distanceBetweenStripes+stripeSize))
    {
        for(int k=0;k<stripeSize;k++)
        {
            for (int j = 0; j < pWidth; j++)
            {
                imgStripe.at<uchar>(i+k,j) = 0;
            }    
        }
    }
    imshow("imgStripe",imgStripe);
    waitKey();

    //get system parameters
    // Dp: Distance between the projector and the system origin 
    // Ds: Distance between the camera and the projector  
    // P:  Pixel size on the sensor plane of the camera  
    // W:  Width between successive stripes on the calibration plane 
    // k:  Radial lens distortion coefficient 
    
    




    //get image from the camera

    //get line and calculate point cloud





    //////////////////////////////////////////////////////////////////
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud;

    string imageName("our_index_stripes.png"); // by default
    if (argc > 1)
    {
        imageName = argv[1];
    }
    Mat image;
    image = imread(imageName.c_str()); // Read the file
    cv::cvtColor(image, image, CV_BGR2GRAY);

    if (image.empty())                      // Check for invalid input
    {
        cout << "Could not open or find the image" << std::endl;
        return -1;
    }

    //vector<int>real_index = { 39, 32, 25, 18, 11, 4, -3, -10, -17, -24, -31 };
    vector<int>real_index = { 48, 41, 34, 27, 20, 13, 6, -1, -8, -15, -22 };
   
    //getPointCloud(790,3.08,61,0.0006)
    double W=4.78, Dp=790, Ds=61, P=0.0006;
    double M = image.rows;
    double N = image.cols;
    vector<double>x, y, z;
    pcl::PointCloud<pcl::PointXYZ>::Ptr basic_cloud_ptr (new pcl::PointCloud<pcl::PointXYZ>);

    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {
            if (image.at<uchar>(i, j) != 0)
            {
cout<<(int)image.at<uchar>(i, j)<<endl;

                double Wn = real_index[(int)image.at<uchar>(i, j)-1] * W;

                double u = i - 0.5*(M + 1);
                double h = j - 0.5*(N + 1);

                x.push_back(Dp - ((Dp*Ds) / (u*P*Dp + Wn)));
                y.push_back((h*P*Dp*Ds) / (u*P*Dp + Wn));
                z.push_back((Wn*Ds) / (u*P*Dp + Wn));

                pcl::PointXYZ basic_point;
                basic_point.x = x.at(x.size()-1);
                basic_point.y = y.at(x.size()-1);
                basic_point.z = z.at(x.size()-1);
                basic_cloud_ptr->points.push_back(basic_point);

                cout<<Dp<<" "
                    <<W<<" "
                    <<Ds<<" "
                    <<P<<" "
                    <<Wn<<" "
                    <<u<<" "
                    <<h<<" "
                    <<i<<" "
                    <<j<<" "
                    <<x.at(x.size()-1)<<" "
                    <<y.at(x.size()-1)<<" "
                    <<z.at(x.size()-1)<<endl;
            }
        }
    }

    pcl::visualization::CloudViewer viewer ("Simple Cloud Viewer");
    viewer.showCloud (basic_cloud_ptr);
    while (!viewer.wasStopped ()){}

    string outFilename = "face3dpointcloud.pcd";
    ofstream outFile(outFilename.c_str());

    if (!outFile)
    {
        cerr << "Error opening output file: " << outFilename << "!" << endl;
        exit(1);
    }

    outFile << "# .PCD v.7 - Point Cloud Data file format" << endl;
    outFile << "VERSION .7" << endl;
    outFile << "FIELDS x y z rgb" << endl;
    outFile << "SIZE 4 4 4 4" << endl;
    outFile << "TYPE F F F F" << endl;
    outFile << "COUNT 1 1 1 1" << endl;
    outFile << "WIDTH " << x.size() << endl;
    outFile << "HEIGHT 1" << endl;
    //outFile << "VIEWPOINT 0 0 0 1 0 0 0" << endl;
    outFile << "POINTS " << x.size() << endl;
    outFile << "DATA ascii" << endl;

    for (int i = 0; i < x.size(); i++)
        outFile << x[i] <<" "<< y[i]<<" " << z[i] << 1111111 << endl;

    return 0;
}
*/