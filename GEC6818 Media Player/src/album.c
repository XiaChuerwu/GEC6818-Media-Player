/*
    显示相册
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "album.h"
#include "linklist_file.h"
#include "dir_music_pic.h"
#include "image_thumbnail.h"
#include "jpeg.h"
#include "touch.h"
#include "lcd.h"

//触摸屏文件路径
static char* event0_path = "/dev/input/event0";
//FrameBuffer文件路径
static char* fb0_path = "/dev/fb0";

static char* path = "./picture";
//相册背景图文件名
static char* album_bg = "photo_bg.jpg";
//返回
static char* exit_jpg = "exit.jpg";

static struct touch_info info;

//定义一个双向循环链表保存所有缩略图照片的原图,用于放大
static Linklist* linklist_jpg;
Node* exit_;

void show_album(){
    int lcd = init_lcd(fb0_path);
    if(lcd == -1)
    {
        perror("");
        return ;
    }

    //创建双向循环链表保存所有有关照片的文件
    Linklist* linklist_photo = create_linklist();
    char* path = "/senming/picture";
    open_dir(path,linklist_photo,PICTURE);
    print_linklist(linklist_photo);

    //查找并且显示相册页面背景图
    Node* p = search_linklist(linklist_photo,album_bg);
    display_jpeg(p->data,0,0);
    //free(p);

    Node* node_photo = linklist_photo->first;
    //展示六张缩略图，除去所有的背景图 _bg.jpg 结尾的图片
    int x0 = 75, y0 = 80;
    int flag = 0;

    //定义一个双向循环链表保存所有缩略图照片的原图,用于放大
    linklist_jpg = create_linklist();

    //展示六个缩略图
    for(int i = 0; i < 6; i++){
        int len = strlen(node_photo->data);
        //判定是不是.jpg或者.jpeg格式图片
        if(strcmp(node_photo->data + len - 4,".jpg") == 0 || strcmp(node_photo->data + len - 5,".jpeg") == 0){
            //判断是不是背景图
            if(strcmp(node_photo->data + len - 7,"_bg.jpg") == 0){
                i -= 1;
                node_photo = node_photo->next;
                continue;
            }
            //判断是不是退出退出按钮
            else if(strcmp(node_photo->data + len - 8,"exit.jpg") == 0){
                i -= 1;
                node_photo = node_photo->next;
                continue;
            }
            //不是背景图就展示缩略图
            else{
                char* inputFile = node_photo->data; // "/senming/picture/home/1.jpg";
                
                //保存原图的位置
                insert_linklist(linklist_jpg,inputFile);

                int len_file_path = strlen(inputFile);
                char outputFile [len_file_path + 5 + 1]; // "/senming/picture/home/1_temp.jpg";

                //先不拷贝.jpg
                strncpy(outputFile,inputFile,len_file_path - 4);

                //再拷贝_temp.jpg
                sprintf(outputFile,"%s%s",outputFile,"_temp.jpg");
                // printf("inputFile = %s\n",inputFile);
                // printf("outputFile = %s\n",outputFile);

                //生成缩略图
                generate_image_thumbnail(inputFile,outputFile);
                if(flag < 3){
                    display_jpeg(outputFile,x0,y0);
                    
                    flag++;
                    x0 = x0 + 250;
                }
                else if(flag == 3){
                    flag = 0;
                    x0 = 75;
                    y0 += 210;
                    display_jpeg(outputFile,x0,y0);
                    x0 = x0 + 250;
                }
            }
        }
        //如果不是jpg文件就跳过
        else{
            i -= 1;
        }
        node_photo = node_photo->next;
    }

    //显示返回的按钮
    exit_ = search_linklist(linklist_photo,exit_jpg);
    display_jpeg(exit_->data,0,0);

    //打开触摸屏幕文件
    open_touch_device(event0_path);

    while(1){
        info = get_touch_info();
        //display_jpeg(linklist_jpg->first->data,0,0);
        //如果是点到了返回就返回主页面
        if(info.x >= 0 && info.x <= 100 && info.y >= 0 && info.y <= 50){
            close_touch();
            destory_lcd();
            destroy_linklist(linklist_photo);

            show_home(fb0_path);
        }
        //否则就是放大图片
        else{
            judgment_region();
        }
        //printf("x = %d,y = %d\n",info.x,info.y);
    }
    // destory_lcd();
    // destroy_linklist(linklist_photo);
    // free(node_photo);
}

/*
    判断点击的是哪个区域
*/
void judgment_region(){
    //判断是放大的哪个图片
    Node* temp_jpg = linklist_jpg->first;
    while(1){
        info = get_touch_info();
        //上面三个
        if(info.y >= 100 && info.y <= 280){
            //判断是那三个
            if(info.x >= 80 && info.x <= 285){
                magnify_photo(temp_jpg);
            }
            else if(info.x >= 400 && info.x <= 600){
                magnify_photo(temp_jpg->next);
            }
            else if(info.x >= 730 && info.x <= 930){
                magnify_photo(temp_jpg->next->next);
            }
        }
        //下面三个
        else if(info.y >= 350 && info.y <= 600){
            //判断是那三个
            temp_jpg = linklist_jpg->first->next->next->next;
            if(info.x >= 80 && info.x <= 285){
                magnify_photo(temp_jpg);
            }
            else if(info.x >= 400 && info.x <= 600){
                magnify_photo(temp_jpg->next);
            }
            else if(info.x >= 730 && info.x <= 930){
                magnify_photo(temp_jpg->next->next);
            }
        }
    }
}

void magnify_photo(Node* temp_jpg){
    display_jpeg(temp_jpg->data,0,0);
    display_jpeg(exit_->data,0,0);
    while(1){
        info = get_touch_info();
        //判断是向左滑还是右滑
        if(info.dir == LEFT){
            temp_jpg = temp_jpg->prev;
            display_jpeg(temp_jpg->data,0,0);
            display_jpeg(exit_->data,0,0);
        }
        else if(info.dir == RIGHT){
            temp_jpg = temp_jpg->next;
            display_jpeg(temp_jpg->data,0,0);
            display_jpeg(exit_->data,0,0);
        }
        //判断是不是点到了返回按钮
        //如果是点到了返回就返回相册页面
        else if(info.x >= 0 && info.x <= 100 && info.y >= 0 && info.y <= 50){
            show_album();
        }
    }
}