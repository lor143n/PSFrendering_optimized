#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <string.h>
#include <boost/filesystem.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>


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

	std::string m_depth;
	std::vector<PSF> m_psfs;

public:

	DepthDatabase(std::string dep) {
		m_depth = dep;
	}

	std::string getDepth() {
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

void loadPSFs(std::string& camera_path, std::vector<DepthDatabase>& psfsDict);

void makePSFsDictionary(path& p, std::vector<DepthDatabase>& depths);


//PSF convolution functions

void psfConvolution(cv::Mat& src_image);


//Auxiliary functions

std::vector<std::string> splitString(std::string str);