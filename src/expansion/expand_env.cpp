#include <c_headers.h>

extern char **environ;
bool is_environ(char *str) {
    int i = 0;
    while (environ[i]) {
        char *env = environ[i];
        char *substr = strdup(env);
        int terminator = strchr(substr, '=') - substr;
        memcpy(substr, env, terminator);
        substr[terminator] = '\0';
        //string token = env.substr(0, env.find('='));
        if (!strcmp(substr, str)) {
            free(substr);
            return true;
        }
            free(substr);
        i++;
    }
    return false;
}

char *expand_env(char *envVar) {
    char *ret = NULL;
    if (is_environ(envVar)){
        ret = getenv(envVar);
    }
    else {
        return strdup("");
    }
    return ret;
}
