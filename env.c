#include "env.h"

#define are_same_alloc(k, v) ((v - k) == (strlen(k) + 1))

/* Static helper functions */

static void freelist(node *cur) {
    if (cur == NULL)
        return;
    node *next = cur->next;
    if (cur->key) {
        if (!are_same_alloc(cur->key, cur->value))
            free(cur->value);
        free(cur->key);
    }
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
    hm->size = 0;
}

/* API functions */

void hm_put(hashmap *hm, char *key, char *value) {
    int idx;
    node *cur, *prev = NULL;
    pthread_mutex_lock(&hm->mutex);

    key = strdup(key);
    if (value == NULL) {
        key = strtok(key, "=");
        value = strtok(NULL, "=");
    }
    else
        value = strdup(value);
    idx = hash(key);

    for (cur = hm->table[idx];
            cur && strcmp(cur->key, key) != 0;
            cur = cur->next) {
        prev = cur;
    }

    /* New entry in new list, don't need to free anything */
    if (cur == NULL && prev == NULL) {
        hm->table[idx] = alloc_node(key, value);
    }
    /* New entry at end of a list, don't need to free anything */
    else if (!cur) {
        prev->next = alloc_node(key, value);
    }
    /* Replacing a value of an existing key, free the new key and old value */
    else {
        if (!are_same_alloc(key, value))
            free(key);
        if (!are_same_alloc(cur->key, cur->value))
            free(cur->value);
        cur->value = value;
    }

    hm->size++;
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

size_t hm_size(hashmap *hm) { return hm->size; }

hashmap *hm_create(char **env) {

    hashmap *hm = malloc(sizeof(hashmap));
    initmap(hm);

    for (char **e = env; *e; e++) {
        if (*e)
            hm_put(hm, *e, NULL);
    }

    return hm;
}

void hm_destroy(hashmap *hm) {
    pthread_mutex_lock(&hm->mutex);
    for (int i = 0 ; i < MAP_SIZE ; i++)
        freelist(hm->table[i]);
    pthread_mutex_unlock(&hm->mutex);
    free(hm);
}
