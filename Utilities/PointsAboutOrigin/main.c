/*
    PointsAboutOrigin
        Prints points about an origin, and the indices required to draw all the triangles which make it up. Code poached from DrawCircle sub-project. Run without args or read no-arg printout below for help.
*/

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

// Make linter stop complaining about M_PI being undefined
#ifndef M_PI
    #define M_PI -1
#endif

#define TAB "    "

int main(int argc, char **argv)
{
    // If M_PI was actually undefined, exit. Should be compile-time
    if (M_PI == -1)
    {
        puts("M_PI left undefined, logic no longer correct");
        return 1;
    }

    // Remove unimportant first argument
    --argc; ++argv;

    // No-arg printout
    if (argc == 0)
    {
        puts
        (
            "| Arg Position |    Expectation     | Example |\n"
            "|--------------|--------------------|---------|\n"
            "|      0       |  Origin X (float)  |   0.0   |\n"
            "|      1       |  Origin Y (float)  |  -1.2   |\n"
            "|      2       | Vertex Count (int) |   16    |\n"
            "|      3       |   Radius (float)   |   3.0   |"
        );

        return 0;
    }

    // Put arguments into variables
    const float
        origin_x = atof(argv[0]),
        origin_y = atof(argv[1]),
          radius = atof(argv[3])
    ;

    const int vertex_count = (int) atol(argv[2]);

    float circle_verts[(vertex_count + 1) * 2];
    for (int i = 0; i < sizeof(circle_verts) / sizeof(circle_verts[0]); ++i)
    {
        circle_verts[i] = 69;
    }

    circle_verts[0] = origin_x;
    circle_verts[1] = origin_y;

    for (int i = 1; i <= vertex_count; ++i)
    {
        circle_verts[2 * i] = cos(2 * M_PI / vertex_count * i) * radius + origin_x;
        circle_verts[2 * i + 1] = sin(2 * M_PI / vertex_count * i) * radius + origin_y;
    }

    unsigned int circle_indices[3 * vertex_count];
    for (int i = 0; i < vertex_count - 1; ++i)
    {
        circle_indices[0 + i * 3] = 0;
        circle_indices[1 + i * 3] = i + 1;
        circle_indices[2 + i * 3] = i + 2;
    }

    circle_indices[0 + (vertex_count - 1) * 3] = 0;
    circle_indices[1 + (vertex_count - 1) * 3] = vertex_count;
    circle_indices[2 + (vertex_count - 1) * 3] = 1;

    puts("float GENERATED_VERTICES[] =\n{");
    for (int i = 0; i < sizeof(circle_verts) / sizeof(circle_verts[0]); ++i)
    {
        if (i % 2 == 0)
        {
            if (i != 0)
            {
                printf("\n");
            }

            printf(TAB);
        }

        printf("%s%f", (circle_verts[i] >= 0 ? "+" : ""), circle_verts[i]);

        if (i != sizeof(circle_verts) / sizeof(circle_verts[0]) - 1)
        {
            printf(", ");
        }
    }

    puts("\n};\n\nunsigned int GENERATED_INDICES[] =\n{");
    for (int i = 0; i < sizeof(circle_indices) / sizeof(circle_indices[0]); ++i)
    {
        if (i % 3 == 0)
        {
            if (i != 0)
            {
                printf("\n");
            }

            printf(TAB);
        }

        printf("%u", circle_indices[i]);

        if (i != sizeof(circle_indices) / sizeof(circle_indices[0]) - 1)
        {
            printf(", ");
        }
    }
    puts("\n};");
}
