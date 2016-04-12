#pragma once

#ifndef FINDCALIBBOARDROI
#define FINDCALIBBOARDROI

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

class findCalibROI
{
public:
	findCalibROI();
	virtual ~findCalibROI();
	int init(char * imgFileName);
	void destroyAll();

private:
	void onMouse( int event, int x, int y, int, void* );

private:
	cv::Mat inputImg, ScribbleImg, outputImg;	//images
	cv::Mat ScribbleMask;	//mask

	bool rButtonDown;
	bool lButtonDown;

	cv::Point prePt;
	cv::Point firstPt;

	std::vector<cv::Point> pointList;
	std::vector<cv::Point> hull;
	std::vector<cv::Point> cartcoord;
	int cartCnt;

	std::string m_sImgFileName;
};

#endif