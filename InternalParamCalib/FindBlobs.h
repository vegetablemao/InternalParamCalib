#pragma once

#ifndef FINDBLOBS_H
#define FINDBLOBS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "FindCalibBoardROI.h"

namespace cv
{
	typedef std::vector<vector<Point> > contourContainer;
	class findBlobs : public findCalibROI
	{
	public:
		findBlobs();
		virtual ~findBlobs();
		contourContainer& findBlobsContours(const Mat& blobImg);
		virtual int init(const std::string& imgFileName);
	private:
		contourContainer m_vContours;
	};
}

#endif