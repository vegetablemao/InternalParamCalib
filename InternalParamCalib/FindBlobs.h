#pragma once

#ifndef FINDBLOBS_H
#define FINDBLOBS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class findBlobs
{
public:
	findBlobs();
	virtual ~findBlobs();

	int init();
private:
};


#endif