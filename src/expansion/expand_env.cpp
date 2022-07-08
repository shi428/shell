#include <shell.h>

extern char **environ;
bool is_environ(const char *str) {
    int i = 0;
    while (environ[i]) {
        string env = environ[i];
        string token = env.substr(0, env.find('='));
        if (!strcmp(token.c_str(), str)) {
            return true;
        }
        i++;
    }
    return false;
}
string expand_env(string &env_var) {
    string ret;
    if (is_environ(env_var.c_str())) {
        ret = getenv(env_var.c_str());
    }
    return ret;
}
