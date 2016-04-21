#include "FindCalibBoardROI.h"
#include <iostream>
#include <fstream>
#include <direct.h>
#include <io.h>

findCalibROI::findCalibROI()
{

}

findCalibROI::~findCalibROI()
{
	destroyAll();
}

int findCalibROI::init(const std::string& pathName)
{
	std::cout << "Please input the number of blobs in x-direction:";
	std::cin >> m_iBlobsX;
	std::cout << "Please input the number of blobs in y-direction:";
	std::cin >> m_iBlobsY;

	m_sPathName = pathName;
	int postIdx = pathName.rfind('.');
	int preIdx = pathName.rfind('\\');
	m_sImgFileName = pathName.substr(preIdx+1, postIdx-preIdx-1);

	//create the directory to store the Intermediate file
	std::string directory = std::string(SAVE_PATH);
	createDir(directory);

	m_bRButtonDown = false;
	m_bLButtonDown = false;
	m_iscribbleRadius = 2;
	m_iCartCoordCnt = 3;

	m_vPointList.clear();
	m_vHullPointList.clear();
	m_vCartCoord.clear();

	m_mInputImg = cv::imread(pathName, CV_LOAD_IMAGE_COLOR);
	if(!m_mInputImg.data )                              
	{
		std::cout <<  "Could not open or find the image: " << pathName << std::endl ;
		return -1;
	}
	m_uiRows = m_mInputImg.rows;
	m_uiCols = m_mInputImg.cols;
	cv::cvtColor(m_mInputImg, m_mInputGrayImg, CV_BGR2GRAY);//opencv default: BGR
	m_mScribbleImg = m_mInputImg.clone();

	m_mScribbleMask.create(2,m_mInputImg.size, CV_8UC1);
	m_mScribbleMask = 0;

	// Create a window for display.
	cv::namedWindow( "Scribble Image", cv::WINDOW_AUTOSIZE);

	// Show our image inside it.
	cv::imshow("Scribble Image", m_mScribbleImg);
	std::cout << "Click a polygon that encloses the calibration pattern. " << std::endl;

	cv::moveWindow("Scribble Image", 300, 0);

	// set the callback on mouse
	cv::setMouseCallback("Scribble Image", onMouse, this);

	return 0;
}

void findCalibROI::onMouse( int event, int x, int y, int, void* p)
{
	findCalibROI* self = static_cast<findCalibROI*>(p);
	if (!self)
	{
		std::cout << "onMouse::the pass in pointer is invalid" << std::endl;
		return;
	}
	
	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:
		{
			std::cout << "On Mouse: (" << x << "," << y << ")" << std::endl;
			if (!self->m_bRButtonDown)
			{
				self->m_vPointList.push_back(cv::Point(x,y));
				
				if (!self->m_bLButtonDown)
				{
					cv::circle(self->m_mScribbleImg, cv::Point(x,y),
						self->m_iscribbleRadius, CV_RGB(255,0,0), -1, CV_AA);
					self->m_pFirstPt = cv::Point(x,y);
					self->m_bLButtonDown = true;
				}
				else
				{
					//circle(ScribbleMask, Point(x,y), scribbleRadius, 255, -1, CV_AA);
					cv::circle(self->m_mScribbleImg, cv::Point(x,y),
						self->m_iscribbleRadius, CV_RGB(255,0,0), -1, CV_AA);
					cv::line(self->m_mScribbleImg, self->m_pPrePt,
						cv::Point(x,y), CV_RGB(255,0,0), 2, CV_AA);
				}
				self->m_pPrePt = cv::Point(x,y);
			}
			break;
		}
		
	case CV_EVENT_LBUTTONUP:

		break;
	case CV_EVENT_RBUTTONDOWN:
		if (!self->m_bRButtonDown)
		{
			cv::convexHull(self->m_vPointList, self->m_vHullPointList, true);
			cv::line(self->m_mScribbleImg, self->m_pPrePt,
				self->m_pFirstPt, CV_RGB(255,0,0), 2, CV_AA);
			cv::fillConvexPoly(self->m_mScribbleMask, self->m_vHullPointList, 1, CV_AA);

			self->m_bRButtonDown = true;
			self->m_vPointList.clear();
			//self->m_vHullPointList.clear();

		}
		else
		{
			switch(self->m_iCartCoordCnt)
			{
			case 3:
				self->m_vCartCoord.push_back(cv::Point(x,y));
				cv::putText(self->m_mScribbleImg, "o", cv::Point(x,y),
					CV_FONT_HERSHEY_COMPLEX, 0.5, CV_RGB(255,0,0));
				self->m_iCartCoordCnt--;
				break;
			case 2:
				self->m_vCartCoord.push_back(cv::Point(x,y));
				cv::putText(self->m_mScribbleImg, "x", cv::Point(x,y),
					CV_FONT_HERSHEY_COMPLEX, 0.5, CV_RGB(0,255,0));
				self->m_iCartCoordCnt--;
				break;
			case 1:
				self->m_vCartCoord.push_back(cv::Point(x,y));
				cv::putText(self->m_mScribbleImg, "y", cv::Point(x,y),
					CV_FONT_HERSHEY_COMPLEX, 0.5, CV_RGB(0,0,255));
				std::cout << "set succeeded!" << std::endl;
				self->m_iCartCoordCnt--;
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
	

	cv::imshow("Scribble Image", self->m_mScribbleImg);
}

void findCalibROI::destroyAll()
{
	// destroy all windows
	cv::destroyWindow("Scribble Image");

	// clear all data
	m_mScribbleMask.release();
	m_mInputImg.release();
	m_mScribbleImg.release();
	m_mROIImg.release();
	m_mInputGrayImg.release();
}

void findCalibROI::reset()
{
	std::cout << "resetting..." << std::endl;
	destroyAll();
	if (init(m_sPathName) == -1)
	{
		std::cout <<  "could not initialize" << std::endl ;
	}
}

void findCalibROI::nextImage()
{
	std::cout << "please input the name of next image:" << std::endl;
	std::string pathName;
	std::cin >> pathName;
	m_sPathName = pathName;
	destroyAll();
	if (init(m_sPathName) == -1)
	{
		std::cout <<  "could not initialize" << std::endl ;
	}
}

int findCalibROI::createDir(const std::string& directory)
{
	std::string subDir = directory + "\\" + m_sImgFileName;
	m_sSubDir = subDir;
	int ret = _access(directory.c_str(), 0);
	if (-1 == ret)
	{
		if (_mkdir(directory.c_str()) == 0)
		{
			std::cout << "directory" << directory << "has been created successfully!" << std::endl;
		}
		else
		{
			std::cout << "Problem creating directory" << directory << std::endl;
			return 0;
		}
	}

	ret = _access(subDir.c_str(), 0);
	if (_mkdir(subDir.c_str()) == 0)
	{
		std::cout << "subDir" << subDir << "has been created successfully!" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "Problem creating subDir" << subDir << std::endl;
		return 0;
	}
}

void findCalibROI::saveROIImage()
{
	std::cout << "saving ROI image from file "  << m_sSubDir << "\\" <<m_sImgFileName << " ..." << std::endl;

	std::string saveName = m_sSubDir + "\\" + m_sImgFileName + "_ROI.jpg" ;
	m_mInputGrayImg.copyTo(m_mROIImg, m_mScribbleMask);
	cv::imwrite(saveName, m_mROIImg);

	std::cout << "save succeessfully in " << saveName << "!" << std::endl;
}

void findCalibROI::saveBlobsImg()
{
	m_mBlobImg.create(2, m_mInputGrayImg.size, m_mInputGrayImg.type());
	m_mBlobImg = 0;
	
	cv::Rect  bRect = cv::boundingRect(m_vHullPointList);//up-right bounding rectangle
	cv::Mat roiRectImg(m_mROIImg, bRect);//point to the roi rect of the roi image
	
	cv::Mat blobImg(bRect.size(), roiRectImg.type());//create a matrix to save the threshold binary image
	double th = cv::threshold(roiRectImg, blobImg, 255, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);
	std::cout << "the threshold value is:" << th << std::endl;
	cv::Mat roiBImg(m_mBlobImg, bRect);
	blobImg.copyTo(roiBImg);

	std::string saveName = m_sSubDir + "\\" + m_sImgFileName + "_Blobs.jpg" ;
	cv::imwrite(saveName, m_mBlobImg);
}

void findCalibROI::saveXMLFile()
{
	std::string xmlFilename = m_sSubDir + "\\" + m_sImgFileName + "_BlobsAndCartCoord.xml" ;
	cv::FileStorage fs(xmlFilename, cv::FileStorage::WRITE);
	fs << "subDirectory" << m_sSubDir;
	fs << "imageFileName" << m_sImgFileName;
	fs << "blobX" << m_iBlobsX;
	fs << "blobY" << m_iBlobsY;
	fs << "cartMat" << m_vCartCoord;
	fs << "blobsMat" << m_mBlobImg;
	fs.release();
}

void findCalibROI::saveIntermediateFiles()
{
	saveROIImage();
	saveBlobsImg();
	saveXMLFile();
}