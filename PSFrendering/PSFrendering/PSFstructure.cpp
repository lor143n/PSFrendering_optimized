#include "PSFrendering.h"

std::vector<DepthDatabase> loadPSFs(std::string& camera_path) {

	std::vector<DepthDatabase> psfsDict;

    path p(camera_path);

    if (!exists(p) || !is_directory(p)) {
        std::cout << "ERROR: path error" << std::endl;
        return psfsDict;
    }

    insertAllDepthFolders(p,  psfsDict);

	return psfsDict;
}


void insertAllDepthFolders(path& p, std::vector<DepthDatabase>& depths) {

    //lets try with boost.filesystem


    try {\

        for (directory_entry& dir : directory_iterator(p)) {

            DepthDatabase new_depth(atof(dir.path().filename().generic_string().c_str()));

            for (directory_entry file : directory_iterator(dir)) {

                std::cout << file.path().filename().generic_string() << std::endl;
                
                //import .exr and save the kernel in new_depth.insertPSF
                //set up cameras directories
                auto psfKer = cv::imread(file.path().generic_string(), cv::IMREAD_GRAYSCALE);

                PSF new_psf(.0, .0, psfKer);

                //PSF constructor do a kernel reference import, verify that outside the brackeys works
            }

            depths.push_back(new_depth);
        }

    } catch (const filesystem_error& ex)
    {
        std::cout << ex.what() << std::endl;
    }



    for (DepthDatabase& depth : depths) {

        std::cout << depth.getDepth() << std::endl;
    }

    return;

}

void log(std::string& str) {
    std::cout << str << std::endl;
}