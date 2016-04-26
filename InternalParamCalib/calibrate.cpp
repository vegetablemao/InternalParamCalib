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

	calibrate::~calibrate()
	{

	}

	void calibrate::readCentroidsGrid(const FileStorage& centGridFS)
	{
		Mat centroidsGrid;
		centGridFS["centroidsGridMat"] >> centroidsGrid;
		m_vCentGrids.push_back(centroidsGrid);

	}

	void calibrate::planecoords()
	{


	}


}