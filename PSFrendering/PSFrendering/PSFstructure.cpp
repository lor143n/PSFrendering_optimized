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

void psfConvolution(cv::Mat& rgb_image, cv::Mat& depth_image, cv::Mat& out_image, std::vector<DepthDatabase> data, int krnl_size) {


    int krnl_range = (krnl_size - 1) / 2;
    

    for (int i = krnl_range; i < rgb_image.rows - krnl_range; i++) {

        for (int j = krnl_range; j < rgb_image.cols - krnl_range ; j++) {
        
            //std::cout << src_image.at<cv::Vec3b>(i, j) << std::endl;

            //KERNEL BUILDING

            cv::Mat krnl;
            float krnl_sum = .0;

            for (int h = 0; h < krnl_size; h++) {
                for (int k = 0; k < krnl_size; k++) {

                    //depth selection //convert getDepth() in float


                    int high_dep_idx = 0;
                    int low_dep_idx = 0;

                    float dep = depth_image.at<cv::float16_t>(i - krnl_range + h, j - krnl_range + k);

                    for (int dep_idx = 0; dep_idx < data.size(); dep_idx++) {


                        if (dep_idx == data.size() - 1) {
                            high_dep_idx = data.size() - 1;
                            low_dep_idx = high_dep_idx - 1;
                            break;
                        }

                        if (dep_idx == 0 && dep < data[dep_idx].getDepth()) {
                            high_dep_idx = dep_idx + 1;
                            low_dep_idx = dep_idx;
                            break;
                        }

                        if (dep >= data[dep_idx].getDepth() && dep <= data[dep_idx+1].getDepth()) {
                            high_dep_idx = dep_idx + 1;
                            low_dep_idx = dep_idx;
                            break;
                        }

                    }

                    float high_depth = data[high_dep_idx].getDepth();
                    float low_depth = data[low_dep_idx].getDepth();


                    //Position Selection

                    //Interpolation (by Position and by Depth)


                }
            }


            //Kernel normalization
            krnl = krnl / krnl_sum;

            //Pixel convolution

            for (int x = 0; x < krnl_size; x++) {
                for (int y = 0; y < krnl_size; y++) {

                    out_image.at<cv::Vec3f>(i - krnl_range, j - krnl_range) = krnl.at<cv::float16_t>(x, y) * rgb_image.at<cv::Vec3f>(i - krnl_range + x, j - krnl_range + y);
                
                }
            }

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