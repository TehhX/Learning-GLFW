/*
EMBED GLSL
==========
Reads files written in GLSL and transpiles(?) them to .h and .c files to be included in a project. Read lines [32, 35] or run without args for help.
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#ifndef GENERATED_SHADER_FNAME
    #define GENERATED_SHADER_FNAME "gensh" // The file name of the output shader files. This will generate gensh.[hc].
#endif

#ifndef GENERATED_SHADER_GUARD
    #define GENERATED_SHADER_GUARD "GENSH_H" // The header guard definition.
#endif

#ifndef STR_TYPEDEF_TNAME
    #define STR_TYPEDEF_TNAME "__genshstr_t" // The type name used in the header and source file.
#endif

int main(int argc, char **argv)
{
    // First argument (exec line) not necessary.
    --argc; ++argv;

    if (argc == 0)
    {
        puts(
            "    Arg Position   |           Expectation            |    Example   \n"
            "---------------------------------------------------------------------\n"
            "          0        |   Output header file directory   |  Project/src \n"
            "  [1, INT_MAX - 1] |        Input source files        | my_thing.vert\n"
        );
        return 1;
    }

// Open header file:
    FILE *f_header, *f_source;
    {
        #define dir argv[0]
        const int dir_len = strlen(dir);
        const bool end_is_slash = (dir[dir_len - 1] == '/');

        #define BASE_NAME "/" GENERATED_SHADER_FNAME ".h"
        char *fullpath = memcpy(malloc(sizeof(char) * (sizeof(BASE_NAME) + dir_len - end_is_slash)), dir, dir_len);
        fullpath[sizeof(BASE_NAME) + dir_len - end_is_slash - 1] = 0;

        memcpy(fullpath + dir_len - end_is_slash, BASE_NAME, sizeof(BASE_NAME) - 1);
        f_header = fopen(fullpath, "w");

        fullpath[sizeof(BASE_NAME) + dir_len - 2 - end_is_slash] = 'c';
        f_source = fopen(fullpath, "w");
        free(fullpath);

        if (!f_header || !f_source)
        {
            puts("Bad generated header/source open.");
            return 2;
        }
        #undef BASE_NAME
        #undef dir
    }

    // No longer need the second (header dir) argument:
    --argc; ++argv;

    FILE **glsl_sources = 0;
    for (int i = 0; i < argc; ++i)
    {
        glsl_sources = realloc(glsl_sources, sizeof(*glsl_sources) * (i + 1));

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
    fputs("#ifndef " GENERATED_SHADER_GUARD "\n#define " GENERATED_SHADER_GUARD "\n//GENERATED WITH EMBED_GLSL. DON'T EDIT MANUALLY!\ntypedef const char*const " STR_TYPEDEF_TNAME ";extern " STR_TYPEDEF_TNAME " ", f_header);
    fputs("#include \"" GENERATED_SHADER_FNAME ".h\"\n//GENERATED WITH EMBED_GLSL. DON'T EDIT MANUALLY!\n" STR_TYPEDEF_TNAME " ", f_source);

    // Create string variables:
    for (int i = 0; i < argc; ++i)
    {
        char delim = (i == argc - 1 ? ';' : ','); // Will be a comma between variables and a semicolon for the last variable.
        fprintf(f_source, "%s=\"", argv[i]);
        fprintf(f_header, "%s%c", argv[i], delim);

        // Read code into string while removing redundancies, comments etc:
        char last = 0, next;
        #define        WRITING 0 // Currently writing text.
        #define SING_LINE_CMNT 1 // Currently eating a single-line comment.
        #define MULT_LINE_CMNT 2 // Currently eating a (potentially) multi-line comment.
        int status = WRITING;
        while ((next = fgetc(glsl_sources[i])) != EOF)
        {
            if (next == '\n' && last != '\n')
            {
                if (status == WRITING)
                {
                    // If last was space, overwrite space with newline:
                    if (last == ' ')
                    {
                        fseek(f_source, -1, SEEK_CUR);
                    }

                    fputs("\\n", f_source);
                }
                else if (status == SING_LINE_CMNT)
                {
                    status = WRITING;
                }
            }
            else if (next != '\n')
            {
                if (last == '/' && status == WRITING)
                {
                    if (next == '/')
                    {
                        fseek(f_source, -1, SEEK_CUR);
                        status = SING_LINE_CMNT;
                        continue;
                    }
                    else if (next == '*')
                    {
                        fseek(f_source, -1, SEEK_CUR);
                        status = MULT_LINE_CMNT;
                        continue;
                    }
                }

                if (last == '*' && next == '/' && status == MULT_LINE_CMNT)
                {
                    status = WRITING;
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

                if (status == WRITING)
                {
                    fputc(next, f_source);
                }
            }

            last = next;
        }
        #undef WRITING
        #undef SING_LINE_CMNT
        #undef MULT_LINE_CMNT

        fprintf(f_source, "\"%c", delim);
    }

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

    return 0;
}
