#include "lcd.h"
#include "char.h"
#include <stdio.h>
/*
    功能：在屏幕任意位置显示任意大小任意颜色的任意一个字符
    参数：
        x0,y0分别是起始点的坐标
        w,h分别是这个字符的宽度和高度
        ch 是这个字符取模数组的首地址
        color 是字体颜色,bg_color是背景颜色
*/
void show_character(int x0, int y0, int w, int h, unsigned char * ch, int color, int bg_color){
    int x = 0, y = 0;
    for(int i = 0; i < w*h/8; i++){
        //循环bit位,从高位7开始到低位0
        for(int j = 7 ; j >= 0; j--){
            //找i,j 和 x,y 的关系 16个j算一行
            //printf("%d ",(char_yuan[i] >> j) & 1 );
            x = 8 * (i % (w/8)) + 7 - j + x0;
            y = i / (w/8) + y0;
            if((ch[i] >> j) & 1 != 0){
                display_point(x,y,color);
            }
            else{
                display_point(x,y,bg_color);
            }
        }
        printf("\n");
    }
}