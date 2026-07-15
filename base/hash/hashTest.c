#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TABLE_SIZE 10007

// 链表节点
typedef struct Entry {
	char key[64];	// 放名字（键）
	int value;	// 放fd（值）
	struct Entry *next;	// 指向下一个盒子（拉链）
} Entry;

// 哈希表
typedef struct {
	Entry **buckets;	// 指针数组，每个元素指向一个链表的头
	int size;			// 总数
} HashMap;


// 哈希算法
unsigned int hash(const char *str, int table_size) {
	unsigned int hash_value = 5381;
	int c;
	while ((c= *str++)) {
		hash_value = ((hash_value << 5) + hash_value) + c;
	}
	return hash_value % table_size;
}

// 初始化一个哈希表
HashMap* create_hashmap() {
	HashMap *map = (HashMap*)malloc(sizeof(HashMap));
	if(!map) return NULL;

	map->size = TABLE_SIZE;
	// calloc 会自动把每个指针初始化为NULL
	map->buckets = (Entry**)calloc(map->size, sizeof(Entry*));
	if (!map->buckets) {
		free(map);
		return NULL;
	}
	return map;
}

// 释放整个哈希表（防止内存泄漏）
void free_hashmap(HashMap *map) {
	if (!map) return;

	for (int i = 0; i < map->size; i++) {
		Entry *curr = map->buckets[i];
		while (curr) {
			Entry *tmp = curr;
			curr = curr->next;
			free(tmp);	// 释放每个节点
		}
	}
	free(map->buckets);	// 释放数组
	free(map);			// 释放结构体
}

// 插入或更新键值对
void put(HashMap *map, const char *key, int value) {
	if(!map || !key) return;

	int index = hash(key, map->size);
	Entry *curr = map->buckets[index];

	// 先查找是否已经存在这个键
	while (curr) {
		if (strcmp(curr->key, key) == 0) {
			curr->value = value;	// 更新值
			return;
		}
		curr = curr->next;
	}

	// 没找到，创建新节点，头插法
	Entry *new_entry = (Entry*)malloc(sizeof(Entry));
	if (!new_entry) return;

	strncpy(new_entry->key, key, sizeof(new_entry->key) - 1);
	new_entry->key[sizeof(new_entry->key) - 1] = '\0';
	new_entry->value = value;
	new_entry->next =  map->buckets[index];
	map->buckets[index] = new_entry;
}

// 根据key查找value，找不到返回-1
int get(HashMap *map, const char *key) {
	if (!map || !key) return -1;

	int index = hash(key, map->size);
	Entry *curr = map->buckets[index];

	while (curr) {
		if (strcmp(curr->key, key) == 0) {
			return curr->value;
		}
		curr = curr->next;
	}
	return -1;	// 没找到
}

// 删除键值对，成功返回 1， 失败返回 0
int remove_key(HashMap *map, const char *key) {
	if (!map || !key) return 0;

	int index = hash(key, map->size);
	Entry *curr = map->buckets[index];
	Entry *prev = NULL;

	while (curr) {
		if (strcmp(curr->key, key) == 0) { 
			if (prev) {
				prev->next = curr->next;
			} else {
				map->buckets[index] = curr->next;
			}
			free(curr);
			return 1;
		}
		prev = curr;
		curr = curr->next;
	}
	return 0;
}

int main() {
	HashMap *testMap = create_hashmap();
	if (!testMap) {
		printf("创建哈希表失败！\n");
		return -1;
	}

	put(testMap, "wanglaoji", 10);
	put(testMap, "jiaduobao", 13);

	printf("wanglaoji:%d\n", get(testMap, "wanglaoji"));
	printf("jiaduobao:%d\n", get(testMap, "jiaduobao"));

	put(testMap, "wanglaoji", 14);
	printf("更新后的值:\nwanglaoji:%d\n", get(testMap, "wanglaoji"));

	remove_key(testMap, "wanglaoji");
	printf("删除后的wanglaoji:%d\n", get(testMap, "wanglaoji"));

	free_hashmap(testMap);
	return 0;
}
