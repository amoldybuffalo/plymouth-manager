#include <string.h>
#include <stdio.h>
#include "map.h"

void map_init(map_element* hash_map, int size)
{	
	hash_map->add = &map_add;
	hash_map->modify = &map_modify;
	hash_map->find = &map_find;
	hash_map->exists = &map_exists;
	for(int i = 0; i < size; i++)
	{
		if(i == size - 1) hash_map[i].key[0] = '\n';
		else hash_map[i].key[0] = '\0';		
		hash_map[i].value = 0;
	}
}

int map_find(map_element* hash_map, char* key)
{
	if(key[0] == '\0') { fprintf(stderr, "Error: empty string is not a valid key.\n"); return -1; }
	if(key[0] == '\n') { fprintf(stderr, "Error: newline is not a valid key.\n"); return -1; }
	
	for(int i = 0;  !hash_map_end(hash_map, i); i++)
	{	
		if(strcmp(hash_map[i].key, key) == 0)
		{	
            return hash_map[i].value;
		}
	}
	return 0;
	
}

int map_add(map_element* hash_map, char* key, int value)
{
	if(key[0] == '\0') { fprintf(stderr, "Error: empty string is not a valid key.\n"); return -1; }
	if(key[0] == '\n') { fprintf(stderr, "Error: newline is not a valid key.\n"); return -1; }
	for(int i = 0; i < (int) strlen(key); i++)
	{
		if(key[i] == '\n') {fprintf(stderr, "Errror: no newlines in keys. \n"); return -1;}
	}
	for(int i = 0; !hash_map_end(hash_map, i); i++)
	{
		if(hash_map[i].key[0] == '\0')
		{
			strcpy(hash_map[i].key, key);
		 	hash_map[i].value = value;
			return 0;
		}
	}
	return -1;	
}

int map_modify(map_element* hash_map, char* key, int value)
{
	for(int i = 0; !hash_map_end(hash_map, i); i++)
	{
				if(strcmp(key, hash_map[i].key) == 0)
				{
				 	hash_map[i].value=value;
					return 0;
				}
	}
	return -1;		
}

int map_exists(map_element* hash_map, char* key)
{
	for(int i = 0; !hash_map_end(hash_map, i); i++)
	{
			if(strcmp(key, hash_map[i].key) == 0)
			{
				return 1;
			}
	}
	return 0;
	
}