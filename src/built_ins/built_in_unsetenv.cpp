#include <c_headers.h>

bool is_environ2(char *str) {
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

void run_builtin_command_unsetenv(char **cmd) {
    if (is_environ2(cmd[1])) {
        if (unsetenv(cmd[1]) == -1) {
            fprintf(stderr, "unsetenv: failed to unset environment variable: %s\n", cmd[1]);
        }
    }
    else {
            fprintf(stderr, "unsetenv: failed to unset environment variable: %s\n", cmd[1]);
    }
}
