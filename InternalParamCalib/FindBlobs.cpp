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
			if (mu[i].m00  > 1e-15)
			{
				double dx = mu[i].m10/mu[i].m00;
				double dy = mu[i].m01/mu[i].m00;
				m_vCentroids.push_back(Point2d(dx, dy));
				int cx = static_cast<int>( dx );
				int cy = static_cast<int>( dy );
				circle(m_mOutputImg, Point(cx, cy), 2, CV_RGB(255,0,0), -1 );
			}
		}
		imwrite("centroids.jpg", m_mOutputImg);
		return m_vCentroids;
	}

	void findBlobs::DeleteOneColOfMat(Mat& object,int col)
	{
		if (col<0 || col >= object.cols)
		{ 		
			std::cout << col << "is out of mat range" << std::endl;
			return;
		}
		if (col == object.cols - 1)
		{
			object = object.t();
			object.pop_back();
			object = object.t();
		}
		else
		{
			for (int i = col+1; i < object.cols; i++) 
			{ 				
				object.col(i-1) = object.col(i); 	
			} 			
			object = object.t();
			object.pop_back();
			object = object.t();
		}
	}

	int findBlobs::findNearestPointInMatrix(const Point& ip, const Mat& mc)
	{
		int numOfCol = mc.cols;
		Mat mip = Mat(ip);
		mip.convertTo(mip, CV_64FC1);
		Mat ipMat = mip * Mat::ones(1,numOfCol, mip.type());
		subtract(ipMat, mc, ipMat);
		pow(ipMat, 2, ipMat);
		
		std::vector<double> dist;
		dist.clear();
		for (int i = 0; i < numOfCol; i++)
		{
			Scalar s = sum(ipMat.col(i));
			dist.push_back(s[0]);
		}
		std::vector<double>::iterator minIter = std::min_element(std::begin(dist), std::end(dist));
		int mind = std::distance(std::begin(dist), minIter);

		return mind;

	}

	const Mat& findBlobs::findCentroidGrid()
	{
		m_mCentroidGrid.create(m_iBlobsX, m_iBlobsY, CV_64FC2);
		m_mCentroidGrid = 0;
		
		Point origin = m_vCartCoord[0];
		Point x1 = m_vCartCoord[1];
		Point y1 = m_vCartCoord[2];
		Mat mc = Mat(m_vCentroids).reshape(1).t();

		//find the left top three points in the centroids.
		int mind = findNearestPointInMatrix(origin, mc);
		Mat cco = mc.col(mind);//orgin point
		DeleteOneColOfMat(mc, mind);
		m_mCentroidGrid.at<Vec2d>(0, 0) = cco;


		mind = findNearestPointInMatrix(x1, mc);
		Mat ccx = mc.col(mind);
		DeleteOneColOfMat(mc, mind);
		m_mCentroidGrid.at<Vec2d>(0, 1) = ccx;


		mind = findNearestPointInMatrix(y1, mc);
		Mat ccy = mc.col(mind);
		DeleteOneColOfMat(mc, mind);
		m_mCentroidGrid.at<Vec2d>(1, 0) = ccy;

		//find the first line of the centroids



		std::cout << mc.cols << std::endl;
		std::cout << m_mCentroidGrid.at<Vec2d>(0, 0) << " " <<  m_mCentroidGrid.at<Vec2d>(0, 1) << " " <<  m_mCentroidGrid.at<Vec2d>(1, 0) << std::endl;
		return m_mCentroidGrid;
	}

}