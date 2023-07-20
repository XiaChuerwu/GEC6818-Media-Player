#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

#include "jpeglib.h"
#include "jpeg.h"
#include "lcd.h"

void display_jpeg(const char * pathname, int x0, int y0){

    //1.    初始化对象
    struct jpeg_decompress_struct dinfo; //声明一个解压的对象
    struct jpeg_error_mgr jerr; //声明一个出错信息的对象
    dinfo.err = jpeg_std_error(&jerr); //初始化这个出错对象
    jpeg_create_decompress(&dinfo); //初始化dinfo这个解压对象

    //2.    指定要解压的文件
    FILE* infile;
    infile = fopen(pathname, "r");
	if (infile == NULL){
        printf("open %s fail",pathname);
        perror("");
        return ;
    }

    jpeg_stdio_src(&dinfo, infile); //指定要解压的图像文件
    
    //3.    调用jpeg_read_header()获取图像信息
    jpeg_read_header(&dinfo, TRUE);

    //不需要4.    用于设置jpeg解压对象dinfo的一些参数。可采用默认参数


    //5.    调用jpeg_start_decompress()启动解压过程
    jpeg_start_decompress(&dinfo);


    //6.    解压一行数据并处理，通常的代码是这样子的:
    unsigned char *buffer = malloc(dinfo.output_width * dinfo.output_components);
    //dinfo.output_scanline , 表示的意思是，已经解压了多少行

    int x = 0, y = 0;
    while (dinfo.output_scanline < dinfo.output_height){
        jpeg_read_scanlines(&dinfo,  //解压对象
                            &buffer,//保存解压后数据的二级指针, 
                            1 //读取多少行数据来解压
                            );	
                            //dinfo.output_scanline + 1
        unsigned int color;
        for(x = 0; x < dinfo.output_width; x++){
            if(dinfo.output_components == 3){
                color = buffer[x * 3 + 0] << 16 | buffer[x * 3 + 1] << 8 | buffer[x * 3 + 2];
            }
            else{
                color = buffer[x * 3 + 0] << 24 | buffer[x * 3 + 1] << 16 | buffer[x * 3 + 2] << 8 | buffer[x * 3 + 3];
            }
            display_point(x0 + x,y + y0,color);
        }
        y++;
    }

    //7.    调用jpeg_finish_decompress()完成解压过程
    jpeg_finish_decompress(&dinfo);

    //8.    调用jpeg_destroy_decompress释放jpeg解压对象dinfo
    jpeg_destroy_decompress(&dinfo);

    fclose(infile);
}