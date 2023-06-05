#pragma once

#define  IMATH_HALF_NO_LOOKUP_TABLE

#include <iostream>
#include <math.h>
#include <array>
#include <vector>
#include <string.h>
#include <boost/filesystem.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfArray.h>
#include <ImathBox.h>
#include <half.h>
#include <ImathVec.h>

using namespace boost::filesystem;

#define ORDER 18
typedef cv::Point3_<uint8_t> Pixel;


class PSF {


private:

	float m_centre_of_mass_x;
	float m_centre_of_mass_y;

	cv::Mat m_kernel;

public:

	PSF(float com_x, float com_y, cv::Mat& kernel) {
		m_centre_of_mass_x = com_x;
		m_centre_of_mass_y = com_y;

		m_kernel = kernel;
	}

	cv::Mat getKernel() {

		return m_kernel;

	}

};

class DepthDatabase {

private:

	double m_depth;
	std::vector<PSF> m_psfs;

public:

	DepthDatabase(double dep) {
		m_depth = dep;
	}

	double getDepth() {
		return m_depth;
	}

	void insertPSF(PSF new_psf) {

		m_psfs.push_back(new_psf);
	}

	void testPrint() {

		std::cout << m_psfs[0].getKernel() << std::endl;
	}
};

//PSFs Databse construction

void loadPSFs(path camera_path, std::vector<DepthDatabase>& psfsDict);

//PSF convolution functions

void psfConvolution(cv::Mat& rgb_image, cv::Mat& depth_image, cv::Mat& out_image, std::vector<DepthDatabase> data, int krnl_size);

//Auxiliary functions

void loadEXR(std::string path, std::array<cv::Mat, 2>& image_result);

std::vector<std::string> splitString(std::string str);