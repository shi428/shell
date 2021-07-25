#include "exec.h"

void exec(struct passwd *p, Node *node) {
    vector <pid_t> children;
    exec_node(p, children, node, STDIN_FILENO, STDOUT_FILENO);
    //cout << children.size() << endl;
    waitpid(children[children.size() - 1], NULL, 0);
    //for (auto i: children) {
        //cout << i << endl;
    //    waitpid(i, NULL, 0);
   // }
}

int exec_node(struct passwd *p, vector <pid_t> &children, Node *node, int readfd, int writefd) {
    if (node->token.type == COMMAND) {
       // cerr << "CMD" << endl;
        return ((Command *)node->obj)->execute(p, children, readfd, writefd);
    }
    if (node->token.type == PIPE) {
        //cerr << "PIPE" << endl;
        //return ((Command *)node->obj)->execute(p, readfd, writefd);
        return exec_pipe(p, children, node, readfd, writefd);
    }
    return 0;
}

int exec_pipe(struct passwd *p, vector <pid_t> &children, Node *node, int readfd, int writefd) {
    int fds[2];
    pipe(fds);
    int left = exec_node(p, children, node->left, readfd, fds[1]);
    close(fds[1]);
    int right = exec_node(p, children, node->right, fds[0], writefd);
    close(fds[0]);
    return left + right;
}
