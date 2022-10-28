#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void checkStrace(int argc, char *argv[])
{
    if (argc < 3)
    {
        // number of arguments is less than 3
        printf("Error in strace: arguments missing\n");
        exit(1);
    }
    if (argv[1][0] < '0' || argv[1][0] > '9')
    {
        printf("Error: %s mask command\n", argv[0]);
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    char *cmdArgs[MAXARG];

    checkStrace(argc, argv);

    if (trace(atoi(argv[1])) < 0)
    {
        fprintf(2, "%s: trace failed\n", argv[0]);
        exit(1);
    }

    for (int i = 2; i < argc && i < MAXARG; i++)
    {
        cmdArgs[i - 2] = argv[i];
    }

    exec(cmdArgs[0], cmdArgs);
    exit(0);
}