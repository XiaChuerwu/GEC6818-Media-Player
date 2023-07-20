#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <string.h>
#include <sys/mman.h>
#include <dirent.h>
#include <linux/input.h>

#include "lcd.h"
#include "music.h"
#include "linklist_file.h"
#include "dir_music_pic.h"
#include "jpeg.h"
#include "touch.h"

//播放标志默认是0，就是暂停状态，1就是播放状态
static int play_flag = 0;
/*初始音量*/
static int vol = 0;
//初始播放默认的暂停的，如果是第一次点击就开始播放，如果是第二次或者多次点击就转为恢复播放
static int first_flag = 0;

//查找的目录
static char* path = "./music";
//触摸屏文件路径
static char* event0_path = "/dev/input/event0";
//FrameBuffer文件路径
static char* fb0_path = "/dev/fb0";

//音乐背景图的文件名
static char* music_bg = "music_bg.jpg";
//返回的的图片
static char* exit_jpg = "exit.jpg";
//播放的图片
static char* play_jpg = "play.jpg";
//暂停的图片
static char* stop_jpg = "stop.jpg";
//上一首的图片
static char* pre_jpg = "pre.jpg";
//下一首的图片
static char* next_jpg = "next.jpg";


/*
    功能：
        显示音乐播放器的背景
*/
void show_music(){
    int lcd = init_lcd(fb0_path);
    if(lcd == -1)
    {
        perror("");
        return ;
    }

    //创建双向循环链表保存"."目录下面也就是当前目录及其子目录下所有的PICTURE文件
    Linklist* linklist = create_linklist();

    open_dir(path,linklist,PICTURE);
    print_linklist(linklist);

    int len;
    //查找音乐背景图并显示
    Node* p = search_linklist(linklist,music_bg);
    display_jpeg(p->data,0,0);

    //查找并显示返回的的图片
    p = search_linklist(linklist,exit_jpg);
    display_jpeg(p->data,0,0);

    //打开触摸屏幕文件
    open_touch_device(event0_path);
    struct touch_info info;

    //查找并显示暂停播放的图片
    p = search_linklist(linklist,stop_jpg);
    display_jpeg(p->data,336,176);

    //查找并显示上一首的图片
    p = search_linklist(linklist,pre_jpg);
    display_jpeg(p->data,272 - 64 - 30 - 32,208);

    //查找并显示下一首的图片
    p = search_linklist(linklist,next_jpg);
    display_jpeg(p->data,464 + 64 + 30 + 32,208);

    //获取目标目录下所有的mp3文件
    //创建双向循环链表保存所有的mp3文件
    Linklist* linklist_mp3 = create_linklist();
    open_dir(path,linklist_mp3,MUSIC);
    print_linklist(linklist);

    Node* node_mp3 = linklist_mp3->first;
    //判断是否点击到了播放器图标
    while(1){
        info = get_touch_info();
        //返回主页面
        if(info.x >= 0 && info.x <= 125 && info.y >= 0 && info.y <= 135){
            destory_lcd();
            destroy_linklist(linklist);
            end_music();
            show_home(fb0_path);
        }
        //暂停播放
        else if(info.x >= 450 && info.x <= 600 && info.y >= 200 && info.y <= 400){
            //如果是暂停状态就转为播放状态
            if(play_flag == 0){
                play_flag = 1;
                play_music(node_mp3);
                //查找并显示开始播放的图片
                p = search_linklist(linklist,play_jpg);
                display_jpeg(p->data,336,176);
                
            }
            else if(play_flag == 1){
                play_flag = 0;
                stop_music();
                //查找并显示暂停播放的图片
                p = search_linklist(linklist,stop_jpg);
                display_jpeg(p->data,336,176);
            }
            
        }
        //上一首
        else if(info.x >= 180 && info.x <= 270 && info.y >= 260 && info.y <= 340){
            node_mp3 = pre_music(node_mp3);
            printf("上一首\n");
        }
        //下一首
        else if(info.x >= 750 && info.x <= 840 && info.y >= 260 && info.y <= 340){
            node_mp3 = next_music(node_mp3);
            printf("下一首\n");
        }
        printf("x = %d, y = %d\n",info.x,info.y);
    }
    destory_lcd();
    destroy_linklist(linklist);
}
/*
    功能：
        播放音乐
    参数：
        node_mp3
*/
void play_music(Node* node_mp3){
    //判断是不是第一次播放
    if(first_flag == 0){
        char command[100] = {0};
        sprintf(command, "madplay %s -a %d &", node_mp3->data,vol);
        printf("%s\n", command);
        system(command);
        //把首次播放标志置1
        first_flag = 1;
        printf("开始播放\n");
    }
    //否则就恢复播放
    else{
        system("killall -CONT madplay &");
        printf("恢复播放\n");
    }
}


/*
    功能：
        暂停音乐
*/
void stop_music(){
    system("killall -STOP madplay &");
    printf("暂停播放\n");
}


/*
    功能：
        终止音乐
*/
void end_music(){
    system("killall -9 madplay");//利用system函数调用killall命令将madplay终止掉 
    printf("音乐已被终止\n");
}

/*
    功能：
        切换到上一首歌曲
    参数：
        node_mp3
*/
Node* pre_music(Node* node_mp3){
    //先终止当前的音乐
    end_music();

    //将第一次播放置0
    first_flag = 0;

    //再切换到上一首歌曲
    node_mp3 = node_mp3->prev;
    printf("%s\n",node_mp3->data);

    play_music(node_mp3);

    return node_mp3;
}

/*
    功能：
        切换到下一首歌曲
    参数：
        node_mp3
*/
Node* next_music(Node* node_mp3){
    //先终止当前的音乐
    end_music();

    //将第一次播放置0
    first_flag = 0;

    //再切换到上一首歌曲
    node_mp3 = node_mp3->next;
    printf("%s\n",node_mp3->data);

    play_music(node_mp3);

    return node_mp3;
}