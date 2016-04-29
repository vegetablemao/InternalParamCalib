#include "FindCalibBoardROI.h"
#include "FindBlobs.h"
#include "calibrate.h"
#include <iostream>
#include <io.h>



//get all file names under the path.
void dir(std::string path, std::vector<std::string>& filenames)
{
	long hFile = 0;
	struct _finddata_t fileInfo;
	std::string pathName;
	filenames.clear();

	if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1) {
		return;
	}
	do {
		if (!(fileInfo.attrib&_A_SUBDIR))
		{
			//std::cout << fileInfo.name << std::endl;
			filenames.push_back(fileInfo.name);
		}
		//std::cout << fileInfo.name << (fileInfo.attrib&_A_SUBDIR? "[folder]":"[file]") << std::endl;
	} while (_findnext(hFile, &fileInfo) == 0);
	_findclose(hFile);
	return;
}


void test1(std::string pathName)
{
	std::vector<std::string> fileNames;
	std::vector<std::string> pathes;
	std::vector<std::string> storeNames;
	dir(pathName, fileNames);

	std::string path;
	std::string store;
	for (int i = 0; i < fileNames.size(); i++)
	{
		path.clear();
		path.assign(pathName).append("\\").append(fileNames[i]);
		pathes.push_back(path);

		store.clear();
		int idx = fileNames[i].find('.');
		store.assign(fileNames[i].substr(0, idx));
		storeNames.push_back(store);
	}


	cv::FileStorage fs(".\\fileNames.xml", cv::FileStorage::WRITE);
	fs << "fileNames" << storeNames;
	fs.release();

	int cnt = 0;
	//cv::findBlobs fb;
	findCalibROI findROI;
	if (findROI.init(pathes[cnt++]) == -1)
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
				if (cnt >= pathes.size())
				{
					std::cout << "all images have been enclosed!" << std::endl;
					return;
				}
				findROI.nextImage(pathes[cnt++]);
				break;
			}
		}
	}
}

void test2(std::string pathName)
{
	std::vector<std::string> FileNames;
	FileNames.clear();
	cv::FileStorage fs0(".\\fileNames.xml", cv::FileStorage::READ);
	fs0["fileNames"] >> FileNames;

	std::vector<std::string> pathes;
	std::string path;
	for (int i = 0; i < FileNames.size(); i++)
	{
		path.clear();
		path.assign(pathName).append("\\").append(FileNames[i]).append("\\").append(FileNames[i]).append("_BlobsAndCartCoord.xml");
		pathes.push_back(path);
	}

	int cnt = 0;
	cv::findBlobs fb;
	cv::FileStorage fs;
	fs.open(pathes[cnt++], cv::FileStorage::READ);
	fb.findBlobsContours(fs);
	fb.findCentroids();
	fb.findCentroidGrid();
	fb.blobvis();

	
	for (;;)
	{
		char key = cv::waitKey(0);
		if (key == 'q')
		{
			std::cout << "goodbye!!" << std::endl;
			return;
		}
		else if (key == 'n')
		{
			if (cnt < pathes.size())
			{
				fs.open(pathes[cnt++], cv::FileStorage::READ);
				fb.findBlobsContours(fs);
				fb.findCentroids();
				fb.findCentroidGrid();
				fb.blobvis();
			}
			else
				return;

		}
	}
}


void test3(std::string pathName)
{
	char* configPath = ".\\fisheye_config_\\config.xml";
	cv::calibrate calib(configPath);
	calib.readCentroidsGrid(pathName);

}

int main(int argc, char *argv[])
{
	//char * defaultBlobsFileName = "elead_intermediate_\\k13\\k13_BlobsAndCartCoord.xml";
	//char * defaultInputFileName = "elead\\k11.jpg";
	//char * defaultCentGridsFileName = "elead_intermediate_\\k11\\k11_centroidsGrid.xml";
	const char * defaultPath1 = ".\\fisheye";
	const char * defaultPath2 = ".\\fisheye_intermediate_";

	/*std::string filename;
	std::cout << "input image filename:" << std::endl;
	std::cin >> filename;*/
	//std::vector<std::string> fileNames;
	//dir(defaultPath, fileNames);

	int testNum = 0;
	std::cout << "input the test case number:" << std::endl;
	std::cin >> testNum;
	switch (testNum)
	{
	case 1:
		test1(defaultPath1);
		break;
	case 2:
		test2(defaultPath2);
		break;
	case 3:
		test3(defaultPath2);
	default:
		break;
	}

	return 0;
}
