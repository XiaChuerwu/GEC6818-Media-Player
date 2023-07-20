#include "linklist_file.h"

#ifndef __DIR_H__
#define __DIR_H__

enum option{
    MUSIC,
    PICTURE
};

/*
    参数：
        first：         原目录
        second：        文件名
        linklist_mp3：  双向循环链表保存所有mp3文件
        opt：           判断是图片还是音频
*/
void get_all_music_or_pic(const char* first, const char* second, Linklist* linklist,enum option opt);

/*
    功能：选择一个目录打开所有的音乐文件
    参数：
        pathname：原目录，即要求的目录路径
*/
void open_dir(const char* pathname, Linklist* linklist,enum option opt);

#endif