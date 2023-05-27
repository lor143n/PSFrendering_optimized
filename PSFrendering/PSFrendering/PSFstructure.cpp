#include "PSFrendering.h"

std::vector<DepthDatabase> loadPSFs(std::string camera_path) {


	std::vector<DepthDatabase> psfsDict;

	return psfsDict;
}


void insertAllDepthFolders(std::vector<DepthDatabase>& depths) {

    //lets try with boost.filesystem


    // windows.h implementation
    /*
    WIN32_FIND_DATA data;

    std::wstring path = L"C:\\palle\\*";

    HANDLE hFind = FindFirstFile(path.c_str(), &data);      // DIRECTORY

    path.pop_back();

    if (hFind != INVALID_HANDLE_VALUE) {

        do {

            std::wstring comp = L".";
            std::wstring comp2 = L"..";

            if (_wcsicmp(data.cFileName, comp.c_str()) == 0 || _wcsicmp(data.cFileName, comp2.c_str()) == 0) {
                continue;
            }


            std::wstring depWstring = L"";
            depWstring += data.cFileName;
            DepthDatabase new_depth = new DepthDatabase(atof(data.c));
            depths.push_back();

        } while (FindNextFile(hFind, &data));

        FindClose(hFind);
    }
    */

}