#include <ft2build.h> // Main/Entry point for freetype2
#include FT_FREETYPE_H // Base ft2 API

#include "stdio.h"

int main()
{
    // Handle to the freetype library
    FT_Library library;

    // Initialize the library, check if error occured
    if (FT_Init_FreeType(&library) != 0)
    {
        // If fail, return error
        puts("Failed to initialize FreeType2.");
        return 1;
    }
}
