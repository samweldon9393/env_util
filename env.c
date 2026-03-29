#include "env.h"

#define MAP_SIZE 100

/* Static helper functions */

static void freelist(node *cur) {
    if (cur == NULL)
        return;
    node *next = cur->next;
    free(cur->key);
    free(cur);
    freelist(next);
}


static node *alloc_node(char *key, char *value) {
    node *n = malloc(sizeof(node));
    n->key = key;
    n->value = value;
    n->next = NULL;
    return n;
}
static char **copy_env(char **env) {
    int len = 0;
    for (char **e = env; *e; e++)
        len++;

    char **env_copy = malloc(sizeof(char *) * (len + 1));
    for (int i = 0 ; i < len ; i++)
        env_copy[i] = strdup(env[i]);
    env_copy[len] = NULL;
    return env_copy;
}

static int hash(const char *key) {
    int hashval = 0;
    int len = strlen(key);
    for (int i = 0 ; i < len ; i++)
        hashval = 37 * hashval + key[i];

    hashval %= MAP_SIZE;
    if (hashval < 0)
        hashval += MAP_SIZE;

    return hashval;
}

static void initmap(hashmap *hm) {
    memset(&hm->table, 0, sizeof(node *) * MAP_SIZE);
    pthread_mutex_init(&hm->mutex, (void *)0);
}

/* API functions */

void hm_put(hashmap *hm, char *key, char *value) {
    pthread_mutex_lock(&hm->mutex);
    int idx = hash(key);

    node *cur, *prev = NULL;
    for (cur = hm->table[idx];
            cur && strcmp(cur->key, key) != 0;
            cur = cur->next) {
        prev = cur;
    }

    if (prev == NULL) {
        hm->table[idx] = alloc_node(key, value);
    }
    else if (cur && strcmp(cur->key, key) == 0) {
        cur->value = value;
    }
    else {
        prev->next = alloc_node(key, value);
    }
    pthread_mutex_unlock(&hm->mutex);
}

const char *hm_get(hashmap *hm, const char *key) {
    pthread_mutex_lock(&hm->mutex);
    int idx = hash(key);

    node *cur;
    for (cur = hm->table[idx];
            cur && strcmp(cur->key, key) != 0;
            cur = cur->next);

    const char *ret;
    if (cur && strcmp(cur->key, key) == 0) {
        ret = cur->value;
    }
    else {
        ret = NULL;
    }
    pthread_mutex_unlock(&hm->mutex);
    return ret;
}

hashmap *hm_create(char **env) {

    char **env_copy = copy_env(env);
    hashmap *hm     = malloc(sizeof(hashmap));
    initmap(hm);

    for (char **e = env_copy; *e; e++) {
        char *key = strtok(*e, "=");
        char *value = strtok(NULL, "=");
        if (key)
            hm_put(hm, key, value);
    }

    free(env_copy);
    return hm;
}

void hm_destroy(hashmap *hm) {
    pthread_mutex_lock(&hm->mutex);
    for (int i = 0 ; i < MAP_SIZE ; i++)
        freelist(hm->table[i]);
    pthread_mutex_unlock(&hm->mutex);
    free(hm);
}
