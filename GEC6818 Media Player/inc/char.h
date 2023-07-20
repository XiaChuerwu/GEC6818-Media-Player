#ifndef __CHAR_H__
#define __CHAR_H__

/*
    功能：在屏幕任意位置显示任意大小任意颜色的任意一个字符
    参数：
        x0,y0分别是起始点的坐标
        w,h分别是这个字符的宽度和高度
        ch 是这个字符取模数组的首地址
        color 是字体颜色,bg_color是背景颜色
*/
void show_character(int x0, int y0, int w, int h, unsigned char * ch, int color, int bg_color);


#endif