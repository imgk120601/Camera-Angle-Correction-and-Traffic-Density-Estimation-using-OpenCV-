#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


struct userdata{
    Mat im;
    vector<Point2f> points;
};

#include "blob.h" //////it has dependency on function.h i.e  header file


void mouseHandler(int event, int x, int y, int flags, void* data_ptr)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        userdata *data = ((userdata *) data_ptr);
        circle(data->im, Point(x,y),3,Scalar(0,0,255), 5, cv::LINE_AA);
        imshow("Image", data->im);
        if (data->points.size() < 4)
        {
            data->points.push_back(Point2f(x,y));
        }
    }
}
