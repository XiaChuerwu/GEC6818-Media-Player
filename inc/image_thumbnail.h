/*
    对JPG图片进行缩略图处理
*/

#ifndef __IMAGE_THUMBNAIL_H
#define __IMAGE_THUMBNAIL_H



/*
    功能：
        生成图片的缩略图（图片的一个缩小版本）
    参数:
        inputFile： 源文件路径名
        outputFile：输出缩略图路径名
*/
int generate_image_thumbnail(const char* inputFile, const char* outputFile);

int write_JPEG_file (const char * filename, unsigned char* image_buffer, int quality,int image_height, int image_width);


/*参数为：
 *返回图片的宽度(w_Dest),
 *返回图片的高度(h_Dest),
 *返回图片的位深(bit_depth),
 *源图片的RGB数据(src),
 *源图片的宽度(w_Src),
 *源图片的高度(h_Src)
 */
unsigned char* do_Stretch_Linear(int w_Dest,int h_Dest,int bit_depth,unsigned char *src,int w_Src,int h_Src);

unsigned char* ReadJpeg(const char * pathname, int* w, int* h);

#endif