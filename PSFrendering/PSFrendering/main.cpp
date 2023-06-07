#include "PSFrendering.h"


int main() {


	/*-----------------------------------------------------
	----------- LOADING SOURCE IMAGE AND PSFs -------------
	-------------------------------------------------------*/

	auto start = std::chrono::high_resolution_clock::now();

	path cameraPath("E:\\GitHub\\lenses\\brendel-tessar_21_3.0_2.8");
	path imagePath("E:\\GitHub\\test\\rabbits_canon-zoom.exr");

	std::cout << "importing camera from: " << cameraPath << std::endl;

	std::vector<DepthDatabase> kernels;
	loadPSFs(cameraPath, kernels);

	std::cout << "importing image from: " << imagePath << std::endl;
	std::cout << "Loading PSFs nedeed"<< std::endl;

	std::array<cv::Mat, 2> src_image;
	loadEXR(imagePath.string(), src_image);


	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	std::cout << "Done. Execution time: " << duration << " milliseconds." << std::endl;
	std::cout << "Start PSF convolution..." << std::endl;


	/*-----------------------------------------------------
	----------- PSF CONVOLUTION ---------------------------
	------------ Debug: 19.10 minuti ----------------------
	------------ Release: 3 seconds -----------------------
	-------------------------------------------------------*/

	start = std::chrono::high_resolution_clock::now();

	cv::Mat out_image(src_image[0].rows, src_image[0].cols, CV_32FC3);

	psfConvolution(src_image[0], src_image[1], out_image, kernels, ORDER);

	end = std::chrono::high_resolution_clock::now();

	duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

	std::cout << "Done. Execution time: " << duration << " seconds." << std::endl;




	/*-----------------------------------------------------
	----------- EXPORT ------------------------------------
	-------------------------------------------------------*/

	saveEXR("E:\\GitHub\\PSFrendering_optimized\\PSFrendering\\out\\res.exr", out_image);

}