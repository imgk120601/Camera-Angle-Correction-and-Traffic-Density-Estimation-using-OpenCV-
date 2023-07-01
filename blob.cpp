// Blob.cpp

#include "blob.h"
using namespace cv;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
MyBlob::MyBlob(vector<Point> mycontour) {

    mycurrentContour = mycontour;

    mycurrentBoundingRect = boundingRect(mycurrentContour);//it is an function which make rect aroud around
    // our contour

    Point mycurrentCenter;

    mycurrentCenter.x = (mycurrentBoundingRect.x + mycurrentBoundingRect.x + mycurrentBoundingRect.width) / 2;
    mycurrentCenter.y = (mycurrentBoundingRect.y + mycurrentBoundingRect.y + mycurrentBoundingRect.height) / 2;

    centerPositions.push_back(mycurrentCenter);

    mydblCurrentDiagonalSize = sqrt(pow(mycurrentBoundingRect.width, 2) + pow(mycurrentBoundingRect.height, 2));

    mydblCurrentAspectRatio = (float)mycurrentBoundingRect.width / (float)mycurrentBoundingRect.height;

    myblnStillBeingTracked = true;
    myblnCurrentMatchFoundOrNewBlob = true;

    myintNumOfConsecutiveFramesWithoutAMatch = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MyBlob::mypredictNextPosition(void) {

    int numPositions = (int)centerPositions.size();

    if (numPositions == 1) {

        mypredictedNextPosition.x = centerPositions.back().x;
        mypredictedNextPosition.y = centerPositions.back().y;

    }
    else if (numPositions == 2) {

        int deltaX = centerPositions[1].x - centerPositions[0].x;
        int deltaY = centerPositions[1].y - centerPositions[0].y;

        mypredictedNextPosition.x = centerPositions.back().x + deltaX;
        mypredictedNextPosition.y = centerPositions.back().y + deltaY;

    }
    else if (numPositions == 3) {

        int sumOfXChanges = ((centerPositions[2].x - centerPositions[1].x) * 2) +
            ((centerPositions[1].x - centerPositions[0].x) * 1);

        int deltaX = (int)std::round((float)sumOfXChanges / 3.0);

        int sumOfYChanges = ((centerPositions[2].y - centerPositions[1].y) * 2) +
            ((centerPositions[1].y - centerPositions[0].y) * 1);

        int deltaY = (int)std::round((float)sumOfYChanges / 3.0);

        mypredictedNextPosition.x = centerPositions.back().x + deltaX;
        mypredictedNextPosition.y = centerPositions.back().y + deltaY;

    }
    else if (numPositions == 4) {

        int sumOfXChanges = ((centerPositions[3].x - centerPositions[2].x) * 3) +
            ((centerPositions[2].x - centerPositions[1].x) * 2) +
            ((centerPositions[1].x - centerPositions[0].x) * 1);

        int deltaX = (int)std::round((float)sumOfXChanges / 6.0);

        int sumOfYChanges = ((centerPositions[3].y - centerPositions[2].y) * 3) +
            ((centerPositions[2].y - centerPositions[1].y) * 2) +
            ((centerPositions[1].y - centerPositions[0].y) * 1);

        int deltaY = (int)std::round((float)sumOfYChanges / 6.0);

        mypredictedNextPosition.x = centerPositions.back().x + deltaX;
        mypredictedNextPosition.y = centerPositions.back().y + deltaY;

    }
    else if (numPositions >= 5) {

        int sumOfXChanges = ((centerPositions[numPositions - 1].x - centerPositions[numPositions - 2].x) * 4) +
            ((centerPositions[numPositions - 2].x - centerPositions[numPositions - 3].x) * 3) +
            ((centerPositions[numPositions - 3].x - centerPositions[numPositions - 4].x) * 2) +
            ((centerPositions[numPositions - 4].x - centerPositions[numPositions - 5].x) * 1);

        int deltaX = (int)std::round((float)sumOfXChanges / 10.0);

        int sumOfYChanges = ((centerPositions[numPositions - 1].y - centerPositions[numPositions - 2].y) * 4) +
            ((centerPositions[numPositions - 2].y - centerPositions[numPositions - 3].y) * 3) +
            ((centerPositions[numPositions - 3].y - centerPositions[numPositions - 4].y) * 2) +
            ((centerPositions[numPositions - 4].y - centerPositions[numPositions - 5].y) * 1);

        int deltaY = (int)std::round((float)sumOfYChanges / 10.0);

        mypredictedNextPosition.x = centerPositions.back().x + deltaX;
        mypredictedNextPosition.y = centerPositions.back().y + deltaY;

    }
    else {
        // should never get here
    }

}





