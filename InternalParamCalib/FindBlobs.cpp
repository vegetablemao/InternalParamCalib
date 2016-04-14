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
	const contourContainer& findBlobs::findBlobsContours(const Mat& blobImg)
	{
		m_vContours.clear();
		findContours(blobImg, m_vContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		if (m_vContours.empty())
		{
			std::cout << "couldn't find any contours!" << std::endl;
			return m_vContours;
		}
		m_mOutputImg = Mat::zeros(m_uiRows, m_uiCols, CV_8UC3);
		drawContours(m_mOutputImg, m_vContours, -1, CV_RGB(255,255,255));
		imwrite("contours.jpg", m_mOutputImg);
		return m_vContours;
	}
	const centroidContainer& findBlobs::findCentroids(const contourContainer& contours)
	{
		m_vCentroids.clear();

		/// Get the moments
		std::vector<Moments> mu;
		for( int i = 0; i < contours.size(); i++ )
		{ 
			mu.push_back(moments( contours[i], false ));
		}

		///  Get the mass centers:
		for( int i = 0; i < mu.size(); i++ )
		{ 
			if (mu[i].m00 < 1e-8)
			{
				continue;
			}
			int cx = static_cast<int>( mu[i].m10/mu[i].m00 );
			int cy = static_cast<int>( mu[i].m01/mu[i].m00 );
			m_vCentroids.push_back(Point(cx, cy));
			circle(m_mOutputImg, Point(cx, cy), 2, CV_RGB(255,0,0), -1 );
		}
		imwrite("centroids.jpg", m_mOutputImg);
		return m_vCentroids;
	}
}