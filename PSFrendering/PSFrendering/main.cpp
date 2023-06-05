#include "PSFrendering.h"


int main() {


	std::string cameraPath = "F:\\lenses\\brendel-tessar_21_3.0_2.8";
	std::string imagePath = "E:\\GitHub\\test\\checker\\checker1024_100_2m.exr";

	std::cout << "importing camera from: " << cameraPath << std::endl;

	std::vector<DepthDatabase> kernels;
	loadPSFs(cameraPath, kernels);

	std::cout << "Done." << std::endl;
	std::cout << "importing image from: " << imagePath << std::endl;

	//import and iteration of exr file with depth value

	std::array<cv::Mat, 2> src_image;
	loadEXR(imagePath, src_image);

	std::cout << "Done." << std::endl;

	//kernels[0].testPrint();
	//image padding (how to do with OpenCV?)

	std::cout << "PSF convolution..." << std::endl;

	//psfConvolution(src_image[0], src_image[1]);

	cv::imwrite("E:\\GitHub\\PSFrendering_optimized\\PSFrendering\\out\\res.jpg", src_image);
}