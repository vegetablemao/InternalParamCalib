#pragma once

#ifndef CALIBRATE_H
#define CALIBRATE_H

#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


namespace cv
{
	class calibrate
	{
	public:

		typedef struct _pixelp
		{
			float m_u;
			float m_v;
			float u0;
			float v0;
		}pixelp, *ppixelp;

		typedef struct _sysConfig
		{
			float blobgapx;
			float blobgapy;
			bool cata;
			float blobradius;
			std::string projtype;
			std::string model;
			float focal;
			float viewfield;
			pixelp cp;
		}sysConfig, *psysConfig;

		calibrate();
		virtual ~calibrate();
		void readCentroidsGrid(const FileStorage& centGridFS);

	private:
		void planecoords();

	private:
		sysConfig m_sysCfg;
		Mat m_mCoordMat;
	};
}


#endif