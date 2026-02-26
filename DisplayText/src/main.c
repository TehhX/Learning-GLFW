// See https://learnopengl.com/In-Practice/Text-Rendering

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

    // Create a FreeMono face
    FT_Face face;
    if (FT_New_Face(library, "", 0, &face))
    {
        // If fail, return error
        puts("Failed to load font face.");
        return 1;
    }

    // Define the size in pixels to extract from the face. Using 0 for width sets the width based on the height dynamically.
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Load specific character from face, in this case 'Q'. FT_LOAD_RENDER tells it to create an 8-bit B&W bitmap.
    FT_Load_Char(face, 'Q', FT_LOAD_RENDER);
}
