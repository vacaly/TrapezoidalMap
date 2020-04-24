//
//  trapezoidalmap.cpp
//  tmp
//
//  Created by CynsierWang on 2020/4/15.
//  Copyright © 2020年 CynsierWang. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <utility>
#include <map>
#include <fstream>
#include "trapezoidalmap.h"
using namespace std;

int pnum;
float lx, rx, uy, dy;
Point WN, EN, WS, ES;
Line Nline, Sline;
NodeList N0, N1, N2, N3, N4, N5;
NodeList root;
Point * points;
Line * lines;
Area a_;
AreaPtr A0, A1, A2;
map<AreaPtr,int> leavesSet;
int leavescnt;

void initTrape(int n, float * arr) {
    root = (NodeList)malloc(sizeof(Node));
    root->classid = 2;
    leavescnt = 0;
    pnum = n;
    lx = arr[0];
    rx = arr[0];
    uy = arr[1];
    dy = arr[1];
    points = (Point*)malloc(sizeof(Point)*n);
    lines = (Line*)malloc(sizeof(Line)*n/2);
    //Create points and lines sets
    for(int i=0;i<n/2;i++){
        points[i*2] = Point(arr[i*4],arr[i*4+1]);
        points[i*2+1] = Point(arr[i*4+2],arr[i*4+3]);
        lines[i] = Line(&points[i*2],&points[i*2+1]);
        //Trape borders
        lx = lx>arr[i*4]   ? arr[i*4]:lx;
        lx = lx>arr[i*4+2] ? arr[i*4+2]:lx;
        rx = rx<arr[i*4]   ? arr[i*4]:rx;
        rx = rx<arr[i*4+2] ? arr[i*4+2]:rx;
        uy = uy<arr[i*4+1] ? arr[i*4+1]:uy;
        uy = uy<arr[i*4+3] ? arr[i*4+3]:uy;
        dy = dy>arr[i*4+1] ? arr[i*4+1]:dy;
        dy = dy>arr[i*4+3] ? arr[i*4+3]:dy;
    }
    lx=lx<0? lx : 0;
    rx=rx>800? rx : 800;
    dy=dy<0? dy : 0;
    uy=uy>600? uy : 600;
    WN.x=lx;WN.y=uy;EN.x=rx;EN.y=uy;
    WS.x=lx;WS.y=dy;ES.x=rx;ES.y=dy;
    Nline = Line(&WN,&EN);Sline = Line(&WS,&ES);
    a_ = Area(&WN,&EN,&Nline,&Sline);
    root->s = &a_;
    root->leftchild = NULL;
    root->rightchild = NULL;
};

bool isAbove(LinePtr l, float x, float y){
    float y_ = (l->pointl->y - l->pointr->y)/(l->pointl->x - l->pointr->x)*(x - l->pointl->x)+l->pointl->y;
    if(y_<y)
        return true;
    else
        return false;
}

NodeList LookArea(float x,float y){
    NodeList n = NULL;
    NodeList r = root;
    while(r->classid!=2){
        if(r->classid==0){
            n = r;
            if(x < r->p->x)
                r = r->leftchild;
            else
                r = r->rightchild;
        }else{
            if(isAbove(r->l,x,y))
                r = r->leftchild;
            else
                r = r->rightchild;
        }
    }
    return r;
}

void passleaf(NodeList r){
    if(r->classid==2){
        map<AreaPtr,int>::iterator iter;
        iter = leavesSet.find(r->s);
        if(iter==leavesSet.end()){
            leavescnt++;
            leavesSet.insert(pair<AreaPtr, int>(r->s, leavescnt));
        }
    }else{
        passleaf(r->leftchild);
        passleaf(r->rightchild);
    }
    return;
}

void printTrape(){
    ofstream ou("Trapezoid.json");
    ou<<"{";
    passleaf(root);
    map<AreaPtr,int>::iterator iter;
    for (iter = leavesSet.begin();iter != leavesSet.end(); iter++){
        if(iter!=leavesSet.begin())
            ou<<",";
        AreaPtr s = iter->first;
        float pointl = s->pointl->x;
        float pointr = s->pointr->x;
        LinePtr l = s->up;
        float y1 = (l->pointl->y - l->pointr->y)/(l->pointl->x - l->pointr->x)*(pointl - l->pointl->x)+l->pointl->y;
        float y2 = (l->pointl->y - l->pointr->y)/(l->pointl->x - l->pointr->x)*(pointr - l->pointl->x)+l->pointl->y;
        l = s->bottom;
        float y3 = (l->pointl->y - l->pointr->y)/(l->pointl->x - l->pointr->x)*(pointl - l->pointl->x)+l->pointl->y;
        float y4 = (l->pointl->y - l->pointr->y)/(l->pointl->x - l->pointr->x)*(pointr - l->pointl->x)+l->pointl->y;
        //cout<<pointl<<","<<y1<<","<<y3<<";\t"<<pointr<<","<<y2<<","<<y4<<endl;
        ou<<"\""<<iter->second<<"\""<<":["<<pointl<<","<<y1<<","<<y3<<","<<pointr<<","<<y2<<","<<y4<<"]";
    }
    ou<<"}";
    ou.close();
}

void insertLine(int lineid){
    Line l = lines[lineid];
    NodeList leftplocate = LookArea(l.pointl->x, l.pointl->y);
    AreaPtr A0_=nullptr, A1_=nullptr;
    AreaPtr A_ = leftplocate->s;
    bool intersected = A_->pointr->x < l.pointr->x;
    if(!intersected){
        A0 = (AreaPtr)malloc(sizeof(Area));//midUp
        A1 = (AreaPtr)malloc(sizeof(Area));//midDown
        A2 = (AreaPtr)malloc(sizeof(Area));//Right
        
        A0->pointl = l.pointl;  A0->pointr = l.pointr;
        A0->up = A_->up;        A0->bottom = &lines[lineid];
        A1->pointl = l.pointl;  A1->pointr = l.pointr;
        A1->up = &lines[lineid];A1->bottom = A_->bottom;
        A2->pointl = l.pointr;  A2->pointr = A_->pointr;
        A2->up = A_->up;        A2->bottom = A_->bottom;
        
        A0->rightUp = A2; A0->rightDown = A2; A1->rightUp = A2; A1->rightDown = A2;
        A2->rightUp = A_->rightUp;
        A2->rightDown = A_->rightDown;
        A_->rightUp = A0;
        A_->rightDown = A1;
        
        A_->pointr = l.pointl;
        
        N0 = (NodeList)malloc(sizeof(Node));
        N1 = (NodeList)malloc(sizeof(Node));
        N2 = (NodeList)malloc(sizeof(Node));
        N3 = (NodeList)malloc(sizeof(Node));
        N4 = (NodeList)malloc(sizeof(Node));
        N5 = (NodeList)malloc(sizeof(Node));
        /*
        ### leftplocate(pointl)
        #### /      \
        # (A_)N0   N1(pointr)
        ######## /    \
        ###### N2(l) N5(A2)
        ###### /    \
        #### N3(A0) N4(A1)
        */
        N5->classid = 2;
        N5->s = A2;
        N5->s->parent = N5;
        N4->classid = 2;
        N4->s = A1;
        N4->s->parent = N4;
        N3->classid = 2;
        N3->s = A0;
        N3->s->parent = N3;
        N2->classid = 1;
        N2->l = &lines[lineid];
        N2->leftchild = N3;
        N2->rightchild = N4;
        N1->classid = 0;
        N1->p = l.pointr;
        N1->leftchild = N2;
        N1->rightchild = N5;
        N0->classid = 2;
        N0->s = A_;
        N0->s->parent = N0;
        leftplocate->classid = 0;
        leftplocate->p = l.pointl;
        leftplocate->leftchild = N0;
        leftplocate->rightchild = N1;
    }else{
        bool intersectorpos = isAbove(&l, A_->pointr->x, A_->pointr->y);
        NodeList neighbor = intersectorpos ? A_->rightDown->parent : A_->rightUp->parent;

        A0 = (AreaPtr)malloc(sizeof(Area));//midTop
        A1 = (AreaPtr)malloc(sizeof(Area));//midBottom
        
        A0->pointl = l.pointl;  A1->pointl = l.pointl;
        A0->pointr = A_->pointr;A1->pointr = A_->pointr;
        A0->up = A_->up;        A1->bottom = A_->bottom;
        A0->bottom = &lines[lineid];    A1->up = &lines[lineid];
        A0->rightUp = A_->rightUp;      A1->rightUp = A_->rightUp;
        A0->rightDown = A_->rightDown;  A1->rightDown = A_->rightDown;
        A_->pointr = l.pointl;
        A_->rightUp = A0;       A_->rightDown = A1;
        /*
        ###### leftplocate(l.pointl)
        #######/     \
        #### N0(A_)  N1(l)
        ########### /     \
        ########## N2(A0) N3(A1)
        */
        N0 = (NodeList)malloc(sizeof(Node));
        N1 = (NodeList)malloc(sizeof(Node));
        N2 = (NodeList)malloc(sizeof(Node));
        N3 = (NodeList)malloc(sizeof(Node));
        
        N3->classid = 2;
        N3->s = A1;
        N3->s->parent = N3;
        N2->classid = 2;
        N2->s = A0;
        N2->s->parent = N2;
        N1->classid = 1;
        N1->l = &lines[lineid];
        N1->leftchild = N2;
        N1->rightchild = N3;
        N0->classid = 2;
        N0->s = A_;
        N0->s->parent = N0;
        leftplocate->classid = 0;
        leftplocate->p = l.pointl;
        leftplocate->leftchild = N0;
        leftplocate->rightchild = N1;
        
        //move to next trape
        leftplocate = neighbor;
        A0_ = A0;
        A1_ = A1;
        A0 = leftplocate->s;
        intersected = A0->pointr->x < l.pointr->x;
        while(intersected){
            /*
             ###### leftplocate(l)
             ####### /    \
             ##### N2(A0) N3(A1)
             */
            if(intersectorpos){// resize N3(A1), N2->s = resized A0
                intersectorpos = isAbove(&l, A0->pointr->x, A0->pointr->y);
                neighbor = intersectorpos ? A0->rightDown->parent : A0->rightUp->parent;
                N2 = (NodeList)malloc(sizeof(Node));
                A0->bottom = &lines[lineid];
                N2->classid = 2;
                N2->s = A0;
                N2->s->parent = N2;
                N3->s->pointr = A0->pointr;
                N3->s->rightUp = A0->rightUp;
                N3->s->rightDown = A0->rightDown;
            }
            else{//resize N2, N3->s = resized A0
                intersectorpos = isAbove(&l, A0->pointr->x, A0->pointr->y);
                neighbor = intersectorpos ? A0->rightDown->parent : A0->rightUp->parent;
                N3 = (NodeList)malloc(sizeof(Node));
                A0->up = &lines[lineid];
                N3->classid = 2;
                N3->s = A0;
                N3->s->parent = N3;
                N2->s->pointr = A0->pointr;
                N2->s->rightUp = A0->rightUp;
                N2->s->rightDown = A0->rightDown;
            }
            leftplocate->classid = 1;
            leftplocate->l = &lines[lineid];
            leftplocate->leftchild = N2;
            leftplocate->rightchild = N3;
            //move to next trape
            leftplocate = neighbor;
            A0_ = A0;
            A1_ = A1;
            A0 = leftplocate->s;
            intersected = A0->pointr->x < l.pointr->x;
        }
        //Eastest trape
        /*
         ###### leftplocate(l.pointl)
         #######/     \
         #### N0(l)  N1(A_)
         ## /     \
         # N2(A0) N3(A1)
         */
        A_ = A0;
        N0 = (NodeList)malloc(sizeof(Node));
        N1 = (NodeList)malloc(sizeof(Node));
        if(intersectorpos){
            N2 = (NodeList)malloc(sizeof(Node));
            A0 = (AreaPtr)malloc(sizeof(Area));
            A0->up = A_->up;        A0->bottom = &lines[lineid];
            A0->pointl = A_->pointl;A0->pointr = l.pointr;
            A0->rightUp = A_;       A0->rightDown = A_;
            N2->classid = 2;
            N2->s = A0;
            N2->s->parent = N2;
            N3->s->pointr = A0->pointr;
            N3->s->rightUp = A_;    N3->s->rightDown = A_;
            A0_->rightUp = A0;
            A0_->rightDown = A0;
        }
        else{
            N3 = (NodeList)malloc(sizeof(Node));
            A1 = (AreaPtr)malloc(sizeof(Area));
            A1->up = &lines[lineid];A1->bottom = A_->bottom;
            A1->pointl = A_->pointl;A1->pointr = l.pointr;
            A1->rightUp = A_;       A1->rightDown = A_;
            N3->classid = 2;
            N3->s = A1;
            N3->s->parent = N3;
            N2->s->pointr = A1->pointr;
            N2->s->rightUp = A_;    N2->s->rightDown = A_;
            A1_->rightUp = A1;
            A1_->rightDown = A1;
        }
        N0->classid = 1;
        N0->l = &lines[lineid];
        N0->leftchild = N2;
        N0->rightchild = N3;
        N1->classid = 2;
        N1->s = A_;
        N1->s->pointl = l.pointr;
        N1->s->parent = N1;
        leftplocate->classid = 0;
        leftplocate->p = l.pointr;
        leftplocate->leftchild = N0;
        leftplocate->rightchild = N1;
    }
}
