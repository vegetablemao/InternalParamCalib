#include "FindCalibBoardROI.h"
#include "FindBlobs.h"
#include <iostream>


void test1(std::string filename)
{
	//cv::findBlobs fb;
	findCalibROI findROI;
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
				findROI.saveIntermediateFiles();
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
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	//cv::Mat blobImg;
	//fs["blobsMat"] >> blobImg;
	//cv::cvtColor(blobImg, blobImg, CV_RGB2GRAY);
	const cv::contourContainer &contours = fb.findBlobsContours(fs);
	fb.findCentroids();
	fb.findCentroidGrid();
	//blobImg.convertTo(blobImg, CV_16UC2);
	//std::cout << blobImg.elemSize() << " " << blobImg.elemSize1() << std::endl;
	//std::cout << blobImg.type() << std::endl;
}

int main(int argc, char *argv[])
{

	/*std::string filename;
	std::cout << "input image filename:" << std::endl;
	std::cin >> filename;*/


	char * defaultBlobsFileName = "elead_intermediate_\\k11\\k11_BlobsAndCartCoord.xml";
	char * defaultInputFileName = "elead\\k11.jpg";
	//test1(defaultInputFileName);
	test2(defaultBlobsFileName);

	


	return 0;
}
