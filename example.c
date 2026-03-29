#include <stdio.h>
#include "env.h"

int main(int argc, char **argv, char **env) {
    hashmap *hm = hm_create(env);

    printf("Current shell is %s\n", hm_get(hm, "SHELL"));
    printf("Current user is %s\n", hm_get(hm, "LOGNAME"));

    hm_destroy(hm);
    return 0;
}
