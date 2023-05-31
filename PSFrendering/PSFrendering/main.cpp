#include "PSFrendering.h"


int main() {


	std::string cameraPath = "F:\\lenses\\brendel-tessar_21_3.0_2.8";
	std::string imagePath = "E:\\GitHub\\test\\rabbits_brendel-tessar.exr";

	std::cout << "importing camera from: " << cameraPath << std::endl;

	std::vector<DepthDatabase> kernels;
	loadPSFs(cameraPath, kernels);

	std::cout << "Done." << std::endl;

	std::cout << "importing image from: " << imagePath << std::endl;

	//import and iteration of exr file with depth value

	auto src_image = cv::imread(imagePath, cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);

	std::cout << "Done." << std::endl;

	//kernels[0].testPrint();

	//print loading time

	//measure time

	//image padding (how to do with OpenCV?)

	std::cout << "PSF convolution..." << std::endl;

	//PSF convolution (how to do manage depth and rgb?)
	psfConvolution(src_image);

	//print convolution time

	cv::imwrite("E:\\GitHub\\PSFrendering_optimized\\PSFrendering\\out\\res.png", src_image);
}