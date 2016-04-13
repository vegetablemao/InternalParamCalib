#pragma once

#ifndef FINDCALIBBOARDROI_H
#define FINDCALIBBOARDROI_H

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <opencv2/imgproc/imgproc.hpp> 

/************************************************************************/
/* this class is used to choose the ROI of the input image with mouse.  */
/************************************************************************/
class findCalibROI
{
public:
	findCalibROI();
	virtual ~findCalibROI();
	int init(const std::string& imgFileName);
	void destroyAll();
	void saveROIImage();
	void reset();
	void nextImage();
	const cv::Mat& findBlobs();
	const std::vector<cv::Point>& getCartCoord() const
	{
		return m_vCartCoord;
	}
	const std::string& getFileName() const
	{
		return m_sImgFileName;
	}

private:
	static void onMouse( int event, int x, int y, int, void* p);

private:
	cv::Mat m_mInputImg, m_mScribbleImg, m_mOutputImg;	//images
	cv::Mat m_mScribbleMask;	//mask
	cv::Mat m_mBlobImg;			//blobs
	int m_iscribbleRadius;

	bool m_bRButtonDown;
	bool m_bLButtonDown;

	cv::Point m_pPrePt;
	cv::Point m_pFirstPt;

	std::vector<cv::Point> m_vPointList;
	std::vector<cv::Point> m_vHullPointList;
	std::vector<cv::Point> m_vCartCoord;//record the orgin,x,y point coordinate
	int m_iCartCoordCnt;

	std::string m_sImgFileName;
};

#endif