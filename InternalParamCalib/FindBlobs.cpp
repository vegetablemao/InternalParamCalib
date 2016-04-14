#include "FindBlobs.h"
#include <iostream>
#include <algorithm>

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
		int numOfBlobs = m_iBlobsX * m_iBlobsY;
		std::sort(m_vContours.begin(), m_vContours.end(), greaterMark); //sort the contours from big to small, we only need the biggest numOfBlobs contours.
		m_vContours.erase(m_vContours.begin()+numOfBlobs, m_vContours.end()); //erase the smallest contours

		drawContours(m_mOutputImg, m_vContours, -1, CV_RGB(255,255,255));
		imwrite("contours.jpg", m_mOutputImg);
		return m_vContours;
	}
	const centroidContainer& findBlobs::findCentroids(/*const contourContainer& contours*/)
	{
		m_vCentroids.clear();

		/// Get the moments
		std::vector<Moments> mu;
		for( int i = 0; i < m_vContours.size(); i++ )
		{ 
			mu.push_back(moments( m_vContours[i], false ));
		}

		///  Get the mass centers:
		for( int i = 0; i < mu.size(); i++ )
		{ 
			int cx = static_cast<int>( mu[i].m10/mu[i].m00 );
			int cy = static_cast<int>( mu[i].m01/mu[i].m00 );
			m_vCentroids.push_back(Point(cx, cy));
			circle(m_mOutputImg, Point(cx, cy), 2, CV_RGB(255,0,0), -1 );
		}
		imwrite("centroids.jpg", m_mOutputImg);
		return m_vCentroids;
	}

}