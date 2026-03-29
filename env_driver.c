#include <stdio.h>
#include "env.h"

int main(int argc, char **argv, char **env) {
    hashmap *hm = get_env_hm(env);

    printf("Current shell is %s\n", hm_get(hm, "SHELL"));
    printf("Current user is %s\n", hm_get(hm, "LOGNAME"));

    destroymap(hm);
    return 0;
}
