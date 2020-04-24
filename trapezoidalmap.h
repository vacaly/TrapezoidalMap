//
//  trapezoidalmap.h
//  tmp
//
//  Created by CynsierWang on 2020/4/14.
//  Copyright © 2020年 CynsierWang. All rights reserved.
//

#ifndef trapezoidalmap_h
#define trapezoidalmap_h
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <utility>
using namespace std;

typedef struct point{
    float x;
    float y;
    point(float x1, float y1){x=x1;y=y1;}
    point(){x=0;y=0;}
}Point,*PointPtr;

typedef struct line{
    PointPtr pointl;
    PointPtr pointr;
    line(PointPtr l,PointPtr r){pointl = l;pointr = r;}
    line(){pointl = NULL;pointr = NULL;}
}Line, *LinePtr;

struct node;
typedef struct leaf{
    PointPtr pointl;
    PointPtr pointr;
    LinePtr up;
    LinePtr bottom;
    leaf * rightUp;
    leaf * rightDown;
    node * parent;
    leaf(PointPtr l,PointPtr r,LinePtr u,LinePtr b){
        pointl = l;
        pointr = r;
        up = u;
        bottom = b;
        rightUp = NULL;
        rightDown = NULL;
        parent = NULL;
    }
    leaf(){};
}Area, *AreaPtr;

typedef struct node{
    int classid;//0 point, 1 line, 2 area leaf
    PointPtr p;
    LinePtr l;
    AreaPtr s;
    node * leftchild;
    node * rightchild;
}Node,*NodeList;

void initTrape(int n, float * arr);
bool isAbove(LinePtr l, float x, float y);
NodeList LookArea(float x,float y);
void insertLine(int lineid);
void passleaves(NodeList r);
void printTrape();
#endif /* trapezoidalmap_h */
