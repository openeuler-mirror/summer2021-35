#include<stdio.h>
#include<stdlib.h>
int main(void) {
	for(int i=0; i<10000;i++){
		free(i);
	}
}
