#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <ctime>
#include "trapezoidalmap.h"
using namespace std;
extern NodeList root;
int main() {

    float f2[200];
    FILE  *fq;
    int cnt = 0;

    fq=fopen("Trapezoid.txt" ,"rt+");
    fscanf(fq,"%d:",&cnt);

    for(int i=0;i<cnt;i++){
        fscanf(fq,"%f,%f,%f,%f,",&f2[i*4], &f2[i*4+1], &f2[i*4+2], &f2[i*4+3]);
    }
    fclose(fq);

    clock_t start,finish;
    
    initTrape(cnt*2,f2);

    start=clock();
    for(int i=0;i<cnt;i++)
        insertLine(i);
    finish=clock();

    cout<<(finish-start)<<"ns"<<endl;
    printTrape();
    return 0;
}
