#ifndef __TOUCH_H__
#define __TOUCH_H__


enum dirction{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct touch_info{
    int x;
    int y;
    enum dirction dir;
};

int open_touch_device(const char * pathname);

void close_touch();

struct touch_info get_touch_info();

#endif