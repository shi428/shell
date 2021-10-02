#include <exec.h>

extern int return_code;
extern string last_arg;
int exec(Tree *tr, struct passwd *p, Node *node, vector <pair<pid_t, vector <string>>> &bPids, vector <int> &pos) {
    vector <pid_t> children;
    vector <string> cmds;
    int ret = exec_node(tr, p, children, node, NULL, STDIN_FILENO, STDOUT_FILENO, cmds);
    //cout << children.size() << endl;
    if (children.size() && !node->background) {
        int status;
        waitpid(children[children.size() - 1], &status, 0);
        return_code = WEXITSTATUS(status);
        last_arg = cmds.back();
    }
    if (node->background && !ret) {
        bPids.push_back(pair<pid_t, vector <string>>(children[children.size() - 1], cmds));
        cout << bPids.size() <<  ". ["  << children[children.size() - 1] << "]" << endl;
        pos.push_back(bPids.size());
    }
    //for (auto i: children) {
    //cout << i << endl;
    //    waitpid(i, NULL, 0);
    // }
    return ret;
}

int exec_node(Tree *tr, struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds) {
    int fdin = dup(STDIN_FILENO);
    int fdout = dup(STDOUT_FILENO);
    int fderr = dup(STDERR_FILENO);
    int ret = 0;
    if (node->type == COMMAND_NODE) {
        // cerr << "CMD" << endl;
        ret = ((Command *)node->obj)->execute(tr, p, children, pipefds, readfd, writefd, cmds);
    }
    if (node->type == PIPE_NODE) {
        //cerr << "PIPE" << endl;
        //return ((Command *)node->obj)->execute(p, readfd, writefd);
        ret =  exec_pipe(tr, p, children, node, pipefds, readfd, writefd, cmds);
    }
    if (node->type == AND_NODE) {
        ret =  exec_and(tr, p, children, node, pipefds, readfd, writefd, cmds);
    }
    if (node->type == OR_NODE) {
        ret =  exec_or(tr, p, children, node, pipefds, readfd, writefd, cmds);
    }
    dup2(fdin, STDIN_FILENO);
    dup2(fdout, STDOUT_FILENO);
    dup2(fderr, STDERR_FILENO);
    return ret;
}

int exec_pipe(Tree *tr, struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds) {
    int fds[2];
    pipe(fds);
    int left = exec_node(tr, p, children, node->left, fds, readfd, fds[1], cmds);
    cmds.push_back(string("|"));
    close(fds[1]);
    int right = exec_node(tr, p, children, node->right, pipefds, fds[0], writefd, cmds);
    close(fds[0]);
    return left + right;
}


int exec_and(Tree *tr, struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds) {
    exec_node(tr, p, children, node->left, pipefds, readfd, writefd, cmds);
        int status;
        if (children.size())
        waitpid(children[children.size() - 1], &status, 0);
        if (!WEXITSTATUS(status)) {
    cmds.push_back(string("&&"));
    exec_node(tr, p, children, node->right, pipefds, readfd, writefd, cmds);
    return 0;
        }
    return -1;;
}
int exec_or(Tree *tr, struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds) {
    exec_node(tr, p, children, node->left, pipefds, readfd, writefd, cmds);
        int status;
        if (children.size())
        waitpid(children[children.size() - 1], &status, 0);
        if (WEXITSTATUS(status)) {
    cmds.push_back(string("&&"));
    exec_node(tr, p, children, node->right, pipefds, readfd, writefd, cmds);
    return 0;
        }
    return -1;;
}
