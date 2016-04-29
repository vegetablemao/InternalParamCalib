#include "calibrate.h"
#include <math.h>
#include <iostream>

namespace cv
{
	calibrate::calibrate()
	{
		m_sysCfg.blobgapx = 0;
		m_sysCfg.blobgapy = 0;
		m_sysCfg.blobradius = 0;
		m_sysCfg.cata = 0;
		m_sysCfg.focal = 0;
		m_sysCfg.model.clear();
		m_sysCfg.projtype.clear();
		m_sysCfg.viewfield = 0;
		m_sysCfg.cp.m_u = 0;
		m_sysCfg.cp.m_v = 0;
		m_sysCfg.cp.u0 = 0;
		m_sysCfg.cp.v0 = 0;
	}

	calibrate::calibrate(const std::string& configPath)
	{
		m_sysCfg.blobgapx = 0;
		m_sysCfg.blobgapy = 0;
		m_sysCfg.blobradius = 0;
		m_sysCfg.cata = 0;
		m_sysCfg.focal = 0;
		m_sysCfg.model.clear();
		m_sysCfg.projtype.clear();
		m_sysCfg.viewfield = 0;
		m_sysCfg.cp.m_u = 0;
		m_sysCfg.cp.m_v = 0;
		m_sysCfg.cp.u0 = 0;
		m_sysCfg.cp.v0 = 0;


		FileStorage fs(configPath, FileStorage::READ);
		fs["blobgapx"] >> m_sysCfg.blobgapx;
		fs["blobgapy"] >> m_sysCfg.blobgapy;
		fs["blobradius"] >> m_sysCfg.blobradius;
		fs["cata"] >> m_sysCfg.cata;
		fs["focal"] >> m_sysCfg.focal;
		fs["model"] >> m_sysCfg.model;
		fs["projtype"] >> m_sysCfg.projtype;
		fs["viewfield"] >> m_sysCfg.viewfield;
		fs["mu"] >> m_sysCfg.cp.m_u;
		fs["mv"] >> m_sysCfg.cp.m_v;
		fs["u0"] >> m_sysCfg.cp.u0;
		fs["v0"] >> m_sysCfg.cp.v0;
		fs.release();
	}

	calibrate::~calibrate()
	{

	}

	void calibrate::readConfigFile(const std::string& configPath)
	{
		FileStorage fs(configPath, FileStorage::READ);
		fs["blobgapx"] >> m_sysCfg.blobgapx;
		fs["blobgapy"] >> m_sysCfg.blobgapy;
		fs["blobradius"] >> m_sysCfg.blobradius;
		fs["cata"] >> m_sysCfg.cata;
		fs["focal"] >> m_sysCfg.focal;
		fs["model"] >> m_sysCfg.model;
		fs["projtype"] >> m_sysCfg.projtype;
		fs["viewfield"] >> m_sysCfg.viewfield;
		fs["mu"] >> m_sysCfg.cp.m_u;
		fs["mv"] >> m_sysCfg.cp.m_v;
		fs["u0"] >> m_sysCfg.cp.u0;
		fs["v0"] >> m_sysCfg.cp.v0;
		fs.release();
	}

	void calibrate::readCentroidsGrid(const std::string& pathName)
	{
		std::vector<std::string> FileNames;
		FileNames.clear();
		FileStorage fs(".\\fileNames.xml", FileStorage::READ);
		fs["fileNames"] >> FileNames;
		fs.release();

		std::string path;
		Mat centroidsGrid;
		for (size_t i = 0; i < FileNames.size(); i++)
		{
			path.clear();
			centroidsGrid.release();
			path.assign(pathName).append("\\").append(FileNames[i]).append("\\").append(FileNames[i]).append("_centroidsGrid.xml");
			fs.open(path, FileStorage::READ);
			fs["centroidsGridMat"] >> centroidsGrid;
			m_vCentGrids.push_back(centroidsGrid);
		}

		planecoords();
		initialiseInternalp();
	}

	void calibrate::planecoords()
	{
		size_t N = m_vCentGrids.size();
		for (int i = 0; i < N; i++)
		{
			size_t ny = m_vCentGrids[i].rows;
			size_t nx = m_vCentGrids[i].cols;
			Mat coordmat(ny, nx, CV_64FC2, Scalar::all(0));
			for (int r = 0; r < ny; r++)
			{
				for (int c = 0; c < nx; c++)
				{
					coordmat.at<Vec2d>(r, c) = Vec2d(m_sysCfg.blobgapx*c, m_sysCfg.blobgapy*r);
				}
			}
			m_vCoordMats.push_back(coordmat);
			coordmat.release();
		}
	}

	void calibrate::initialiseInternalp()
	{
		DoubleContainer theta, cy, coeff;
		int N = static_cast<int>( (m_sysCfg.viewfield/2) / 0.1 );
		double thetamax = m_sysCfg.viewfield / 2 * CV_PI / 180;
		double interval = 0.1 / 180 * CV_PI;
				
		theta.resize(N, 0);
		cy.resize(N, 0);
		for (int i = 0; i < N; i++)
		{
			theta[i] = i * interval;

			if (!m_sysCfg.projtype.compare("equidistance"))
			{
				cy[i] = theta[i] * m_sysCfg.focal;
			} 
			else if (!m_sysCfg.projtype.compare("perspective"))
			{
				cy[i] = std::tan(theta[i]) * m_sysCfg.focal;
			}
			else if (!m_sysCfg.projtype.compare("stereographic"))
			{
				cy[i] = 2 * std::tan(theta[i]/2) * m_sysCfg.focal;
			}
			else if (!m_sysCfg.projtype.compare("equisolidangle"))
			{
				cy[i] = 2 * std::sin(theta[i]/2) * m_sysCfg.focal;
			}
			else if (!m_sysCfg.projtype.compare("orthographic"))
			{
				cy[i] = std::sin(theta[i]) * m_sysCfg.focal;
			}
			else
			{
				std::cerr << "Unknow projection type" << std::endl;
			}
		}

		polyFitOddLsq(theta, cy, 3, coeff);

		double rmax = coeff[0]*thetamax + coeff[1]*std::pow(thetamax,3);

		m_vCoeffInit = coeff;
		m_dThetamax = thetamax;
	}

	void calibrate::polyFitOddLsq(const DoubleContainer& x, const DoubleContainer& y, int n, DoubleContainer& coeff)
	{
		coeff.clear();
		size_t M = x.size();
		std::vector<int> pows;
		pows.clear();
		for (int i = 1; i <= n; i+=2)
		{
			pows.push_back(i);
		}
		size_t nodd = pows.size();
		
		Mat X;
		Mat mx = Mat(x);
		Mat my = Mat(y);
		for (int i = 0; i < nodd; i++)
		{
			Mat tx = mx.clone();
			int p = pows[i];
			pow(tx, p, tx);
			tx = tx.t();
			X.push_back(tx);
		}
		X = X.t();

		Mat mc = (X.t() * X).inv() * X.t() * my;//最小二乘法
		coeff = mc.col(0);
	}

	void calibrate::findEllipse()
	{

	}

}