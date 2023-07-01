// ObjectTrackingCPP.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include "csvfile.h"

#include "blob.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include<vector>

#define SHOW_STEPS            // un-comment or comment this line to show steps or not
using namespace cv;
using namespace std;
using namespace std::chrono;
//////////////////////////////////////////////////////
struct userdata{
    Mat im;
    vector<Point2f> points;
};

struct bs_struct {
	int noofsplit;
	int splitno;
	//int mycouncounter;//FOR METHOD-4 for diff -diff frame--for diff-phreads
};

vector<Point2f> pts_dst = {Point2f(0,0),Point2f(327,0),Point2f(327,777),Point2f(0,777)};

 //--SKIPING THE PROCESS OF CHOOSING
vector<Point2f> src_dst= {Point2f(978,211),Point2f(1308,210),Point2f(1602,1005),Point2f(360,1009)};
 Mat matrix= getPerspectiveTransform(src_dst,pts_dst);




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

///////////////////////////////////////////////////////////////////
struct Picture{
    Mat im;
    vector<Point2f> points;
};

//FOR METHOD--3 FOR USING DATA BETWEEN THREADS
int tempStaticqueueDensity;
int tempcurrentNoOfBlobs;
int myframepartcount;

// function prototypes ////////////////////////////////////////////////////////////////////////////
void mymatchCurrentFrameBlobsToExistingBlobs(vector<MyBlob> &existingBlobs, vector<MyBlob> &currentFrameBlobs);
void myaddBlobToExistingBlobs(MyBlob &currentFrameBlob, vector<MyBlob> &existingBlobs, int &intIndex);
void myaddNewBlob(MyBlob &currentFrameBlob, vector<MyBlob> &existingBlobs);
double distanceBetweenPoints(Point point1, Point point2);


///////////////////////////////////////////////////////////////////////////////////////////////////
void* mybsfunc (void* arg){

	struct bs_struct *arg_struct =
			(struct bs_struct*) arg;


   ///METHOD--3
   //int *limit_ptr = (int*) arg; //taking argument given to us and putting it in limit value
   //float mynoofsplit =(float)arg_struct->noofsplit ; // extracting information from our  structure
   
   //float mysplitno=(float)arg_struct->splitno; 	//extracting information from our structure
 
 
 
 //METHOD---4
  int mynoofsplit =(int)arg_struct->noofsplit ; // extracting information from our  structure
   
   int mysplitno=(int)arg_struct->splitno; 	//extracting information from our structure
  // int mymycouncounter=(int) arg_struct->mycouncounter;
   
   
   VideoCapture mycapVideo;

    Mat myimgFrame1;
    
    Mat myimgFrame2;
    
    vector<MyBlob> myblobs;

    mycapVideo.open("trafficvideo.mp4");

    if (!mycapVideo.isOpened()) {                                                 // if unable to open video file
        cout << "\nerror in  reading video file--check your video " << endl;      // show error message

        return(0);                                                              // and exit program
    }
    mycapVideo.read(myimgFrame1);
    mycapVideo.read(myimgFrame2);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //here we will get our matrix
    
                        csvfile csv("MyTable.csv"); // throws exceptions!
       
        		csv << "frameCount" << "StaticqueueDensity" << "currentNoOfBlobs" << endrow;
   
  
   /* 
    Picture img;
    img.im = myimgFrame1;

    cout << "Click on the four corners of the road" << endl;
    imshow("Image", myimgFrame1);//here we will show our first frame
    
    setMouseCallback("Image", mouseHandler, &img);//from first frame he will choose the point
    waitKey(0);
    
    for(int i=0;i<4;i++){
    	cout<<img.points[i] <<endl;
    
    }

    vector<Point2f> pts_dst = {Point2f(0,0),Point2f(327,0),Point2f(327,777),Point2f(0,777)};
     Mat matrix;
    
    
    if(img.points.size()<=0){
   	 cout<<img.points.size()<<endl;
    	return(0);
    }
    
    
    matrix= getPerspectiveTransform(img.points,pts_dst);
    
*/
/////////////////////////////////////////////////////////////////////////////////
    warpPerspective(myimgFrame1,myimgFrame1, matrix, Point(327,777) );
    warpPerspective(myimgFrame2,myimgFrame2, matrix, Point(327,777) );
    
    
    
    //this is for part c of assignment--------------
    float myimgheight= myimgFrame1.size().height;
    float myimgwidth= myimgFrame2.size().width;
    
    
    float heightpersplit= myimgheight/(float)mynoofsplit;
    
    Rect roi;      //we are croping accoring to noofsplit  and split no.
    roi.x=0.0;
    roi.y=0.0 + mysplitno*heightpersplit ;///we have define these thing ones only then use roi afterward
    roi.width= myimgwidth;
    roi.height= heightpersplit;
    
    myimgFrame1=myimgFrame1(roi);   //we have croped out here
    myimgFrame2=myimgFrame2(roi);
   
    ///////////////////////////////
    
   // waitKey(0);
/////////////////////////////////////////////////////////////////////////////////////////


    char CheckEscKey = 0;

    bool myblnFirstFrame = true;

    int frameCount = 1;
    
    int StaticqueueDensity=0;
    int currentNoOfBlobs=0;
     int RealcurrentNoOfBlobs=0;//for static density
 

    while (mycapVideo.isOpened() && CheckEscKey != 27) {
    
    	frameCount++;//here we have have readed two frame and incresing every time
    	
    	vector<MyBlob> mycurrentFrameBlobs; //for measuring current number of blobs
        vector<MyBlob> RealcurrentFrameBlobs;//for the static case

        Mat myimgFrame1Copy = myimgFrame1.clone();//making copy
        Mat myimgFrame2Copy = myimgFrame2.clone();//making copy

        Mat myimgDifference;
        Mat myimgThresh;

        cvtColor(myimgFrame1Copy, myimgFrame1Copy, COLOR_BGR2GRAY);//convert copied into gray scale
        cvtColor(myimgFrame2Copy, myimgFrame2Copy, COLOR_BGR2GRAY);//convert copied into grey scale

        GaussianBlur(myimgFrame1Copy, myimgFrame1Copy, Size(5, 5), 0);//creating blur
        GaussianBlur(myimgFrame2Copy, myimgFrame2Copy, Size(5, 5), 0);//creating blur

        absdiff(myimgFrame1Copy, myimgFrame2Copy, myimgDifference);//this will take the differernce of two img and put
        //into myimgDifference

        threshold(myimgDifference, myimgThresh, 30, 255.0, 0);//thershing of the mydifference

                                                                                           
        Mat kernel = getStructuringElement(cv::MORPH_RECT, Size(5, 5)); // creating three structure for dilatation
       
        dilate(myimgThresh, myimgThresh, kernel);//dilation of our image
                
        Mat myimgThreshCopy = myimgThresh.clone();//make clonne of my mytheresh image

        vector<vector<Point> > mycontours;//this will store my contour boundary

        findContours(myimgThreshCopy, mycontours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);//finding contour boundary on mythershcopy 
        // and putting it in my mycontours vector list
        
        vector<vector<Point> > myconvexHulls(mycontours.size());

        for (unsigned int i = 0; i < mycontours.size(); i++) {
            convexHull(mycontours[i], myconvexHulls[i]);//putting all my contour boundary in convexHulls vector list
        }


        for (auto &myconvexHull : myconvexHulls) {//seraching in my convexhulls and removing noisees(small contours)
            MyBlob mypossibleBlob(myconvexHull);    //created a blob variable myPossibleBlob which take value myconvexHulls


		//METHOD--3 BOUNDARY CONDITIONS
            if (mypossibleBlob.mycurrentBoundingRect.area() > 500 &&
                mypossibleBlob.mydblCurrentAspectRatio >= 0.2 &&
                mypossibleBlob.mydblCurrentAspectRatio <= 1.25 &&
                mypossibleBlob.mycurrentBoundingRect.width > 20 &&
                mypossibleBlob.mycurrentBoundingRect.height > 20 &&
                mypossibleBlob.mydblCurrentDiagonalSize > 30.0 &&
                (contourArea(mypossibleBlob.mycurrentContour) / (double)mypossibleBlob.mycurrentBoundingRect.area()) > 0.40) {
                
                mycurrentFrameBlobs.push_back(mypossibleBlob);//only pushing blobs which are of some use
            }  
            //for the static case
            if (mypossibleBlob.mycurrentBoundingRect.area() > 500 &&
                mypossibleBlob.mydblCurrentAspectRatio >= 0.2 &&
                mypossibleBlob.mydblCurrentAspectRatio <= 1.25 &&
                mypossibleBlob.mycurrentBoundingRect.width > 20 &&
                mypossibleBlob.mycurrentBoundingRect.height > 20 &&
                mypossibleBlob.mydblCurrentDiagonalSize > 30.0 &&
                (contourArea(mypossibleBlob.mycurrentContour) / (double)mypossibleBlob.mycurrentBoundingRect.area()) > 0.40
                
                // &&//----changes for part -3///
                //mypossibleBlob.mycurrentBoundingRect.y > roi.y &&
                //mypossibleBlob.mycurrentBoundingRect.y < 600 //if our contour is abobe this y then only we will count this--in static
               
               
                //when the new contours it will affect only dynameic not static
  
                ) {//when it depart no. of static not affected
                //check only b/w these y range
                RealcurrentFrameBlobs.push_back(mypossibleBlob);
            }
        }

        //drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs");

        if (myblnFirstFrame == true) {
            for (auto &mycurrentFrameBlob : mycurrentFrameBlobs) {
                myblobs.push_back(mycurrentFrameBlob);
            }
        }
        else {
            mymatchCurrentFrameBlobsToExistingBlobs(myblobs, mycurrentFrameBlobs);
        }
        myimgFrame2Copy = myimgFrame2.clone();          // get another copy of frame 2 since we changed the previous frame 2 copy in the processing above



	for (unsigned int i = 0; i < myblobs.size(); i++) {

        if (myblobs[i].myblnStillBeingTracked == true) {//make a ractangle around blobs which are still tracked
            rectangle(myimgFrame2Copy, myblobs[i].mycurrentBoundingRect, Scalar(0.0, 0.0, 255.0), 2);
             }
       }

        //drawBlobInfoOnImage(blobs, imgFrame2Copy)
        
        
        imshow("imgFrame2Copy", myimgFrame2Copy);///this is our actual video

        //cv::waitKey(0);                 // uncomment this line to go frame by frame for debugging

                // now we prepare for the next iteration
///////////////////////////////////////////////////////////////////////////////////////
//here we will calculate static queue density--------------------------------
    	 
	  int previousNoOfBlobs=currentNoOfBlobs;
	  int RealpreviousNoOfBlobs=RealcurrentNoOfBlobs;//for staitcqueque-------
         ////////////////////////////////////////////////////////////////////////
         //calculation no. of current blob as a measure of dynamic density.//taking data at every fitth frame
         
         if(frameCount%10==0){//METHOD-3 FOR ALL frames  //for METHOD--4//for frame-thread analysis and process
         		 currentNoOfBlobs=mycurrentFrameBlobs.size();
         		 
         		 RealcurrentNoOfBlobs=RealcurrentFrameBlobs.size();//because we want to measure static in some range of 
         		 // y only---so that when vehicle just enter and just escaped does not affect our static
                       if( RealpreviousNoOfBlobs> RealcurrentNoOfBlobs){//because density can't be negative
                       	StaticqueueDensity=StaticqueueDensity+previousNoOfBlobs - currentNoOfBlobs;
                       }else{
                        
                        	StaticqueueDensity=StaticqueueDensity + RealpreviousNoOfBlobs - RealcurrentNoOfBlobs;
                        
                        }
                        if(StaticqueueDensity<0 ){//static queue density cannot be negative
                        StaticqueueDensity=0;
                        }
                        
                        
                        if(StaticqueueDensity>(int)float(15/(float)mynoofsplit)){//static queue density cannot be more than 17(observation)
                        	StaticqueueDensity=(int)((float)(15/(float)mynoofsplit));
                        
                        }
                        
                        
                        tempStaticqueueDensity=tempStaticqueueDensity+StaticqueueDensity;
    			tempcurrentNoOfBlobs=tempcurrentNoOfBlobs+currentNoOfBlobs;
    			
                      if(tempStaticqueueDensity>15){//static queue density cannot be more than 17(observation)
                        	tempStaticqueueDensity=15;
                        
                        }
    			
    			if(mysplitno==mynoofsplit-1){//here we are adding StaticqueuDensity and currentNoOfBlobs of all----
    			//---parts of the frames
    			  
    			  		
    			  
    			  csv << to_string(frameCount) << to_string(tempStaticqueueDensity) << to_string(tempcurrentNoOfBlobs) <<endrow;
 
    			  /*
    			  ---FOR MY CONVINENCE
         			cout<<  to_string(mynoofsplit)+"," +to_string(mysplitno)+"," +to_string(frameCount)+","+ to_string(tempStaticqueueDensity)+","+to_string(tempcurrentNoOfBlobs)<<endl;
         			
         			
         		*/	
         			
                        	tempStaticqueueDensity=0;
    				tempcurrentNoOfBlobs=0;
    			
    			
    			}
                       
                        
                        
                      
         		
         		
         		//return(0);//exiting thread after every frame 
         
         
         
         
         
         }
         ///////////////////////////////////////////////////////////       
               
        mycurrentFrameBlobs.clear();
        RealcurrentFrameBlobs.clear();//for staic queue
        
        ///////////////////////////////////////////////////////////////////////////////////
        myimgFrame1 = myimgFrame2.clone();           // move frame 1 up to where frame 2 is
	
	mycapVideo.read(myimgFrame2);	           // start your journey from imgFrame2//here we gone to next frame and put it into "imagFrame2" image.....
 	
		  ////////////////////////////////////////////////////////////////////////////////
//new addition

	if(!myimgFrame2.empty()){//last frame is giving some problem --in wrap perspective func ---"_src.total()>0"
	//that is why i have excluded last frmae
	       warpPerspective(myimgFrame2,myimgFrame2, matrix, Point(327,777) );
	}else{
	
	     return(0);
	}
	   
	   
	   ///for part c 
	   
	   myimgFrame2=myimgFrame2(roi);  ///here croping the image for particular frame//and doing stuff
	   
	   
	   
	   
	   //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

        myblnFirstFrame = false;//declaring that we have consumed 1 first frame         
        CheckEscKey = waitKey(30);
    }

    if (CheckEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
        waitKey(0);                         // hold the windows open to allow the "end of video" message to show
    }
    // note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows





	pthread_exit(0);
	//NO HARM IN UNCOMMENTING THIS
}





int main(int argc, char **argv) {//taking input from user
	if (argc < 2) {
		printf("METHOD-3/METHOD-4 Give argument --numberofpthreads/no of split of our frameUsage");
	
		printf("Usage: %s <num>\n", argv[0]);
		exit(-1);
	}

	//METHOD--3
	int noofthreads = stoi(argv[1]);//convert string argument into integer value
	auto start = high_resolution_clock::now();
	
	struct bs_struct args[noofthreads];//making a list(args) of our bs_struct structure 
	
	
	int counter=2;//for mycouncounter
        // Launch thread
	pthread_t tids[noofthreads];
	while(counter<=2){
	
	for (int i = 0; i < noofthreads; i++) {
		args[i].splitno = i;//split number will tell of which split part u refering to
		args[i].noofsplit=noofthreads; //no of split is equal to number of threads
		//args[i].mycouncounter=counter;

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_create(&tids[i], &attr, mybsfunc, &args[i]);  //our atrribute is that struvture and it contain two parameter one is 
		//pthread_join(tids[i], NULL);
		//no of split of the frame and other is which splitno is refering to here of the frame //and
		//and at ithe splitno ith phtread is working
		
		counter++;
		//each time our counter -value increasing by one thus increasing our .mycouncounter(use for frame-by-thread process)
	}
	
	
	}

	// Wait until thread is done its work
	for (int i = 0; i < noofthreads; i++) {
		pthread_join(tids[i], NULL);
		//printf("Sum for thread %d is %lld\n",
		//		i, args[i].answer);
	}
	
	
	
	
	/*
	//pthread_t tid;

	// Create attributes
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	pthread_create(&tid, &attr, mybsfunc, &limit);//just making another thread and its function is our mybsfunc
	//and passing an argument

	// Do other stuff here...

	// Wait until thread is done its work
	pthread_join(tid, NULL);
	//printf("Sum is %lld\n", sum);
	
	*/
	
	auto stop = high_resolution_clock::now();
  
    auto duration = duration_cast<microseconds>(stop - start);
  
    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;
  
	
	exit(0);
	
	


   
    return(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mymatchCurrentFrameBlobsToExistingBlobs(vector<MyBlob> &existingBlobs, vector<MyBlob> &currentFrameBlobs) {

    for (auto &existingBlob : existingBlobs) {

        existingBlob.myblnCurrentMatchFoundOrNewBlob = false;

        existingBlob.mypredictNextPosition();
    }

    for (auto &currentFrameBlob : currentFrameBlobs) {

        int intIndexOfLeastDistance = 0;
        double dblLeastDistance = 100000.0;

        for (unsigned int i = 0; i < existingBlobs.size(); i++) {
            if (existingBlobs[i].myblnStillBeingTracked == true) {
                double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].mypredictedNextPosition);

                if (dblDistance < dblLeastDistance) {
                    dblLeastDistance = dblDistance;
                    intIndexOfLeastDistance = i;
                }
            }
        }

        if (dblLeastDistance < currentFrameBlob.mydblCurrentDiagonalSize * 1.15) {
            myaddBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
        }
        else {
            myaddNewBlob(currentFrameBlob, existingBlobs);
        }

    }

    for (auto &existingBlob : existingBlobs) {

        if (existingBlob.myblnCurrentMatchFoundOrNewBlob == false) {
            existingBlob.myintNumOfConsecutiveFramesWithoutAMatch++;
        }

        if (existingBlob.myintNumOfConsecutiveFramesWithoutAMatch >= 5) {
            existingBlob.myblnStillBeingTracked = false;
        }

    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void myaddBlobToExistingBlobs(MyBlob &mycurrentFrameBlob, vector<MyBlob> &existingBlobs, int &intIndex) {

    existingBlobs[intIndex].mycurrentContour = mycurrentFrameBlob.mycurrentContour;
    existingBlobs[intIndex].mycurrentBoundingRect = mycurrentFrameBlob.mycurrentBoundingRect;

    existingBlobs[intIndex].centerPositions.push_back(mycurrentFrameBlob.centerPositions.back());

    existingBlobs[intIndex].mydblCurrentDiagonalSize = mycurrentFrameBlob.mydblCurrentDiagonalSize;
    existingBlobs[intIndex].mydblCurrentAspectRatio = mycurrentFrameBlob.mydblCurrentAspectRatio;

    existingBlobs[intIndex].myblnStillBeingTracked = true;
    existingBlobs[intIndex].myblnCurrentMatchFoundOrNewBlob = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void myaddNewBlob(MyBlob &mycurrentFrameBlob, vector<MyBlob> &existingBlobs) {

    mycurrentFrameBlob.myblnCurrentMatchFoundOrNewBlob = true;

    existingBlobs.push_back(mycurrentFrameBlob);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double distanceBetweenPoints(Point point1, Point point2) {
    
    int intX = abs(point1.x - point2.x);
    int intY = abs(point1.y - point2.y);

    return(sqrt(pow(intX, 2) + pow(intY, 2)));
}









































