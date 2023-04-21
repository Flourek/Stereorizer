
#include <Python.h>
#include <fstream>
//#include "opencv2/opencv.hpp"
#include "iostream"
#include "header.h"
//#include "Image.h"
#include "Depth.h"
#include "Opt.h"

void importModules() {

    FILE* fp;
    const char* file = "import_modules.py";
    fp = fopen(file, "r");
    PyRun_SimpleFile(fp, file);
    fclose(fp);

}

int generateDepthMap(std::string input_path, std::string model_path, Depth &depth, PyInterpreterState *interp) {

    // Get the pathname of the new depth image
    std::string result_path = input_path.substr(input_path.find_last_of("/\\") + 1);
    result_path = result_path.substr(0, result_path.find_last_of("."));
    result_path = "./output/" + result_path + "-dpt_beit_large_512.png";

    auto& opt = Opt::Get();

//     Skip generating if file already exists
    std::ifstream infile(result_path);
    if ( !infile.good() ){
        PyThreadState* tstate = PyThreadState_New(interp);
        PyThreadState_Swap(tstate);

        std::cout << "Generating depth map" << std::endl;

        if(opt.midas_first_execution){
            importModules();
            opt.midas_first_execution = false;
        }

        int argc = 2;
        wchar_t** wargv = new wchar_t*[argc];
        FILE* fp;
        const char* file = "inference_depth.py";

        wargv[0] = Py_DecodeLocale(input_path.c_str(), nullptr);
        wargv[1] = Py_DecodeLocale(model_path.c_str(), nullptr);

        Py_SetProgramName(wargv[0]);

        PySys_SetArgv(argc, wargv);

        fp = fopen(file, "r");
        PyRun_SimpleFile(fp, file);

        for(int i = 0; i < argc; i++)
        {
            PyMem_RawFree(wargv[i]);
            wargv[i] = nullptr;
        }

        fclose(fp);
        delete[] wargv;
        wargv = nullptr;

    }

    std::cout << infile.good() << std::endl;

    depth = Depth(result_path);

    return 1;

}
