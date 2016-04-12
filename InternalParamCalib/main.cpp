#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision 
#include <sstream>  // string to number conversion 

#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O



using namespace std;
using namespace cv;


// images
Mat inputImg, ScribbleImg, outputImg;

// mask
Mat ScribbleMask;


// user clicked mouse buttons flags
bool rButtonDown = false;
bool lButtonDown = false;
int scribbleRadius = 2;
Point prePt;
Point firstPt;
vector<Point> pointList;
vector<Point> hull;
vector<Point> cartcoord;
int cartCnt = 3;



// mouse listener
static void onMouse( int event, int x, int y, int, void* )
{
	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:
		cout << "On Mouse: (" << x << "," << y << ")" << endl;
		if (!rButtonDown)
		{
			if (!lButtonDown)
			{
				//circle(ScribbleMask, Point(x,y), scribbleRadius, 255, -1);
				circle(ScribbleImg, Point(x,y), scribbleRadius, CV_RGB(255,0,0), -1);
				firstPt = Point(x,y);

				lButtonDown = true;
			}
			else
			{
				//circle(ScribbleMask, Point(x,y), scribbleRadius, 255, -1, CV_AA);
				circle(ScribbleImg, Point(x,y), scribbleRadius, CV_RGB(255,0,0), -1, CV_AA);
				line(ScribbleImg, prePt, Point(x,y), CV_RGB(255,0,0), 2, CV_AA);
			}
		}
		break;
	case CV_EVENT_LBUTTONUP:
		pointList.push_back(Point(x,y));
		prePt = Point(x,y);
		break;
	case CV_EVENT_RBUTTONDOWN:
		if (!rButtonDown)
		{
			convexHull(pointList, hull, true);
			line(ScribbleImg, prePt, firstPt, CV_RGB(255,0,0), 2, CV_AA);
			fillConvexPoly(ScribbleMask, hull, 255, CV_AA);

			/*Mat hullMatrix;
			int size[2] = {0};
			size[0] = hull.size();
			size[1] = 2;
			hullMatrix.create(2, size, CV_16UC1);
			hullMatrix = 0;
			for (int i = 0; i < hull.size(); i++)
			{
				hullMatrix.at<ushort>(i, 0) = hull[i].x;
				hullMatrix.at<ushort>(i, 1) = hull[i].y;
			}
			double xMinval, xMaxval = 0;
			double yMinval, yMaxval = 0;
			minMaxLoc(hullMatrix.col(0), &xMinval, &xMaxval);
			minMaxLoc(hullMatrix.col(1), &xMinval, &xMaxval);*/
			
			rButtonDown = true;
			pointList.clear();
			hull.clear();
		}
		else
		{
			switch(cartCnt)
			{
			case 3:
				cartcoord.push_back(Point(x, y));
				putText(ScribbleImg,"O",Point(x,y), CV_FONT_HERSHEY_COMPLEX, 0.5, CV_RGB(255,0,0));
				cartCnt--;
				break;
			case 2:
				cartcoord.push_back(Point(x, y));
				putText(ScribbleImg,"X",Point(x,y), CV_FONT_HERSHEY_COMPLEX, 0.5, CV_RGB(0,255,0));
				cartCnt--;
				break;
			case 1:
				cartcoord.push_back(Point(x, y));
				putText(ScribbleImg,"Y",Point(x,y), CV_FONT_HERSHEY_COMPLEX, 0.5, CV_RGB(0,0,255));
				cartCnt--;
				break;
			default:
				break;
			}
			
		}
		break;
	case CV_EVENT_RBUTTONUP:

		
		break;
	default:
		break;
	}
	

	imshow("Scribble Image", ScribbleImg);
	imshow("Mask Image", ScribbleMask);
}


// clear everything before closing
void destroyAll()
{
	// destroy all windows
	destroyWindow("Scribble Image");
	destroyWindow("Mask Image");

	// clear all data
	ScribbleMask.release();
	inputImg.release();
	ScribbleImg.release();
}

// init all images/vars
int init(char * imgFileName)
{
	lButtonDown = false;
	hull.clear();
	pointList.clear();
	cartcoord.clear();

	// Read the file
	inputImg = imread(imgFileName, CV_LOAD_IMAGE_COLOR);
	ScribbleImg = inputImg.clone();

	// Check for invalid input
	if(!inputImg.data )                              
	{
		cout <<  "Could not open or find the image: " << imgFileName << endl ;
		return -1;
	}

	// this is the mask to keep the user scribbles
	ScribbleMask.create(2,inputImg.size,CV_8UC1);
	ScribbleMask = 0;

	// Create a window for display.
	namedWindow( "Scribble Image", CV_WINDOW_AUTOSIZE);
	namedWindow( "Mask Image", CV_WINDOW_AUTOSIZE );

	// Show our image inside it.
	imshow("Scribble Image", ScribbleImg);                       
	imshow("Mask Image", ScribbleMask);

	moveWindow("Scribble Image", 1,1);
	moveWindow("Mask Image", inputImg.cols + 50, 1);

	// set the callback on mouse
	setMouseCallback("Scribble Image", onMouse, 0);

	return 0;
}


int main(int argc, char *argv[])
{

	String image_name,numBinsStr,bhaSlopeStr;
	//cout<<"input Parameters:"<<endl;
	//cout<<"image name: ";
	//cin>>image_name;

	// get img name parameter
	char * imgFileName = "fisheye\\k11.jpg";

	if (init(imgFileName)==-1)
	{
		cout <<  "Could not initialize" << endl ;
		return -1;
	}

	// Wait for a keystroke in the window
	for (;;)
	{
		char key = waitKey(0);                          
		switch (key)
		{
		case 'q':
			cout << "goodbye" << endl;
			destroyAll();
			return 0;
		case '-':
			//ËõÐ¡»­±ÊÖ±¾¶
			if (scribbleRadius > 2)
				scribbleRadius --;
			cout << "current radius is " << scribbleRadius << endl;
			break;
		case '+':
			if (scribbleRadius < 100)
				scribbleRadius ++;
			cout << "current radius is " << scribbleRadius << endl;
			break;
		case 's':
			{
				inputImg.copyTo(outputImg, ScribbleMask);
				imwrite("output.jpg",outputImg);
				break;
			}
		case 'r':
			{
				cout << "resetting" << endl;
				destroyAll();
				if (init(imgFileName)==-1)
				{
					cout <<  "could not initialize" << std::endl ;
					return -1;
				}
				break;
			}
		}
	}


	return 0;
}
