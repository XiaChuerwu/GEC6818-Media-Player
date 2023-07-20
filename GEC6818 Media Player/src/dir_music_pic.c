#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dir_music_pic.h"
#include "linklist_file.h"

/*
    参数：
        first：         原目录
        second：        文件名
        linklist_mp3：  双向循环链表保存所有mp3文件
        opt：           判断是图片还是音频
*/
void get_all_music_or_pic(const char* first, const char* second, Linklist* linklist,enum option opt){
    struct stat* buf = (struct stat*)malloc(sizeof(struct stat));

    //使用 strncpy 和 strcat 函数实现将原目录fisrt和文件名进行拼接，将拼接后的字符串目录保存在dest字符数组中
    char pathname[strlen(first)+strlen(second)+1+1];

    sprintf(pathname,"%s/%s",first,second);

    //打印所有的文件名路径
    //printf("%s\n",pathname);

    //调用stat函数获取dest字符数组中存储的文件路径名 来获取文件的信息/状态
    int r = lstat(pathname,buf);
    if(r == -1){
        perror("lstat error");
    }
    //printf("second = %s\n",second);

    //如果是普通文件就判断是不是.mp3文件
    if(buf->st_mode & S_IFREG){
        int len = strlen(second);
        //判断opt是要获取音乐文件还是图片文件
        if(opt == MUSIC){
            if(strcmp(second+len-4,".mp3") == 0){
                insert_linklist(linklist,pathname);
                //测试打印输入将.MP3文件存储进链表
                //printf("%s\n",linklist_mp3->first->data);
            }
        }
        else if(opt == PICTURE){
            if(strcmp(second+len-4,".bmp") == 0 
            || strcmp(second+len-4,".jpg") == 0 
            || strcmp(second+len-5,".jpeg") == 0
            ){
                insert_linklist(linklist,pathname);
            }
        }

    }
    //判断是不是目录文件除 . 和 .. 
    else if(strcmp(second,".") && strcmp(second,"..")){
        open_dir(pathname,linklist,opt);
        //printf("seconde = %s\n",second);
    }

    free(buf);
}

/*
    功能：选择一个目录打开所有的音乐文件
    参数：
        pathname：  原目录，即要求的目录路径
        linklist：  保存所有的音乐或图片文件路径
        opt：       判断是要获取图片的还是音频的
*/
void open_dir(const char* pathname, Linklist* linklist,enum option opt){
    //打开目录
    DIR *dirp = opendir(pathname);
    if(dirp == NULL){
        perror("原路径出错");
        return ;
    }

    //循环读取该目录的文件
    struct dirent * p;
    int num = 0;
    while(1){
        p = readdir(dirp);
        if(p == NULL)
            break;
        get_all_music_or_pic(pathname,p->d_name,linklist,opt);
        //printf("%s\n",p->d_name);
    }

    free(p);
    closedir(dirp);

    return ;
}
