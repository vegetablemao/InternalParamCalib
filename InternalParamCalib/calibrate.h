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
	typedef std::vector<double> DoubleContainer;
	class calibrate
	{
	public:

		typedef struct _pixelp
		{
			double m_u;
			double m_v;
			double u0;
			double v0;
		}pixelp, *ppixelp;

		typedef struct _sysConfig
		{
			double blobgapx;
			double blobgapy;
			bool cata;
			double blobradius;
			std::string projtype;
			std::string model;
			double focal;
			double viewfield;
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
		void polyFitOddLsq(const DoubleContainer& x, const DoubleContainer& y, int n, DoubleContainer& coeff);
		void findEllipse();
	private:
		sysConfig m_sysCfg;
		MatContainer m_vCoordMats;
		MatContainer m_vCentGrids;
		
		DoubleContainer m_vCoeffInit;
		double m_dThetamax;
	};
}


#endif