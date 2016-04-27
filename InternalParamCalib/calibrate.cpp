#include "calibrate.h"

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
		for (int i = 0; i < FileNames.size(); i++)
		{
			path.clear();
			centroidsGrid.release();
			path.assign(pathName).append("\\").append(FileNames[i]).append("\\").append(FileNames[i]).append("_centroidsGrid.xml");
			fs.open(path, FileStorage::READ);
			fs["centroidsGridMat"] >> centroidsGrid;
			m_vCentGrids.push_back(centroidsGrid);
		}

		planecoords();
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
}