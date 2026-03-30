# env-hashmap

A lightweight C library that loads a process's environment variables into a thread-safe hash table for fast O(1) key lookups.

## How it works

When a C program receives its environment as a `char **env` array, each entry is a single string in the form `KEY=VALUE`. This library copies those strings, splits them on `=` using `strtok`, and inserts the resulting key/value pairs into a chained hash table. The table has a fixed size (default 100 buckets, configurable via macro in env.h), and uses a polynomial rolling hash (source [Mark Allen Weiss](https://ebooks.karbust.me/Technology/Data.Structures.and.Algorithm.Analysis.in.Java.3rd.Edition.Nov.2011.pdf)). The API does expose a put operation to add to the hash table, though it never rehashes as I expect that to be a rare event (the purpose of this utility is to access pre-existing environment variables more easily). All operations are protected by a `pthread` mutex, making the map safe to read from multiple threads.

## API

```c
// Load the process environment into a new heap-allocated hashmap
hashmap *hm_create(char **env);

// Look up a variable — returns NULL if not found
const char *hm_get(hashmap *hm, const char *key);

// Get the current size of the hashmap
size_t hm_size(hashmap *hm);

// Insert or overwrite a key/value pair
void hm_put(hashmap *hm, char *key, char *value);

// Free all memory and destroy the mutex
void hm_destroy(hashmap *hm);
```

## Building

```bash
make          # builds env.a (static library) and example (demo binary)
make clean    # remove build artifacts
```

Requires GCC and pthreads (standard on Linux/macOS).

## Usage example

```c
#include "env.h"

int main(int argc, char **argv, char **env) {
    hashmap *hm = get_env_hm(env);

    printf("Shell: %s\n", hm_get(hm, "SHELL"));
    printf("User:  %s\n", hm_get(hm, "LOGNAME"));

    destroymap(hm);
    return 0;
}
```

Link against the static library when compiling your own program:

```bash
gcc -o myprogram myprogram.c env.a -lpthread
```

## Files

| File | Description |
|------|-------------|
| `env.h` | Public header — types and function declarations |
| `env.c` | Hash table implementation |
| `example.c` | Demo program |
| `Makefile` | Build rules |
