#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <opencv2/core.hpp>
#include <string.h>
#include <fstream>

#define ORDER 18


class PSF {


private:
	float centre_of_mass_x;
	float centre_of_mass_y;

	float kernel[ORDER][ORDER];

public:

	PSF(float com_x, float com_y) {
		this->centre_of_mass_x = com_x;
		this->centre_of_mass_y = com_y;

		for (short i = 0; i < ORDER; i++) {
			for (short j = 0; j < ORDER; j++) {
				kernel[i][j] = 0;
			}
		}
	}

};

class DepthDatabase {

private:

	float m_depth;

	std::vector<PSF> m_psfs;

public:

	DepthDatabase(float dep) {
		m_depth = dep;
	}


	void insertPsf(PSF new_psf) {

		m_psfs.push_back(new_psf);
	}
};

std::vector<DepthDatabase> loadPSFs(std::string camera_path);