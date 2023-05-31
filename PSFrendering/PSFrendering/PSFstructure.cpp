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

void psfConvolution(cv::Mat& src_image) {

    /*
    src_image.forEach<Pixel>([](Pixel& p, const int* position) -> void {
        
        p.y = 255;
        
    });
    */

    for (int r = 0; r < src_image.rows; ++r) {
        Pixel* ptr = src_image.ptr<Pixel>(r, 0);
        const Pixel* ptr_end = ptr + src_image.cols;
        for (; ptr != ptr_end; ++ptr) {
            ptr->x *= 250;
            ptr->y *= 250;
            ptr->z *= 250;
        }
    }

}



//Auxiliary functions

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