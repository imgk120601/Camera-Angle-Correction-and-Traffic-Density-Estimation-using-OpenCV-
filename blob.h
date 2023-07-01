// Blob.h

#ifndef MY_BLOB
#define MY_BLOB

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;
using namespace std;



///////////////////////////////////////////////////////////////////////////////////////////

class MyBlob {
public:
    // member variables ///////////////////////////////////////////////////////////////////////////
    vector<Point> mycurrentContour;

    Rect mycurrentBoundingRect;

    vector<Point> centerPositions;

    double mydblCurrentDiagonalSize;
    double mydblCurrentAspectRatio;

    bool myblnCurrentMatchFoundOrNewBlob;

    bool myblnStillBeingTracked;

    int myintNumOfConsecutiveFramesWithoutAMatch;

    Point mypredictedNextPosition;
    
    // function prototypes ////////////////////////////////////////////////////////////////////////
    MyBlob(vector<Point> mycontour);
    void mypredictNextPosition(void);

};

#endif    // MY_BLOB



