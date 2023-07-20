#ifndef __LCD_H__
#define __LCD_H__


/*
    初始化屏幕：打开并映射
*/
int init_lcd(const char* pathname);

//销毁屏幕，解除映射值并且关闭文件
void destory_lcd();

/*
    显示一个像素点
*/
void display_point(int x, int y, int color);

/*
    清屏
*/
void clear_lcd(int bgcolor);

int show_home(const char * path);


#endif