#include "FindBlobs.h"
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
				object.col(i).copyTo(object.col(i-1)); 	
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
		std::vector<double>::iterator minIter = std::min_element(dist.begin(), dist.end());
		int mind = std::distance(dist.begin(), minIter);

		return mind;

	}

	void findBlobs::sortTheDistFromThePoint(const Point& ip, const Mat& mc, vector<int>& idx)
	{
		idx.clear();
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
			idx.push_back(i);
		}
		sort(idx.begin(), idx.end(),
			[& dist](size_t i1, size_t i2) {return dist[i1] <  dist[i2];});

	}

	void findBlobs::computeCos(const Mat& co, const Mat& cco, const Mat& cxy, 
		const std::vector<Point>& pairs,
		std::vector<double>& cosa, std::vector<double>& cosb)
	{
		Mat vxy = cxy - co * Mat::ones(1, cxy.cols, co.type());
		Mat v =  co - cco;

		
		std::cout << vxy << std::endl;
		std::cout << v << std::endl;
		std::cout << co << std::endl;
		std::cout << cco << std::endl;
		std::cout << cxy << std::endl;

		vector<double> vxyabs;
		vxyabs.clear();
		for (int i = 0; i < cxy.cols; i++)
		{
			double tt = norm(vxy.col(i));
			vxyabs.push_back(norm(vxy.col(i)));
		}
		cosa.clear();
		cosb.clear();
		for (int i = 0; i < cxy.cols; i++)
		{
			int k1 = pairs[i].x;
			int k2 = pairs[i].y;
			Mat tmpa = vxy.col(k1).t() * vxy.col(k2);
			double tcos = tmpa.at<double>(0,0) / ( vxyabs[k1] * vxyabs[k2] );
			cosa.push_back(tcos);

			Mat tmpb = v.t() * vxy.col(i);
			tcos =  tmpb.at<double>(0, 0) / ( vxyabs[i] * norm(v.col(0)) );
			cosb.push_back(tcos);
		}
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
		std::cout << mc << std::endl;
		Mat cco(2, 1, mc.type());
		mc.col(mind).copyTo(cco);	//orgin point

		std::cout << cco << std::endl;
		DeleteOneColOfMat(mc, mind);
		std::cout << mc << std::endl;
		std::cout << cco << std::endl;
		m_mCentroidGrid.at<Vec2d>(0, 0) = cco;


		mind = findNearestPointInMatrix(x1, mc);
		std::cout << mc << std::endl;
		Mat ccx(2, 1, mc.type());
		mc.col(mind).copyTo(ccx);
		std::cout << ccx << std::endl;
		DeleteOneColOfMat(mc, mind);
		std::cout << mc << std::endl;
		m_mCentroidGrid.at<Vec2d>(0, 1) = ccx;


		mind = findNearestPointInMatrix(y1, mc);
		Mat ccy(2, 1, mc.type());
		mc.col(mind).copyTo(ccy);
		DeleteOneColOfMat(mc, mind);
		m_mCentroidGrid.at<Vec2d>(1, 0) = ccy;

		//find the first line of the centroids
		std::vector<int> idx;
		sortTheDistFromThePoint(x1, mc, idx);

		const int tmpCountMax = 6;
		int tmpidx[tmpCountMax] = {3, 4, 5, 6, 7, 8};
		int tmpCount = 0;

		Mat cxy;
		cxy.create(2, 3, mc.type());
		cxy = 0;
		for(size_t i = 0; i < 3; i++)
		{
			mc.col(idx[i]).copyTo(cxy.col(i));
			//cxy.col(i) = mc.col(idx[i]);
		}
		std::cout << mc << std::endl;
		std::cout << cxy << std::endl;


		std::vector<Point> pairs;
		pairs.clear();
		pairs.push_back(Point(1, 2));
		pairs.push_back(Point(0, 2));
		pairs.push_back(Point(0, 1));

		std::vector<double> cosa;
		std::vector<double> cosb;
		computeCos(ccx, cco, cxy, pairs, cosa, cosb);





		std::for_each (idx.begin(), idx.end(), printFunc);
		std::cout << std::endl;
		std::cout << mc.cols << std::endl;
		std::cout << m_mCentroidGrid.at<Vec2d>(0, 0) << " " <<  m_mCentroidGrid.at<Vec2d>(0, 1) << " " <<  m_mCentroidGrid.at<Vec2d>(1, 0) << std::endl;
		return m_mCentroidGrid;
	}

}