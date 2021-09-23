#include<stdio.h>
#include<stdlib.h>
int main(){
        int *p = (int *)malloc(sizeof(3000));
        while(1){
                free(p);
        }
}
