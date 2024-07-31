#include "../stdlib/src/hehos.h"
#include "../stdlib/src/stdlib.h"
#include "../stdlib/src/stdio.h"
#include "../stdlib/src/string.h"
#include <stdbool.h>

int main(int argc, char** argv)
{
    void* ptr = malloc(512);
    free(ptr);

    char buf[1024];
    //hehos_terminal_readline(buf, sizeof(buf), true);

    print(buf);

    printf("Baka %i UwU\n", 96);

    putchar('Z');

    hehos_exit();
    while(1);

    return 0;
}