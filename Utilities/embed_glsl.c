/*
EMBED GLSL
==========
Reads files written in GLSL and transpiles(?) them to .h and .c files to be included in a project. Read lines [25,29] or run without args for help.
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#define err(MSG) { (MSG); exit(1); }

int main(int argc, char **argv)
{
    // First argument not necessary.
    --argc; ++argv;

    if (argc < 3)
    {
        err(
            " Arg Position |          Expectation             |    Example   \n"
            "----------------------------------------------------------------\n"
            "       0      |  Output header/source file path  |  Project/src \n"
            "    [1,inf)   |        Input source files        | my_thing.vert\n"
        );
    }

// Open files:
    // Check if path ends in a slash:
    int path_index = 0;
    while (argv[0][path_index++]);
    bool has_slash = argv[0][path_index - 1] == '/';
#define path_len ((const int) path_index)

#define HEADER_NAME "/generated_shaders.h"
    FILE *f_header;
    {
        char *header_fpath = (char *) memcpy(malloc(sizeof(char) * (sizeof(HEADER_NAME) + path_len + !has_slash + 1)), argv[0], path_len);
        memcpy(header_fpath + path_len - has_slash - 1, HEADER_NAME, sizeof(HEADER_NAME));
        if (!(f_header = fopen(header_fpath, "w")))
        {
            err("Bad f_header open.");
        }
        free(header_fpath);
    }
#undef HEADER_NAME

#define SOURCE_NAME "/generated_shaders.c"
    FILE *f_source;
    {
        char *source_fpath = (char *) memcpy(malloc(sizeof(char) * (sizeof(SOURCE_NAME) + path_len + !has_slash + 1)), argv[0], path_len);
        memcpy(source_fpath + path_len - has_slash - 1, SOURCE_NAME, sizeof(SOURCE_NAME));
        if (!(f_source = fopen(source_fpath, "w")))
        {
            err("Bad f_source open.");
        }
        free(source_fpath);
    }
#undef SOURCE_NAME

    FILE **glsl_sources = 0;
    const int glsl_sources_c = argc - 1;
    for (int i = 0; i < glsl_sources_c; ++i)
    {
        glsl_sources = realloc(glsl_sources, sizeof(*glsl_sources) * (i + 1));

        if (!(glsl_sources[i] = fopen(argv[i + 1], "r")))
        {
            err("Bad glsl_sources open.");
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

        // Read code into string:
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
                if (next == '/' && last == '/')
                {
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
    if (fclose(f_header))
    {
        err("Bad close f_header.");
    }

    if (fclose(f_source))
    {
        err("Bad close f_source.");
    }

    for (int i = 0; i < glsl_sources_c; ++i)
    {
        if (fclose(glsl_sources[i]))
        {
            err("Bad close_glsl");
        }
    }
    free(glsl_sources);

    puts("NOTE: DON'T FORGET TO INCLUDE \"generated_shaders.h\" IN APPLICABLE SOURCE FILES!");

    return 0;
}
