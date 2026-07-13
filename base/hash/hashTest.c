#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TABLE_SIZE 10007

typedef struct Entry {
	char key[64];	// 放名字（键）
	int value;	// 放fd（值）
	struct Entry *next;	// 指向下一个盒子（拉链）
} Entry;


