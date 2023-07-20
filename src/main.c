#include <stdio.h>

#include "lcd.h"

//FrameBuffer文件路径
static char* fb0_path = "/dev/fb0";

int main(){

    int r = show_home(fb0_path);
    if(r == -1){
        printf("open fb0 fail");
    }

    return 0;
}