#include "zHighlightRemoval.h"
#include <dirent.h>
#include <iostream>
int main(int argc, char *argv[]) {

//  if (argc==2){
//    zHighlightRemoval hr(argv[1]);
//    return 0;
//  }
//  else{
//    printf("syntax error: highlight <filename>\n");
//  }
    DIR *dpdf;
    struct dirent *epdf;
    std::string path = "../input";
    dpdf = opendir(path.c_str());
    if (dpdf != nullptr){
        while (epdf = readdir(dpdf)){
//            std::cout << epdf->d_name << std::endl;
            if (strcmp(epdf->d_name, ".") != 0 && strcmp(epdf->d_name, "..") != 0 )
//                std::cout << epdf->d_name << std::endl;
                zHighlightRemoval(path + "/" + epdf->d_name);
        }
    }
    closedir(dpdf);

//
//    zHighlightRemoval("../images/fish_1_5.ppm");
    return 1;
}
