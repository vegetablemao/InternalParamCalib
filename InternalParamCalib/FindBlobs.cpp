#include "FindBlobs.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <math.h>

namespace cv
{
	findBlobs::findBlobs()
	{

	}

	findBlobs::~findBlobs() 
	{
		destroyWindow("labled centroids");
	}

	/*int findBlobs::init(const std::string& imgFileName)
	{
		m_vContours.clear();
		return findCalibROI::init(imgFileName);

	}*/

	const contourContainer& findBlobs::findBlobsContours(const FileStorage& blobImgFile)
	{
		m_vContours.clear();
		Mat blobImg;
		blobImgFile["blobsMat"] >> blobImg;
		findContours(blobImg, m_vContours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		if (m_vContours.empty())
		{
			std::cout << "couldn't find any contours!" << std::endl;
			return m_vContours;
		}
		blobImgFile["subDirectory"] >> m_sSubDirectory;
		blobImgFile["imageFileName"] >> m_sImageFileName;
		blobImgFile["cartMat"] >> m_vCartCoord;
		blobImgFile["blobX"] >> m_uBlobX;
		blobImgFile["blobY"] >> m_uBlobY;

		m_mOutputImg = Mat::zeros(blobImg.rows, blobImg.cols, CV_8UC3);
		int numOfBlobs = m_uBlobX * m_uBlobY;
		std::sort(m_vContours.begin(), m_vContours.end(), greaterMark); //sort the contours from big to small, we only need the biggest numOfBlobs contours.
		m_vContours.erase(m_vContours.begin()+numOfBlobs, m_vContours.end()); //erase the smallest contours

		drawContours(m_mOutputImg, m_vContours, -1, CV_RGB(255,255,255));
		std::string contourFileName = m_sSubDirectory + "\\" + m_sImageFileName + "_contours.jpg";
		imwrite(contourFileName, m_mOutputImg);
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
		
		std::string centImgName = m_sSubDirectory + "\\" + m_sImageFileName + "_centroids.jpg";
		imwrite(centImgName, m_mOutputImg);

		std::string centXMLFileName = m_sSubDirectory + "\\" + m_sImageFileName + "_centroids.xml";
		cv::FileStorage fs(centXMLFileName, cv::FileStorage::WRITE);
		fs << "centroidsMat" << m_vCentroids;
		fs.release();
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

	void findBlobs::sortTheDistFromThePoint(const Mat& ip, const Mat& mc, vector<int>& idx)
	{
		idx.clear();
		int numOfCol = mc.cols;

		Mat ipMat = ip * Mat::ones(1,numOfCol, ip.type());
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

	void findBlobs::sortTheDistFromThreePoint(const Mat& ip, const Mat& mc, vector<int>& idx)
	{
		idx.clear();
		int numOfCol = mc.cols;

		Mat ip1 = ip.col(0);
		Mat ip1Mat = ip1 * Mat::ones(1,numOfCol, ip.type());
		subtract(ip1Mat, mc, ip1Mat);
		pow(ip1Mat, 2, ip1Mat);

		Mat ip2 = ip.col(1);
		Mat ip2Mat = ip2 * Mat::ones(1,numOfCol, ip.type());
		subtract(ip2Mat, mc, ip2Mat);
		pow(ip2Mat, 2, ip2Mat);

		Mat ip3 = ip.col(2);
		Mat ip3Mat = ip3 * Mat::ones(1,numOfCol, ip.type());
		subtract(ip3Mat, mc, ip3Mat);
		pow(ip3Mat, 2, ip3Mat);

		std::vector<double> dist;
		dist.clear();
		for (int i = 0; i < numOfCol; i++)
		{
			Scalar s = sum(ip1Mat.col(i)) + sum(ip2Mat.col(i)) + sum(ip3Mat.col(i));
			dist.push_back(s[0]);
			idx.push_back(i);
		}
		sort(idx.begin(), idx.end(),
			[& dist](size_t i1, size_t i2) {return dist[i1] <  dist[i2];});
	}

	void findBlobs::computeCos(const Mat& co, const Mat& v, const Mat& cco, const Mat& cxy, const Mat& vxy,
		const std::vector<Point>& pairs,
		std::vector<double>& cosa, std::vector<double>& cosb)
	{

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
		m_mCentroidGrid.create(m_uBlobX, m_uBlobY, CV_64FC2);
		m_mCentroidGrid = 0;

		Point origin = m_vCartCoord[0];
		Point x1 = m_vCartCoord[1];
		Point y1 = m_vCartCoord[2];
		Mat mc = Mat(m_vCentroids).reshape(1).t();

		//find the left top three points in the centroids.
		int mind = findNearestPointInMatrix(origin, mc);
		Mat cco(2, 1, mc.type());
		mc.col(mind).copyTo(cco);	//orgin point
		DeleteOneColOfMat(mc, mind);
		m_mCentroidGrid.at<Vec2d>(0, 0) = cco;

		mind = findNearestPointInMatrix(x1, mc);
		Mat ccx(2, 1, mc.type());
		mc.col(mind).copyTo(ccx);
		DeleteOneColOfMat(mc, mind);
		m_mCentroidGrid.at<Vec2d>(0, 1) = ccx;

		mind = findNearestPointInMatrix(y1, mc);
		Mat ccy(2, 1, mc.type());
		mc.col(mind).copyTo(ccy);
		DeleteOneColOfMat(mc, mind);
		m_mCentroidGrid.at<Vec2d>(1, 0) = ccy;

		
		std::vector<Point> pairs;
		pairs.clear();
		pairs.push_back(Point(1, 2));
		pairs.push_back(Point(0, 2));
		pairs.push_back(Point(0, 1));
		Mat co = ccx;
		Mat v =  co - cco;
		int j = 0;


		//find the first line of the centroids
		for (int i = 1; i < m_uBlobX-1; i++)
		{
			std::vector<int> idx;
			sortTheDistFromThePoint(co, mc, idx);

			const int len = 6;
			int tmpidx[len] = {2, 3, 4, 5, 6, 7};
			int tmpCount = 0;
			int tmpCountmax = len - 1;

			bool ok = false;
			Mat cxy;
			Mat vxy;
			std::vector<int> indxy;
			std::vector<double> cosa;
			std::vector<double> cosb;
			Point pair;
			int mincosind;
			while (!ok)
			{			
				cxy.create(2, 3, mc.type());
				cxy = 0;
				for(size_t i = 0; i < 2; i++)
				{
					mc.col(idx[i]).copyTo(cxy.col(i));
				}
				mc.col(idx[tmpidx[tmpCount]]).copyTo(cxy.col(2));
				vxy = cxy - co * Mat::ones(1, cxy.cols, co.type());
				
				computeCos(co, v, cco, cxy, vxy, pairs, cosa, cosb);

				std::vector<double>::iterator minIter = std::min_element(cosa.begin(), cosa.end());
				mincosind = std::distance(cosa.begin(), minIter);
				
				
				indxy.clear();
				if (2 == mincosind)
				{
					pair = pairs[mincosind];
					indxy.push_back(idx[pair.x]);
					indxy.push_back(idx[pair.y]);
				}
				else
				{
					pair = pairs[mincosind];
					indxy.push_back(idx[pair.x]);
					indxy.push_back(idx[tmpidx[tmpCount]]);
				}
				ok = true;

				std::vector<double>::iterator maxIter = std::max_element(cosb.begin(), cosb.end());

				if ( acos(*maxIter)*180/CV_PI > 25 )
				{
					if (tmpCount < len)
					{
						tmpCount += 1;
						ok = false;
					}
					else
						std::cout << "In blobsgrid: Can not find (0," << i+1 << ")" << std::endl;  
				}
			}
			Mat cxyo = cxy.clone();
			cxyo.col(pair.x).copyTo(cxy.col(0));
			cxyo.col(pair.y).copyTo(cxy.col(1));
			cxy = cxy.t();
			cxy.pop_back();
			cxy = cxy.t();

			Mat vxyo = vxy.clone();
			vxyo.col(pair.x).copyTo(vxy.col(0));
			vxyo.col(pair.y).copyTo(vxy.col(1));
			vxy = vxy.t();
			vxy.pop_back();
			vxy = vxy.t();
			Mat ll = Mat::zeros(1, 2, cxy.type());
			vxy.push_back(ll);

			Mat vtmp = vxy.col(0).cross(vxy.col(1));
			int trueminind = pair.x;
			if (vtmp.at<double>(2,0) < 0)
			{
				flip(vxy, vxy, 1);
				flip(cxy, cxy, 1);
				flip(indxy, indxy, 1);
				trueminind = pair.y;
			}

			std::vector<double>::iterator maxIter = std::max_element(cosa.begin(), cosa.end());
			int maxind = std::distance(cosa.begin(), maxIter);
			if ( (*maxIter > cos(5.0/180.0*CV_PI)) && (maxind!=trueminind ))
			{
				Mat m1 = cxy.col(0) - cxyo.col(mincosind);
				Mat m2 = m1.t() * v;
				if (m2.at<double>(0,0) > 0)
				{
					cxyo.col(mincosind).copyTo(cxy.col(0));
					if (2 == mincosind)
					{
						indxy[0] = idx[tmpidx[tmpCount]];
					}
					else
					{
						indxy[0] = idx[mincosind];
					}
				}
			}
			m_mCentroidGrid.at<Vec2d>(j, i+1) = cxy.col(0);
			v = cxy.col(0) - co;
			co = cxy.col(0);
			DeleteOneColOfMat(mc, indxy[0]);
		}

		//find the first col of the centroids
		int i = 0;
		co = ccy;
		v = co - cco;
		for (int j = 1; j < m_uBlobY-1; j++)
		{
			std::vector<int> idx;
			sortTheDistFromThePoint(co, mc, idx);

			const int len = 6;
			int tmpidx[len] = {2, 3, 4, 5, 6, 7};
			int tmpCount = 0;
			int tmpCountmax = len - 1;

			bool ok = false;
			Mat cxy;
			Mat vxy;
			std::vector<int> indxy;
			std::vector<double> cosa;
			std::vector<double> cosb;
			Point pair;
			int mincosind;
			while (!ok)
			{			
				cxy.create(2, 3, mc.type());
				cxy = 0;
				for(size_t i = 0; i < 2; i++)
				{
					mc.col(idx[i]).copyTo(cxy.col(i));
				}
				mc.col(idx[tmpidx[tmpCount]]).copyTo(cxy.col(2));
				vxy = cxy - co * Mat::ones(1, cxy.cols, co.type());

				computeCos(co, v, cco, cxy, vxy, pairs, cosa, cosb);

				std::vector<double>::iterator minIter = std::min_element(cosa.begin(), cosa.end());
				mincosind = std::distance(cosa.begin(), minIter);


				indxy.clear();
				if (2 == mincosind)
				{
					pair = pairs[mincosind];
					indxy.push_back(idx[pair.x]);
					indxy.push_back(idx[pair.y]);
				}
				else
				{
					pair = pairs[mincosind];
					indxy.push_back(idx[pair.x]);
					indxy.push_back(idx[tmpidx[tmpCount]]);
				}
				ok = true;

				std::vector<double>::iterator maxIter = std::max_element(cosb.begin(), cosb.end());

				if ( *maxIter < cos(30.0/180.0*CV_PI) )
				{
					if (tmpCount < len)
					{
						tmpCount += 1;
						ok = false;
					}
					else
						std::cout << "In blobsgrid: Can not find (" << j+1 << ",0)" << std::endl;  
				}
			}
			Mat cxyo = cxy.clone();
			cxyo.col(pair.x).copyTo(cxy.col(0));
			cxyo.col(pair.y).copyTo(cxy.col(1));
			cxy = cxy.t();
			cxy.pop_back();
			cxy = cxy.t();

			Mat vxyo = vxy.clone();
			vxyo.col(pair.x).copyTo(vxy.col(0));
			vxyo.col(pair.y).copyTo(vxy.col(1));
			vxy = vxy.t();
			vxy.pop_back();
			vxy = vxy.t();
			Mat ll = Mat::zeros(1, 2, cxy.type());
			vxy.push_back(ll);

			Mat vtmp = vxy.col(0).cross(vxy.col(1));
			int trueminind = pair.x;
			if (vtmp.at<double>(2,0) > 0)
			{
				flip(vxy, vxy, 1);
				flip(cxy, cxy, 1);
				flip(indxy, indxy, 1);
				trueminind = pair.y;
			}

			std::vector<double>::iterator maxIter = std::max_element(cosa.begin(), cosa.end());
			int maxind = std::distance(cosa.begin(), maxIter);
			if ( (*maxIter > cos(5.0/180.0*CV_PI)) && (maxind!=trueminind ))
			{
				Mat m1 = cxy.col(0) - cxyo.col(mincosind);
				Mat m2 = m1.t() * v;
				if (m2.at<double>(0,0) > 0)
				{
					cxyo.col(mincosind).copyTo(cxy.col(0));
					if (2 == mincosind)
					{
						indxy[0] = idx[tmpidx[tmpCount]];
					}
					else
					{
						indxy[0] = idx[mincosind];
					}
				}
			}
			m_mCentroidGrid.at<Vec2d>(j+1, i) = cxy.col(0);
			v = cxy.col(0) - co;
			co = cxy.col(0);
			DeleteOneColOfMat(mc, indxy[0]);
		}
		
		double costol = cos(30.0/180.0*CV_PI);
		for (int j = 1; j < m_uBlobY; j++)
		{
			for (int i = 1; i < m_uBlobX; i++)
			{
				if ((i==m_uBlobX-1) && (j==m_uBlobX-1))
				{
					m_mCentroidGrid.at<Vec2d>(j, i) = mc.col(0);
				}
				else
				{
					Mat leftTopMat;
					std::vector<int> sind;
					int size[2] = {3, 2};
					Vec2d v1 = m_mCentroidGrid.at<Vec2d>(j-1, i-1);
					Vec2d v2 = m_mCentroidGrid.at<Vec2d>(j-1, i);
					Vec2d v3 = m_mCentroidGrid.at<Vec2d>(j, i-1);
					leftTopMat.push_back(v1);
					leftTopMat.push_back(v2);
					leftTopMat.push_back(v3);
					leftTopMat = leftTopMat.reshape(1).t();
					sind.clear();
					sortTheDistFromThreePoint(leftTopMat, mc, sind);

					Mat cclose;
					if (sind.size() > 2)
					{
						cclose.create(2, 3, mc.type());
						cclose = 0;
						for(size_t i = 0; i < 3; i++)
						{
							mc.col(sind[i]).copyTo(cclose.col(i));
						}
					}
					else
					{
						cclose.create(2, 2, mc.type());
						cclose = 0;
						for(size_t i = 0; i < 2; i++)
						{
							mc.col(sind[i]).copyTo(cclose.col(i));
						}
					}
					Vec2d v = v2 - v1;
					Mat mv3(v3);
					Mat vs = mv3 * Mat::ones(1, cclose.cols, mc.type());
					subtract(cclose, vs, vs);
					std::vector<double> cosa;
					cosa.clear();
					for (int k = 0; k < cclose.cols; k++)
					{
						Mat tmp = Mat(v).t() * vs.col(k);
						double tcos = tmp.at<double>(0,0) / (norm(v)*norm(vs.col(k)));
						cosa.push_back(tcos);
					}
					std::vector<double>::iterator maxIter = std::max_element(cosa.begin(), cosa.end());
					int maxind = std::distance(cosa.begin(), maxIter);
					int minind = 0;
					if (cosa[0] > costol)
					{
						minind = sind[0];
					}
					else if (cosa[0] < costol && cosa[1] > costol)
					{
						minind = sind[1];
					}
					else
					{
						minind = sind[maxind];
					}
					m_mCentroidGrid.at<Vec2d>(j, i) = mc.col(minind);
					DeleteOneColOfMat(mc, minind);
				}
			}
		}

		std::string centXMLFileName = m_sSubDirectory + "\\" + m_sImageFileName + "_centroidsGrid.xml";
		cv::FileStorage fs(centXMLFileName, cv::FileStorage::WRITE);
		fs << "centroidsGridMat" << m_mCentroidGrid;
		fs.release();

		return m_mCentroidGrid;
	}

	void findBlobs::blobvis()
	{
		for (int k=0; k < m_uBlobX; k++)
		{
			for (int l=0; l < m_uBlobY; l++)
			{
				std::stringstream tex;
				tex << l;
				tex << ',';
				tex << k;
				Point pt;
				pt.x =  m_mCentroidGrid.at<Vec2d>(l, k).val[0] + 2;
				pt.y =  m_mCentroidGrid.at<Vec2d>(l, k).val[1] + 2;
				putText(m_mOutputImg, tex.str(), pt, FONT_HERSHEY_PLAIN, 0.8, CV_RGB(255,0,0));
				 
			}
		}
		namedWindow( "labled centroids", cv::WINDOW_AUTOSIZE);
		imshow("labled centroids", m_mOutputImg);
	}

}