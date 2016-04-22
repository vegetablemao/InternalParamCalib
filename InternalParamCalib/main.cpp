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

	fb.findBlobsContours(fs);
	fb.findCentroids();
	fb.findCentroidGrid();
	fb.blobvis();

	char key = cv::waitKey(0);
	if (key == 'q')
	{
		std::cout << "goodbye!!" << std::endl;
	}

}

int main(int argc, char *argv[])
{
	char * defaultBlobsFileName = "elead_intermediate_\\k13\\k13_BlobsAndCartCoord.xml";
	char * defaultInputFileName = "elead\\k11.jpg";

	/*std::string filename;
	std::cout << "input image filename:" << std::endl;
	std::cin >> filename;*/

	int testNum = 0;
	std::cout << "input the test case number:" << std::endl;
	std::cin >> testNum;
	switch (testNum)
	{
	case 1:
		test1(defaultInputFileName);
		break;
	case 2:
		test2(defaultBlobsFileName);
		break;
	default:
		break;
	}

	return 0;
}
