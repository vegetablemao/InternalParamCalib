#include "FindCalibBoardROI.h"
#include "FindBlobs.h"
#include <iostream>


void test1(std::string filename)
{
	cv::findBlobs fb;
	findCalibROI& findROI = fb;
	if (findROI.init(filename) == -1)
	{
		std::cout <<  "Could not initialize" << std::endl;
		return;
	}
	// Wait for a keystroke in the window
	for (;;)
	{
		char key = cv::waitKey(0);            
		switch (key)
		{
		case 'q':
			std::cout << "goodbye!!" << std::endl;

			return;

		case 's':
			{
				findROI.saveROIImage();
				const cv::contourContainer &contours = fb.findBlobsContours(findROI.getBlobsImg());
				fb.findCentroids();
				fb.findCentroidGrid();
				//test
				const std::vector<cv::Point> &coord = findROI.getCartCoord();
				if (coord.size() == 3)
				{
					std::cout << "coord[0]:(" << coord[0].x <<","<< coord[0].y <<")"<< std::endl;
					std::cout << "coord[1]:(" << coord[1].x <<","<< coord[1].y <<")"<< std::endl;
					std::cout << "coord[2]:(" << coord[2].x <<","<< coord[2].y <<")"<< std::endl;
				}
				break;
			}
		case 'r':
			{
				findROI.reset();
				break;
			}
		case 'n':
			{
				findROI.nextImage();
				break;
			}
		}
	}
}

void test2(std::string filename)
{
	cv::findBlobs fb;
	cv::Mat blobImg = cv::imread(filename);
	cv::cvtColor(blobImg, blobImg, CV_RGB2GRAY);
	const cv::contourContainer &contours = fb.findBlobsContours(blobImg);
	fb.findCentroids();
	fb.findCentroidGrid();
	//blobImg.convertTo(blobImg, CV_16UC2);
	//std::cout << blobImg.elemSize() << " " << blobImg.elemSize1() << std::endl;
	//std::cout << blobImg.type() << std::endl;
}

int main(int argc, char *argv[])
{

	std::string filename;
	std::cout << "input image filename:" << std::endl;
	std::cin >> filename;


	char * defaultImgFileName = "elead\\k11.jpg";
	test2(filename);

	


	return 0;
}
