#include "PSFrendering.h"


int main() {


	std::string cameraPath = "E:\\GitHub\\PSFrendering_optimized\\PSFrendering\\lenses";
	std::string imagePath = "E:\\GitHub\\PSFrendering_optimized\\PSFrendering\\source";

	std::cout << "importing camera from: " << cameraPath << std::endl;
	std::cout << "importing image from: " << cameraPath << std::endl;

	//measure time

	auto src_image = cv::imread(imagePath, cv::IMREAD_UNCHANGED);

	std::vector<DepthDatabase> kernels = loadPSFs(cameraPath);

	//print loading time

	//measure time

	//image padding (how to do with OpenCV?)

	//PSF convolution (how to do manage depth and rgb?)

	//print convolution time

	//export output image


}