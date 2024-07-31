#include "../stdlib/src/hehos.h"
#include "../stdlib/src/stdlib.h"
#include "../stdlib/src/stdio.h"
#include "../stdlib/src/string.h"
#include <stdbool.h>

int main(int argc, char** argv)
{
    // skip command name
    for (int i = 1; i < argc; i++)
    {
        printf("%s ", argv[i]);
    }


    hehos_exit();
    while(1);

    return 0;
}