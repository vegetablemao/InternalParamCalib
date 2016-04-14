#include "FindBlobs.h"
#include <iostream>

namespace cv
{
	findBlobs::findBlobs()
	{

	}

	findBlobs::~findBlobs() {}

	int findBlobs::init(const std::string& imgFileName)
	{
		m_vContours.clear();
		return findCalibROI::init(imgFileName);

	}
	contourContainer& findBlobs::findBlobsContours(const Mat& blobImg)
	{
		m_vContours.clear();
		findContours(blobImg, m_vContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		if (m_vContours.empty())
		{
			std::cout << "couldn't find any contours!" << std::endl;
			return m_vContours;
		}
		Mat dst = Mat::zeros(m_uiRows, m_uiCols, CV_8UC1);
		drawContours(dst, m_vContours, -1, 255);
		imwrite("contours.jpg", dst);
		return m_vContours;
	}
}