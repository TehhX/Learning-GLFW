/* EMBED_GLSL
   Reads files written in GLSL and ports them to .h and .c files. */

#include "stdio.h"
#include "stdlib.h"

int glsl_sources_count = 0;
FILE *c_header, *c_source, **glsl_sources = NULL;

static inline void err(const char *msg)
{
    puts(msg);
    exit(1);
}

int main(int argc, char **argv)
{
    --argc; ++argv;

    if (argc < 3)
    {
        err("Need arguments.");
    }

// Open files:
    if (!(c_header = fopen(argv[0], "wb")))
    {
        err("Bad c_header open.");
    }

    if (!(c_source = fopen(argv[1], "wb")))
    {
        err("Bad c_source open.");
    }

    for (int i = 0; i < argc - 2; ++i)
    {
        glsl_sources = realloc(glsl_sources, sizeof(*glsl_sources) * (i + 1));

        if (!(glsl_sources[i] = fopen(argv[i + 2], "rb")))
        {
            err("Bad glsl_sources open.");
        }

        char *period = argv[i + 2];
        while (*period != '.')
        {
            ++period;
        }
        *period = 0;
    }

// Processing:
    fprintf(c_header, "// GENERATED WITH EMBED_GLSL. DON'T EDIT MANUALLY.\n\nextern const char *%s;\n", argv[2]);
    fprintf(c_source, "// GENERATED WITH EMBED_GLSL. DON'T EDIT MANUALLY.\n\nconst char *%s = \"", argv[2]);

    int next;
    while ((next = fgetc(glsl_sources[0])) != EOF)
    {
        switch (next)
        {
        case '\n':
            fputs("\\n", c_source);
            break;

        default:
            fputc(next, c_source);
            break;
        }
    }
    fputs("\";\n", c_source);

// Close files:
    if (fclose(c_header))
    {
        err("Bad close c_header.");
    }

    if (fclose(c_source))
    {
        err("Bad close c_source.");
    }

    for (int i = 0; i < argc - 2; ++i)
    {
        if (fclose(glsl_sources[i]))
        {
            err("Bad close_glsl");
        }
    }

    free(glsl_sources);

    return 0;
}
