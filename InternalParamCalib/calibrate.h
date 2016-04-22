#pragma once

#ifndef CALIBRATE_H
#define CALIBRATE_H

#include <string>

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
	private:

	};
}


#endif