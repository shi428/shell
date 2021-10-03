#include <shell.h>
struct termios tty_attr;
void set_tty_raw_mode() {
    struct termios newtty_attr;
    tcgetattr(0,&shell_tmodes);
    tcgetattr(0,&newtty_attr);
    newtty_attr.c_lflag &= (~(ICANON|ECHO));
    newtty_attr.c_cc[VTIME] = 0;
    newtty_attr.c_cc[VMIN] = 1;

    tcsetattr(0,TCSANOW,&newtty_attr);
}

void tty_reset() {
    /* flush and reset */
    tcsetattr(0,TCSAFLUSH,&shell_tmodes);
}
