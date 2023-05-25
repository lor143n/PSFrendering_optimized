#include <iostream>
#include <array>
#include <vector>

#define ORDER 18


class DepthDatabase {

private:

	float depth;

	std::vector<PSF> psfs;

public:

	DepthDatabase(float dep) {
		this->depth = dep;
	}


	void insertPsf(PSF new_psf) {


		this->psfs.push_back(new_psf);

	}
};

class PSF {


private:
	float centre_of_mass_x;
	float centre_of_mass_y;

	float kernel[12][12];

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