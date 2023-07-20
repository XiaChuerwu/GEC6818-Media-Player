#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdlib.h>

#include "touch.h"

static int fd;

int open_touch_device(const char * pathname){
    fd = open(pathname,O_RDONLY);
    if(fd == -1){
        perror("open dev fail");
        return -1;
    }

    return 0;
}

void close_touch(){
    close(fd);
}

struct touch_info get_touch_info(){
    struct input_event ev;
    struct touch_info info;
    
    int r;
    int x = -1, y = -1;

    while(1){
        r = read(fd,&ev,sizeof(ev));
        if(r == -1){
            perror("no value");
            break;
        }
        if(r == 0)
            continue;
        if(ev.type == EV_ABS && ev.code == REL_X){
            if(x == -1)
                x = ev.value;
            info.x = ev.value;
        }
        if(ev.type == EV_ABS && ev.code == REL_Y){
            if(y == -1)
                y == ev.value;
            info.y = ev.value;
        }
        if(ev.type == EV_KEY && ev.code == BTN_TOUCH && ev.value == 0)
            break;
    }

    //判断四个方向
    //根据起点和终点坐标得到滑动方向
    //思路：分4种情况
    //向上：起点y坐标大于终点y坐标并且 △y大于△x
    //向下：起点y坐标小于终点y坐标并且 △y大于△x
    //向左：起点x坐标大于终点x坐标并且 △x大于△y
    //向左：起点x坐标小于终点x坐标并且 △x大于△y
    if(y > info.y && abs(y-info.y) > abs(x-info.x)){
        info.dir = UP;
    }
    else if(y < info.y && abs(y-info.y) > abs(x-info.x)){
        info.dir = DOWN;
    }
    else if(x > info.x && abs(y-info.y) < abs(x-info.x)){
        info.dir = LEFT;
    }
    else{
        info.dir = RIGHT;
    }

    return info;
}
