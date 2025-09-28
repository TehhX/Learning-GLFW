/* EMBED_GLSL
   reads files written in GLSL and ports them to .h and .c files. */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

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
        err(
            "EMBED_GLSL Help:\n"
            "  argv[0]: Output header file path and name e.g my_header.h\n"
            "  argv[1]: Output source file path and name e.g my_source.c\n"
            "  argv[2..]: Input source files e.g my_thing.glsl other_thing.glsl"
        );
    }

// Open files:
    if (!(c_header = fopen(argv[0], "w")))
    {
        err("Bad c_header open.");
    }

    if (!(c_source = fopen(argv[1], "w")))
    {
        err("Bad c_source open.");
    }

    const int glsl_sources_c = argc - 2;
    for (int i = 0; i < glsl_sources_c; ++i)
    {
        glsl_sources = realloc(glsl_sources, sizeof(*glsl_sources) * (i + 1));

        if (!(glsl_sources[i] = fopen(argv[i + 2], "r")))
        {
            err("Bad glsl_sources open.");
        }

        // Replace period in file name with underscore for variable name:
        char *traveller = argv[i + 2] + strlen(argv[i + 2]) - 1;
        while (*traveller != '.')
        {
            --traveller;
        }
        *traveller = '_';

        // Move start of string one past last path seperator (/) to get file name.
        while (traveller != argv[i + 2])
        {
            --traveller;

            if (*traveller == '/')
            {
                argv[i + 2] = traveller + 1;
                break;
            }
        }
    }

// Processing:
    // TODO: Implement header guard in generated header file.
    fputs("// GENERATED WITH EMBED_GLSL. DON'T EDIT MANUALLY.\n", c_header);
    fputs("// GENERATED WITH EMBED_GLSL. DON'T EDIT MANUALLY.\n", c_source);

    for (int i = 0, next; i < glsl_sources_c; ++i)
    {
        fprintf(c_header, "\nextern const char * const %s;", argv[i + 2]);
        fprintf(c_source, "\nconst char * const %s = \"", argv[i + 2]);

        while ((next = fgetc(glsl_sources[i])) != EOF)
        {
            if (next == '\n')
            {
                fputs("\\n", c_source);
            }
            else
            {
                fputc(next, c_source);
            }
        }

        fputs("\";", c_source);
    }

    fputc('\n', c_source);
    fputc('\n', c_header);

// Close files:
    if (fclose(c_header))
    {
        err("Bad close c_header.");
    }

    if (fclose(c_source))
    {
        err("Bad close c_source.");
    }

    for (int i = 0; i < glsl_sources_c; ++i)
    {
        if (fclose(glsl_sources[i]))
        {
            err("Bad close_glsl");
        }
    }

    free(glsl_sources);

    return 0;
}
