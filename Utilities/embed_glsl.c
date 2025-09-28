/* EMBED GLSL
Reads files written in GLSL and transpiles(?) them to .h and .c files to be included in a project. Read lines [16,19] or run without args for help.

TODO LIST:
    * Implement header guard in generated header file
    * Unnecessary duplicate spaces increasing file size without reason should be removed
    * Tab characters should be replaced by single spaces
    * Remove comments etc. from output
    * Consider reducing all things into a single line after the generation warning comment and header guard
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define err(MSG) { puts(MSG); exit(1); }

int main(int argc, char **argv)
{
    --argc; ++argv;

    if (argc < 3)
    {
        err(
            " Arg Position |          Expectation             |    Example   \n"
            "----------------------------------------------------------------\n"
            "       0      | Output header file path and name |  my_header.h \n"
            "       1      | Output source file path and name |  my_source.c \n"
            "    [2,inf)   |        Input source files        | my_thing.vert\n"
        );
    }

// Open files:
    FILE *c_header;
    if (!(c_header = fopen(argv[0], "w")))
    {
        err("Bad c_header open.");
    }

    FILE *c_source;
    if (!(c_source = fopen(argv[1], "w")))
    {
        err("Bad c_source open.");
    }

    FILE **glsl_sources = NULL;
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
    fputs("// GENERATED WITH EMBED_GLSL. DON'T EDIT MANUALLY.\n\n", c_header);
    fputs("// GENERATED WITH EMBED_GLSL. DON'T EDIT MANUALLY.\n\n", c_source);

    for (int i = 0, next; i < glsl_sources_c; ++i)
    {
        fprintf(c_header, "extern const char * const %s;\n", argv[i + 2]);
        fprintf(c_source, "const char * const %s = \"", argv[i + 2]);

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

        fputs("\";\n", c_source);
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
