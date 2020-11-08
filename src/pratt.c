#include <stdio.h>
#include <stdlib.h>

#include "eval.h"

int main(void)
{
    char *line = NULL;
    size_t size = 0;
    ssize_t ret = 0;

    while ((getline(&line, &size, stdin)) > 0)
    {
        int res;
        if (!eval_string(line, &res))
        {
            fputs("Could not parse input\n", stderr);
            ret = 1;
            continue;
        }

        printf("%d\n", res);
    }

    free(line);

    return ret;
}
