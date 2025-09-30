/*
EMBED GLSL
==========
Reads files written in GLSL and transpiles(?) them to .h and .c files to be included in a project. Read lines [25,28] or run without args for help.

TODO:
    * Remove "/*" style comments from output.
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

// NOTE: Make sure to reflect any changes to GENERATED_SHADERS_NAME and/or GENERATED_SHADERS_GUARD in all relevant files or unexpected errors may arise.

#ifndef GENERATED_SHADERS_NAME
#define GENERATED_SHADERS_NAME "gensh" // The file name of the output shader .c and .h files.
#endif

#ifndef GENERATED_SHADERS_GUARD
#define GENERATED_SHADERS_GUARD "__GENSH_H__" // The header guard definition.
#endif

#ifndef STR_TYPEDEF_TNAME
#define STR_TYPEDEF_TNAME "__genshstr" // The type name used in the header and source file.
#endif

#define BASE_NAME "/" GENERATED_SHADERS_NAME "."

int main(int argc, char **argv)
{
    // First argument not necessary.
    --argc; ++argv;

    if (argc < 3)
    {
        puts(
            " Arg Position |          Expectation             |    Example   \n"
            "----------------------------------------------------------------\n"
            "       0      |  Output header/source file path  |  Project/src \n"
            "    [1,inf)   |        Input source files        | my_thing.vert\n"
        );
        return 1;
    }

// Open files:
    FILE *f_header, *f_source;
    {
        const int path_len = strlen(argv[0]);
        const bool end_is_slash = argv[0][path_len - 1] == '/';

        char *path_manip = memcpy(malloc(sizeof(*path_manip) * (sizeof(BASE_NAME) + 1 + path_len - end_is_slash)), argv[0], path_len);

        memcpy(path_manip + path_len - end_is_slash, BASE_NAME "h", sizeof(BASE_NAME) + 1);
        f_header = fopen(path_manip, "w");

        path_manip[path_len + sizeof(BASE_NAME) - 1 - end_is_slash] = 'c';

        f_source = fopen(path_manip, "w");
        free(path_manip);

        if (!f_header || !f_source)
        {
            puts("Bad generated header/source open.");
            return 2;
        }
    }

    // No longer need the second argument:
    --argc; ++argv;

    FILE **glsl_sources = 0;
    for (int i = 0; i < argc; ++i)
    {
        glsl_sources = realloc(glsl_sources, sizeof(*glsl_sources) * i);

        if (!(glsl_sources[i] = fopen(argv[i], "r")))
        {
            puts("Bad glsl_sources open.");
            return 3;
        }

        int char_ind = strlen(argv[i]) - 1;
        // Replace period in file name with underscore for variable name:
        for (; argv[i][char_ind] != '.'; --char_ind);
        argv[i][char_ind] = '_';

        // Move start of string one past last path seperator (/) to get file name.
        while (char_ind >= 0)
        {
            --char_ind;

            if (argv[i][char_ind] == '/')
            {
                argv[i] += char_ind + 1;
                break;
            }
        }
    }

// Process input, generate files:
    fputs("#ifndef " GENERATED_SHADERS_GUARD "//GENERATED WITH EMBED_GLSL. DON'T EDIT MANUALLY\n#define " GENERATED_SHADERS_GUARD "\ntypedef const char*const " STR_TYPEDEF_TNAME ";extern " STR_TYPEDEF_TNAME " ", f_header);
    fputs("#include \"" GENERATED_SHADERS_NAME ".h\"//GENERATED WITH EMBED_GLSL. DON'T EDIT MANUALLY\n" STR_TYPEDEF_TNAME " ", f_source);

    // Create string variables:
    for (int i = 0; i < argc; ++i)
    {
        char delim = (i == argc - 1 ? ';' : ','); // Will be a comma between variables and a semicolon for the last variable.
        fprintf(f_header, "%s%c", argv[i], delim);
        fprintf(f_source, "%s = \"", argv[i]);

        // Read code into string while removing redundancies, comments etc:
        char last = 0, next;
        bool writing = true;
        while ((next = fgetc(glsl_sources[i])) != EOF)
        {
            if (next == '\n' && last != '\n')
            {
                if (writing)
                {
                    // If last was space, overwrite space with newline:
                    if (last == ' ')
                    {
                        fseek(f_source, -1, SEEK_CUR);
                    }

                    fputs("\\n", f_source);
                }
                else
                {
                    writing = true;
                }
            }
            else if (next != '\n')
            {
                // Comment syntax:
                if (next == '/' && last == '/')
                {
                    // Undo '/' write from last loop:
                    fseek(f_source, -1, SEEK_CUR);
                    writing = false;
                    continue;
                }

                if (next == '\t')
                {
                    next = ' ';
                }

                if (next == ' ' && (last == ' ' || last == '\n'))
                {
                    continue;
                }

                if (writing)
                {
                    fputc(next, f_source);
                }
            }

            last = next;
        }

        fprintf(f_source, "\"%c", delim);
    }

    fputs("\n", f_source);
    fputs("\n#endif\n", f_header);

// Close files:
    if (fclose(f_header) || fclose(f_source))
    {
        puts("Bad generated header/source close.");
        return 4;
    }

    for (int i = 0; i < argc; ++i)
    {
        if (fclose(glsl_sources[i]))
        {
            puts("Bad glsl_sources close.");
            return 5;
        }
    }
    free(glsl_sources);

    // Comment out to remove warning:
    puts("NOTE: DON'T FORGET TO INCLUDE \"" GENERATED_SHADERS_NAME ".h\" IN APPLICABLE SOURCE FILES!");

    return 0;
}
