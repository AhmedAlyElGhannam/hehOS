#include "shelly.h"
#include "../../stdlib/src/stdio.h"
#include "../../stdlib/src/stdlib.h"
#include "../../stdlib/src/hehos.h"
int main(int argc, char** argv)
{
    printf("hehOS v0.6.9\n");
    while(1) 
    {
        printf("0:\\ \n");
        printf("> ");
        char buf[1024];
        hehos_terminal_readline(buf, sizeof(buf), true);
        print("\n");
        hehos_system_run(buf);
        print("\n");
    }
    hehos_exit();
    return 0;
}