#pragma once

#ifndef FINDBLOBS_H
#define FINDBLOBS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "FindCalibBoardROI.h"

namespace cv
{
	typedef std::vector<Point> contourType;
	typedef std::vector<contourType> contourContainer;
	typedef std::vector<Point2d> centroidContainer;
	
	class findBlobs : public findCalibROI
	{
	public:
		findBlobs();
		virtual ~findBlobs();
		virtual int init(const std::string& imgFileName);
		const contourContainer& findBlobsContours(const Mat& blobImg);
		const centroidContainer& findCentroids(/*const contourContainer& contours*/);
		const Mat& findCentroidGrid();
		
	private:
		static bool greaterMark(const contourType& s1, const contourType& s2) { return s1.size() > s2.size() ;};
		void DeleteOneColOfMat(Mat& object,int col);
		int findNearestPointInMatrix(const Point& ip, const Mat& mc);
	private:
		contourContainer m_vContours;
		centroidContainer m_vCentroids;
		Mat m_mCentroidGrid;
		Mat m_mOutputImg;
	};
}

#endif