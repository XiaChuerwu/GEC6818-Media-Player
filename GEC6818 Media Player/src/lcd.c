#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <string.h>
#include <sys/mman.h>

#include "lcd.h"
#include "jpeg.h"
#include "linklist_file.h"
#include "dir_music_pic.h"
#include "music.h"
#include "touch.h"
#include "image_thumbnail.h"
#include "album.h"

static int fd;//帧缓冲文件描述符
static int* p;
static int lcd_width;
static int lcd_height;
static int lcd_pixel_bit;

//触摸屏文件路径
static char* event0_path = "/dev/input/event0";
//FrameBuffer文件路径
static char* fb0_path = "/dev/fb0";

static char* path = "."; 
static char* home_bg = "home_bg.jpg";


/*
    初始化屏幕：打开并映射
*/
int init_lcd(const char* pathname){
    //打开文件
    fd = open(pathname,O_RDWR);
    if(fd == -1){
        perror("open lcd error");
        return -1;
    }

    //获取屏幕信息
    struct fb_var_screeninfo var;
    ioctl(fd,FBIOGET_VSCREENINFO,&var);// 失败返回-1，同时errno被设置

    //如果成功了，那么屏幕的参数信息就保存在 var中了
    lcd_width = var.xres;
    lcd_height = var.yres;
    lcd_pixel_bit = var.bits_per_pixel;

    //映射
    p = mmap(NULL,lcd_height*lcd_width*lcd_pixel_bit/8,PROT_WRITE | PROT_READ,
            MAP_SHARED,fd,0);
    if(p == NULL){
        perror("mmap error");
        return -1;
    }

    return 0;
}

//销毁屏幕，解除映射值并且关闭文件
void destory_lcd(){
    munmap(p,lcd_height*lcd_width*lcd_pixel_bit/8);
    close(fd);
}

/*
    显示一个像素点
*/
void display_point(int x, int y, int color){
    if(x >= 0 && x < lcd_width && y >= 0 && y < lcd_height)
        *(p + lcd_width*y + x) = color;
}

/*
    清屏
*/
void clear_lcd(int bgcolor){
    for(int i = 0 ; i < lcd_height; i++){
        for(int j = 0 ; j < lcd_width; j++){
            *(p + lcd_width*i + j) = bgcolor;
        }
    }

}

/*
    功能:
        显示主页面
*/
int show_home(const char * fb0_path){
    int lcd = init_lcd(fb0_path);
    if(lcd == -1){
        return -1;
    }

    //创建双向循环链表保存所有有关home的文件
    Linklist* linklist = create_linklist();
    open_dir(path,linklist,PICTURE);
    print_linklist(linklist);

    //Node* p = linklist->first;
    int len;
    Node* p = search_linklist(linklist,home_bg);
    //先显示主页面背景图
    display_jpeg(p->data,0,0);

    // char* inputFile = "/senming/picture/home/home_bg.jpg";
    // char* outputFile = "/senming/picture/home_temp.jpg";
    // generate_image_thumbnail(inputFile,outputFile);
    // display_jpeg(outputFile,0,0);

    
    //打开触摸屏幕文件
    open_touch_device(event0_path);
    struct touch_info info;
    while(1){
        info = get_touch_info();
        //判断是否点击到了播放器图标
        if(info.x >= 150 && info.x <= 340 && info.y >= 200 && info.y <= 400){
            close_touch();
            destory_lcd();
            destroy_linklist(linklist);

            show_music();
        }
        //判断是否点击到了相册图标
        else if(info.x >= 690 && info.x <= 850 && info.y >= 200 && info.y <= 400){
            close_touch();
            destory_lcd();
            destroy_linklist(linklist);

            show_album();
        }
        printf("x = %d,y = %d\n",info.x,info.y);
    }
}