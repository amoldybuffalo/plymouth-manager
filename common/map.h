typedef struct map_element
{
	char key[500];
	int value;
	int (*add)(struct map_element*, char*, int);
	int (*modify)(struct map_element*, char*, int);
	int (*exists)(struct map_element*, char*);
	int (*find)(struct map_element*, char*);
} map_element;


#define hash_map_end(hash_map, i) (hash_map[ i ].key[0] == '\n')

void map_init(map_element* hash_map, int size);
int map_find(map_element* hash_map, char* key);
int map_add(map_element* hash_map, char* key, int value);
int map_modify(map_element* hash_map, char* key, int value);
int map_exists(map_element* hash_map, char* key);