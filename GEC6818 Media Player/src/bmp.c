#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <string.h>
#include <sys/mman.h>

#include "bmp.h"
#include "lcd.h"

void display_bmp(const char * pathname, int x0, int y0){
    //打开文件
    int fd = open(pathname,O_RDONLY);
    if(fd == -1){
        printf("open %s fail",pathname);
        perror("");
        return ;
    }

    clear_lcd(0);

    //获取宽度和高度
    int width = 0, height = 0;
    lseek(fd,0x12,SEEK_SET);
    read(fd,&width,4);
    read(fd,&height,4);

    //printf("%d %d",width,height);

    //读取像素数组的信息
    lseek(fd,54,SEEK_SET);
    unsigned char buf[width*height*24/8];
    read(fd,buf,width*height*24/8);

    int n = 0;
    unsigned char R, G, B;
    //读取图片所有像素点的信息，依次是蓝绿红 蓝绿红
    //显示图片
    for(int y = height - 1; y >= 0; y--){
        for(int x = 0; x < width; x++){
            //获取RGB
            B = buf[n++];
            G = buf[n++];
            R = buf[n++];
            int color = R << 16 | G << 8 | B;
            display_point(x+x0,y+y0,color);
        }
    }
}