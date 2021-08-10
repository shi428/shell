#include <exec.h>

extern int return_code;
extern string last_arg;
int exec(struct passwd *p, Node *node, vector <pair<pid_t, vector <string>>> &bPids, vector <int> &pos) {
    vector <pid_t> children;
    vector <string> cmds;
    int ret = exec_node(p, children, node, NULL, STDIN_FILENO, STDOUT_FILENO, cmds);
    //cout << children.size() << endl;
    if (children.size() && !node->background) {
        int status;
        waitpid(children[children.size() - 1], &status, 0);
        return_code = WEXITSTATUS(status);
        last_arg = cmds.back();
    }
    if (node->background) {
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

int exec_node(struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds) {
    if (node->token.type == COMMAND) {
        // cerr << "CMD" << endl;
        return ((Command *)node->obj)->execute(p, children, pipefds, readfd, writefd, cmds);
    }
    if (node->token.type == PIPE) {
        //cerr << "PIPE" << endl;
        //return ((Command *)node->obj)->execute(p, readfd, writefd);
        return exec_pipe(p, children, node, pipefds, readfd, writefd, cmds);
    }
    return 0;
}

int exec_pipe(struct passwd *p, vector <pid_t> &children, Node *node, int *pipefds, int readfd, int writefd, vector <string> &cmds) {
    int fds[2];
    pipe(fds);
    int fdin = dup(STDIN_FILENO);
    int fdout = dup(STDOUT_FILENO);
    int fderr = dup(STDERR_FILENO);
    int left = exec_node(p, children, node->left, fds, readfd, fds[1], cmds);
    dup2(fdin, STDIN_FILENO);
    dup2(fdout, STDOUT_FILENO);
    dup2(fderr, STDERR_FILENO);
    cmds.push_back(string("|"));
    close(fds[1]);
    int right = exec_node(p, children, node->right, pipefds, fds[0], writefd, cmds);
    dup2(fdin, STDIN_FILENO);
    dup2(fdout, STDOUT_FILENO);
    dup2(fderr, STDERR_FILENO);
    close(fds[0]);
    return left + right;
}

