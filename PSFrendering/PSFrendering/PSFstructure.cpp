#include "PSFrendering.h"

//PSFs Databse construction

void loadPSFs(path camera_path, std::vector<DepthDatabase>& psfsDict) {

    if (!exists(camera_path) || !is_directory(camera_path)) {
        std::cout << "ERROR: path error" << std::endl;
        return;
    }

    try {

        for (directory_entry& dir : directory_iterator(camera_path)) {


            double depth_value = ::atof(dir.path().filename().generic_string().c_str());

            DepthDatabase new_depth(depth_value);

            for (directory_entry file : directory_iterator(dir)) {

                auto psfKer = cv::imread(file.path().generic_string(), cv::IMREAD_ANYCOLOR | cv::IMREAD_ANYDEPTH);

                //sostituire atof con qualcosa che converta in float non in double

                float com_x = std::stof(splitString(file.path().stem().string())[0].c_str());
                float com_y = std::stof(splitString(file.path().stem().string())[1].c_str());

                //PSF constructor do a kernel reference import, verify that outside the brackeys works
                PSF new_psf(com_x, com_y, psfKer);

                new_depth.insertPSF(new_psf);
            }

            psfsDict.push_back(new_depth);
        }

        std::sort(psfsDict.begin(), psfsDict.end());
        
    }
    catch (const filesystem_error& ex)
    {
        std::cout << ex.what() << std::endl;
    }



	return;
}

bool operator<(const DepthDatabase& a, const DepthDatabase& b) {
    return a.m_depth < b.m_depth;
}


//PSF convolution functions

void psfConvolution(cv::Mat& rgb_image, cv::Mat& depth_image, cv::Mat& out_image, std::vector<DepthDatabase> data, int krnl_size) {

    int krnl_range = (krnl_size - 1) / 2;
    
    cv::Mat krnl(krnl_size, krnl_size, CV_32FC1);

    #pragma omp parallel for
    for (int i = krnl_range; i < rgb_image.rows - krnl_range; i++) {
        for (int j = krnl_range; j < rgb_image.cols - krnl_range ; j++) {
        

            //KERNEL BUILDING
            float krnl_sum = 0;
            
            #pragma omp parallel for
            for (int h = 0; h < krnl_size; h++) {
                for (int k = 0; k < krnl_size; k++) {

                    //depth selection //convert getDepth() in float


                    int high_dep_idx = 0;
                    int low_dep_idx = 0;

                    float dep = depth_image.at<float>(i - krnl_range + h, j - krnl_range + k);

                    for (int dep_idx = 0; dep_idx < data.size(); dep_idx++) {


                        if (dep_idx == data.size() - 1) {
                            high_dep_idx = ((int) data.size()) - 1;
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

                    double high_depth = data[high_dep_idx].getDepth();
                    double low_depth = data[low_dep_idx].getDepth();

                    //Position Selection

                    std::vector<PSF> low_positions_krnls;
                    std::vector<PSF> high_positions_krnls;

                    //low

                    for (int psf_idx = 0; psf_idx < data[low_dep_idx].m_psfs.size(); psf_idx++) {

                        data[low_dep_idx].m_psfs[psf_idx].m_distance = std::sqrt(std::powl((i - data[low_dep_idx].m_psfs[psf_idx].m_centre_of_mass_x), 2) + std::pow((j - data[low_dep_idx].m_psfs[psf_idx].m_centre_of_mass_y), 2));

                        if (psf_idx == 0) {
                            low_positions_krnls.push_back(data[low_dep_idx].m_psfs[psf_idx]);
                        }
                        else if (psf_idx < INTERPOLATION_COUNT) {

                            bool done = false;
                            for (int count = 0; count < psf_idx; count++) {

                                if (data[low_dep_idx].m_psfs[psf_idx].m_distance < low_positions_krnls[0].m_distance) {
                                    low_positions_krnls.insert(low_positions_krnls.begin(), data[low_dep_idx].m_psfs[psf_idx]);
                                    done = true;
                                    break;
                                }

                                if (!done)
                                    low_positions_krnls.push_back(data[low_dep_idx].m_psfs[psf_idx]);

                            }
                        }
                        else
                        {
                            for (int count = 0; count < psf_idx; count++) {
                                if (data[high_dep_idx].m_psfs[psf_idx].m_distance < low_positions_krnls[0].m_distance) {
                                    low_positions_krnls.insert(low_positions_krnls.begin(), data[high_dep_idx].m_psfs[psf_idx]);
                                    break;
                                }
                            }
                        }

                    }

                    //high

                    for (int psf_idx = 0; psf_idx < data[high_dep_idx].m_psfs.size(); psf_idx++) {

                        data[high_dep_idx].m_psfs[psf_idx].m_distance = std::sqrt(std::powl((i - data[high_dep_idx].m_psfs[psf_idx].m_centre_of_mass_x), 2) + std::pow((j - data[high_dep_idx].m_psfs[psf_idx].m_centre_of_mass_y), 2));

                        if (psf_idx == 0) {
                            high_positions_krnls.push_back(data[high_dep_idx].m_psfs[psf_idx]);
                        }
                        else if (psf_idx < INTERPOLATION_COUNT) {

                            bool done = false;
                            for (int count = 0; count < psf_idx; count++) {

                                if (data[high_dep_idx].m_psfs[psf_idx].m_distance < high_positions_krnls[0].m_distance) {
                                    high_positions_krnls.insert(high_positions_krnls.begin(), data[high_dep_idx].m_psfs[psf_idx]);
                                    done = true;
                                    break;
                                }

                                if (!done)
                                    high_positions_krnls.push_back(data[high_dep_idx].m_psfs[psf_idx]);

                            }
                        }
                        else
                        {
                            for (int count = 0; count < psf_idx; count++) {
                                if (data[high_dep_idx].m_psfs[psf_idx].m_distance < high_positions_krnls[0].m_distance) {
                                    high_positions_krnls.insert(high_positions_krnls.begin(), data[high_dep_idx].m_psfs[psf_idx]);
                                    break;
                                }
                            }
                        }

                    }

                    //Interpolation (by Position and by Depth)

                    krnl.at<float>(h, k) = data[high_dep_idx].m_psfs[0].m_kernel.at<float>(h,k);
                    krnl_sum += data[high_dep_idx].m_psfs[0].m_kernel.at<float>(h, k);

                }
            }
            

            //Kernel normalization
            krnl = krnl / krnl_sum;

            //Pixel convolution

            for (int x = 0; x < krnl_size; x++) {
                for (int y = 0; y < krnl_size; y++) {

                    out_image.at<cv::Vec3f>(i - krnl_range, j - krnl_range) = krnl.at<float>(x, y) * rgb_image.at<cv::Vec3f>(i - krnl_range + x, j - krnl_range + y);
                
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



void saveEXR(const char fileName[], const cv::Mat& image)
{
    int width = image.cols;
    int height = image.rows;

    Imf::Rgba* pixels = new Imf::Rgba[width * height];

    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {

            cv::Vec3f src_pixel = image.at<cv::Vec3f>(i, j);

            pixels[i * width + j].r = src_pixel[0];
            pixels[i * width + j].g = src_pixel[1];
            pixels[i * width + j].b = src_pixel[2];
            pixels[i * width + j].a = 1;

        }
    }

    Imf::RgbaOutputFile file(fileName, image.cols , image.rows, Imf::WRITE_RGBA); 
    file.setFrameBuffer(pixels, 1, image.cols); //?
    file.writePixels(image.rows);  

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