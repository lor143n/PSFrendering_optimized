#include "PSFrendering.h"

//PSFs Databse construction

void loadPSFs(std::string& camera_path, std::vector<DepthDatabase>& psfsDict) {

    path p(camera_path);

    if (!exists(p) || !is_directory(p)) {
        std::cout << "ERROR: path error" << std::endl;
        return;
    }

    makePSFsDictionary(p,  psfsDict);

	return;
}


void makePSFsDictionary(path& p, std::vector<DepthDatabase>& depths) {

    try {

        for (directory_entry& dir : directory_iterator(p)) {

            DepthDatabase new_depth(dir.path().filename().generic_string().c_str());

            for (directory_entry file : directory_iterator(dir)) {

                auto psfKer = cv::imread(file.path().generic_string(), cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);

                //sostituire atof con qualcosa che converta in float non in double
                float com_x = atof(splitString(file.path().stem().generic_string())[0].c_str());
                float com_y = atof(splitString(file.path().stem().generic_string())[1].c_str());

                //PSF constructor do a kernel reference import, verify that outside the brackeys works
                PSF new_psf(com_x, com_y, psfKer);

                new_depth.insertPSF(new_psf);
            }

            depths.push_back(new_depth);
        }

    } catch (const filesystem_error& ex)
    {
        std::cout << ex.what() << std::endl;
    }

   
    return;
}


//PSF convolution functions

void psfConvolution(cv::Mat& rgb_image, cv::Mat& depth_image) {

    /*
    src_image.forEach<Pixel>([](Pixel& p, const int* position) -> void {
        
        p.y = 255;
        
    });
    */
    /*
    for (int r = 0; r < src_image.rows; ++r) {
        Pixel* ptr = src_image.ptr<Pixel>(r, 0);
        const Pixel* ptr_end = ptr + src_image.cols;
        for (; ptr != ptr_end; ++ptr) {
            ptr->x = 255;
            ptr->y = 255;
            ptr->z = 255;
        }
    }
    */

    for (int i = 0; i < rgb_image.rows; i++) {

        for (int j = 0; j < rgb_image.cols; j++) {
        
            //std::cout << src_image.at<cv::Vec3b>(i, j) << std::endl;
        }

    }

}



//Auxiliary functions
//Import OpenEXR
void loadEXR(std::string path, std::array<cv::Mat, 2>& image_result) {

    Imf::RgbaInputFile file(path.c_str());
    Imath::Box2i dw = file.dataWindow();
    int width = dw.max.x - dw.min.x + 1;
    int height = dw.max.y - dw.min.y + 1;

    Imf::Array2D<Imf::Rgba> pixels;
    pixels.resizeErase(height, width);

    file.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
    file.readPixels(dw.min.y, dw.max.y);

    cv::Mat rgbImage(height, width, CV_32FC3);
    cv::Mat depthImage(height, width, CV_32FC1);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {

            const Imf::Rgba& pixel = pixels[i][j];
            rgbImage.at<cv::Vec3f>(i, j) = cv::Vec3f(pixel.r, pixel.g, pixel.b);
            depthImage.at<float>(i, j) = pixel.a;

        }
    }

    image_result[0] = rgbImage;
    image_result[1] = depthImage;

}

std::vector<std::string> splitString(std::string str) {

    std::vector<std::string> tokens;
    std::string str_found = "";

    for (auto& x : str) {
      
        if (x == '-') {
            tokens.push_back(str_found);
            str_found = "";
            continue;
        }
            
        str_found += x;
    }

    tokens.push_back(str_found);
    return tokens;
}