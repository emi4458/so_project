#include <stdlib.h>
#include <stdio.h>
#include "bit_map.h"

int main(int argc ,char** argv){
    BitMap bitmap;
    BitMap_alloc(&bitmap,8);
    BitMap_setBit(&bitmap,4,1);
    BitMap_print(&bitmap);
    
}