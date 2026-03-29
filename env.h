#ifndef _ENV_H
#define _ENV_H

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAP_SIZE 100

typedef struct node {
    char          *key;
    char          *value;
    struct node   *next;
} node;

typedef struct hashmap {
    node                *table[MAP_SIZE];
    pthread_mutex_t     mutex;
} hashmap;

/*
 * Initialzie the hashmap
 */
void initmap(hashmap *hm);

/*
 * Free hashmap data
 */
void destroymap(hashmap *hm);

/*
 * A simple hash function
 */
int hash(const char *key);

/*
 * Add a key, value pair to map (or overwrite existing value of a key)
 */
void hm_put(hashmap *hm, char *key, char *value);

/*
 * Get the value for a given key. NULL if key not in map.
 */
const char *hm_get(hashmap *hm, const char *key);

/*
 * Create a hashmap from a program's environment pointer
 */
hashmap *get_env_hm(char **env);

#endif // _ENV_H
