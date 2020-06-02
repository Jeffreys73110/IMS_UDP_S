#include <string.h>
#include<stdio.h>

typedef unsigned char	uint8_t; 

uint8_t c2u(char s){
	if(s>96) return s-87;
	if(s>64) return s-55;
	if(s>47) return s-48;
}