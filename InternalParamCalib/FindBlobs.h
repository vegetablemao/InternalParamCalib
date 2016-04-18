#pragma once

#ifndef FINDBLOBS_H
#define FINDBLOBS_H

#include <iostream>
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
		static bool greaterMark(const contourType& s1, const contourType& s2) { return s1.size() > s2.size() ;}
		static void printFunc(int i) { std::cout << ' ' << i; }
		void DeleteOneColOfMat(Mat& object,int col);
		int findNearestPointInMatrix(const Point& ip, const Mat& mc);
		void computeCos(const Mat& co, const Mat& cco, const Mat& cxy, const std::vector<Point>& pairs, std::vector<double>& cosa, std::vector<double>& cosb);
		void sortTheDistFromThePoint(const Point& ip, const Mat& mc, std::vector<int>& idx);
	private:
		contourContainer m_vContours;
		centroidContainer m_vCentroids;
		Mat m_mCentroidGrid;
		Mat m_mOutputImg;
	};
}

#endif