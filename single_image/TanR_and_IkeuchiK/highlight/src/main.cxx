#include "zHighlightRemoval.h"

int main(int argc, char *argv[]) {
//  if (argc==2){
//    zHighlightRemoval hr(argv[1]);
//    return 0;
//  }
//  else{
//    printf("syntax error: highlight <filename>\n");
//  }

    zHighlightRemoval("../images/fish.ppm");
    return 1;
}
