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
	typedef std::vector<vector<Point> > contourContainer;
	typedef std::vector<Point> centroidContainer;
	class findBlobs : public findCalibROI
	{
	public:
		findBlobs();
		virtual ~findBlobs();
		virtual int init(const std::string& imgFileName);
		const contourContainer& findBlobsContours(const Mat& blobImg);
		const centroidContainer& findCentroids(const contourContainer& contours);
	private:
		contourContainer m_vContours;
		centroidContainer m_vCentroids;
		Mat m_mOutputImg;
	};
}

#endif