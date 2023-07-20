#include "linklist_file.h"

#ifndef __MUSIC_H__
#define __MUSIC_H__

void show_music();

/*
    功能：
        播放音乐
    参数：
        linklist_mp3 
*/
void play_music(Node* node_mp3);

/*
    功能：
        暂停音乐
    参数：
        linklist_mp3 
*/
void stop_music();


/*
    功能：
        终止音乐
*/
void end_music();

/*
    功能：
        切换到上一首歌曲
    参数：
        node_mp3
*/
Node* pre_music(Node* node_mp3);

/*
    功能：
        切换到下一首歌曲
    参数：
        node_mp3
*/
Node* next_music(Node* node_mp3);
#endif