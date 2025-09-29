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
        const int path_len = strlen(argv[0]) + 1;
        const bool end_is_slash = argv[0][path_len - 1] == '/';

#define BASE_NAME "/generated_shaders."
#define FULL_PATH_LEN (sizeof(BASE_NAME) + 1)
        char *path_manip = (char *) memcpy(malloc(sizeof(char) * (FULL_PATH_LEN + path_len + !end_is_slash + 1)), argv[0], path_len);

        // Header file:
        const int begin_offset = path_len - end_is_slash - 1;
        memcpy(path_manip + begin_offset, BASE_NAME "h", FULL_PATH_LEN);
        f_header = fopen(path_manip, "w");

        // Source file:
        path_manip[FULL_PATH_LEN + 1] = 'c';
#undef FULL_PATH_LEN
#undef BASE_NAME
        f_source = fopen(path_manip, "w");
        free(path_manip);

        // Error checking:
        if (!f_header || !f_source)
        {
            puts("Bad generated header/source open.");
            return 2;
        }
    }

    FILE **glsl_sources = 0;
    const int glsl_sources_c = argc - 1;
    for (int i = 0; i < glsl_sources_c; ++i)
    {
        glsl_sources = realloc(glsl_sources, sizeof(*glsl_sources) * (i + 1));

        if (!(glsl_sources[i] = fopen(argv[i + 1], "r")))
        {
            puts("Bad glsl_sources open.");
            return 3;
        }

        int char_ind = strlen(argv[i + 1]) - 1;
        // Replace period in file name with underscore for variable name:
        for (; argv[i + 1][char_ind] != '.'; --char_ind);
        argv[i + 1][char_ind] = '_';

        // Move start of string one past last path seperator (/) to get file name.
        while (argv[char_ind])
        {
            --char_ind;

            if (argv[i + 1][char_ind] == '/')
            {
                argv[i + 1] += char_ind + 1;
                break;
            }
        }
    }

// Process input, generate files:
    fputs("#ifndef GENERATED_SHADERS_H\n#define GENERATED_SHADERS_H//GENERATED WITH EMBED_GLSL. DON'T EDIT MANUALLY\ntypedef const char*const __generated_shaders_string;extern __generated_shaders_string ", f_header);
    fputs("#include \"generated_shaders.h\"//GENERATED WITH EMBED_GLSL. DON'T EDIT MANUALLY\n__generated_shaders_string ", f_source);

    // Create string variables:
    for (int i = 0; i < glsl_sources_c; ++i)
    {
        char delim = (i == glsl_sources_c - 1 ? ';' : ','); // Will be a comma between variables and a semicolon for the last variable.
        fprintf(f_header, "%s%c", argv[i + 1], delim);
        fprintf(f_source, "%s = \"", argv[i + 1]);

        // Read code into string while removing redundancies, comments etc:
        char last = 0, next;
        bool writing = true;
        while ((next = fgetc(glsl_sources[i])) != EOF)
        {
            if (next == '\n')
            {
                if (last != '\n')
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
            }
            else
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

    for (int i = 0; i < glsl_sources_c; ++i)
    {
        if (fclose(glsl_sources[i]))
        {
            puts("Bad glsl_sources close.");
            return 5;
        }
    }
    free(glsl_sources);

    puts("NOTE: DON'T FORGET TO INCLUDE \"generated_shaders.h\" IN APPLICABLE SOURCE FILES!");

    return 0;
}
