#pragma once

#ifndef CALIBRATE_H
#define CALIBRATE_H

#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


namespace cv
{
	typedef std::vector<Mat> MatContainer;
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
		calibrate(const std::string& configPath);
		virtual ~calibrate();
		void readConfigFile(const std::string& configPath);
		void readCentroidsGrid(const std::string& pathName);

	private:
		void planecoords();
		void initialiseInternalp();
	private:
		sysConfig m_sysCfg;
		MatContainer m_vCoordMats;
		MatContainer m_vCentGrids;
	};
}


#endif