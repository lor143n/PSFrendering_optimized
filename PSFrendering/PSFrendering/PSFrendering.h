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

	double m_centre_of_mass_x;
	double m_centre_of_mass_y;

	cv::Mat m_kernel;

public:

	PSF(float com_x, float com_y, cv::Mat& kernel) {
		m_centre_of_mass_x = com_x;
		m_centre_of_mass_y = com_y;

		m_kernel = kernel;
	}

};

class DepthDatabase {

private:

	float m_depth;

public:

	std::vector<PSF> m_psfs;

	DepthDatabase(float dep) {
		m_depth = dep;
	}

	float getDepth() {
		return m_depth;
	}


	void insertPSF(PSF new_psf) {

		m_psfs.push_back(new_psf);
	}
};

void loadPSFs(std::string& camera_path, std::vector<DepthDatabase>& psfsDict);

void insertAllDepthFolders(path& p, std::vector<DepthDatabase>& depths);

std::vector<std::string> splitString(std::string str);