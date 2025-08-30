#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>
// #include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>
#if CONFIG_IMLIB2
#include <Imlib2.h>
#elif CONFIG_GDK_PIXBUF_XLIB
#include <gdk-pixbuf-xlib/gdk-pixbuf-xlib.h>
#else
#error "Require Imlib2 or GdkPixbuf"
#endif

#if CONFIG_IMLIB2
static XcursorImage* load(const char* input_image) {
    // Initialize Imlib2
    Imlib_Image image;
    int width, height;

    // Load the image
    image = imlib_load_image(input_image);
    if (!image) {
        fprintf(stderr, "Error: Could not load image '%s'\n", input_image);
        return NULL;
    }
    imlib_context_set_image(image);
    width = imlib_image_get_width();
    height = imlib_image_get_height();

    // Get image data
    DATA32 *data = imlib_image_get_data_for_reading_only();

    // Create Xcursor image
    XcursorImage *cursor = XcursorImageCreate(width, height);
    if (!cursor) {
        fprintf(stderr, "Error: Could not create Xcursor image\n");
        imlib_free_image();
        return NULL;
    }

    // Copy pixel data from Imlib2 to Xcursor
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            DATA32 pixel = data[y * width + x];
            cursor->pixels[y * width + x] = (
                (((pixel >> 24) & 0xff) << 24) | // Alpha
                (((pixel >> 16) & 0xff) << 16) | // Red
                (((pixel >>  8) & 0xff) <<  8) | // Green
                (((pixel >>  0) & 0xff) <<  0));  // Blue
        }
    }

    imlib_free_image();

    return cursor;
}
#elif CONFIG_GDK_PIXBUF_XLIB
static XcursorImage* load(const char* input_image) {
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(input_image, &error);
    if (!pixbuf) {
        fprintf(stderr, "Error: Could not load image '%s': %s\n",
                input_image, error->message);
        g_error_free(error);
        return NULL;
    }

    int width = gdk_pixbuf_get_width(pixbuf);
    int height = gdk_pixbuf_get_height(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    guchar *pixels = gdk_pixbuf_get_pixels(pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels(pixbuf);

    // Create Xcursor image
    XcursorImage *cursor = XcursorImageCreate(width, height);
    if (!cursor) {
        fprintf(stderr, "Error: Could not create Xcursor image\n");
        g_object_unref(pixbuf);
        return NULL;
    }

    // Copy pixel data from GdkPixbuf to Xcursor
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            guchar *p = pixels + y * rowstride + x * n_channels;
            guint32 pixel = 0;
            if (n_channels == 4) {
                // RGBA
                pixel = (p[3] << 24) | (p[0] << 16) | (p[1] << 8) | p[2];
            } else if (n_channels == 3) {
                // RGB (no alpha)
                pixel = (0xff << 24) | (p[0] << 16) | (p[1] << 8) | p[2];
            }
            cursor->pixels[y * width + x] = pixel;
        }
    }

    g_object_unref(pixbuf);

    return cursor;
}
#endif

static int read_hotspot(const char* prog, const char* path, int* xhot, int* yhot) {
    // --- find the hotspot by reading the xpm header manually ---
    FILE* xpm = fopen(path, "rb");
    if (xpm == nullptr) {
        fprintf(stderr, "%s: Cannot open %s for reading: %s\n",
                prog, path, strerror(errno));
        return -1;
    }

    char header[10] = "";
    if (fread(header, 1, 9, xpm) != 9 ||
        strncmp(header, "/* XPM */", 9))
    {
        fclose(xpm);
        return -1;
    }

    while (fgetc(xpm) != '{'); // --- that's safe since imlib accepted ---

    int code = -1;
    while (xpm) {
        int c = fgetc(xpm);
        switch (c) {
        case '/':
            if ((c = fgetc(xpm)) == '/') // --- eat C++ line comment ---
                while (fgetc(xpm) != '\n');
            else { // --- eat block comment ---
               int pc; do { pc = c; c = fgetc(xpm); }
               while (c != '/' && pc != '*');
            }
            break;

        case ' ': case '\t': case '\r': case '\n': // --- whitespace ---
            break;

        case '"': { // --- the XPM header ---
            unsigned foo; int x = 0, y = 0;
            int tokens = fscanf(xpm, "%u %u %u %u %d %d",
                &foo, &foo, &foo, &foo, &x, &y);
            if (tokens == 6) {
                *xhot = (x < 0 ? 0 : x);
                *yhot = (y < 0 ? 0 : y);
                code = 0;
            } else if (tokens != 4)
                fprintf(stderr, "%s: Malformed XPM header (%d)\n", prog, tokens);

            fclose(xpm);
            xpm = nullptr;
            break;
        }
        default:
            if (c == EOF)
                fprintf(stderr, "%s: Unexpected end of XPM %s\n", prog, path);
            else
                fprintf(stderr, "%s: Unexpected character %s\n", prog, path);

            fclose(xpm);
            xpm = nullptr;
        }
    }
    return code;
}

static void guess_hotspot(const char* path, int* xhot, int* yhot, int width, int height) {
    const char* base = basename(path);
    size_t len = strspn(base, "BDLRTUcefghilmorstvz");
    if (len >= 4) {
        if (!strncmp(base, "scroll", 5)) {
            if (!strncmp(base, "scrollL", len))
                *xhot = 0, *yhot = height / 2;
            else if (!strncmp(base, "scrollR", len))
                *xhot = width - 1, *yhot = height / 2;
            else if (!strncmp(base, "scrollU", len))
                *xhot = width / 2, *yhot = 0;
            else if (!strncmp(base, "scrollD", len))
                *xhot = width / 2, *yhot = height - 1;
        }
        else if (!strncmp(base, "size", 4)) {
            if (!strncmp(base, "sizeR", len))
                *xhot = width - 1, *yhot = height / 2;
            else if (!strncmp(base, "sizeTR", len))
                *xhot = width - 1, *yhot = 0;
            else if (!strncmp(base, "sizeT", len))
                *xhot = width / 2, *yhot = 0;
            else if (!strncmp(base, "sizeTL", len))
                *xhot = 0, *yhot = 0;
            else if (!strncmp(base, "sizeL", len))
                *xhot = 0, *yhot = height / 2;
            else if (!strncmp(base, "sizeBL", len))
                *xhot = 0, *yhot = height - 1;
            else if (!strncmp(base, "sizeB", len))
                *xhot = width / 2, *yhot = height - 1;
            else if (!strncmp(base, "sizeBR", len))
                *xhot = width - 1, *yhot = height - 1;
        }
        else {
            if (!strncmp(base, "left", len))
                *xhot = *yhot = 0;
            else if (!strncmp(base, "move", len))
                *xhot = width / 2, *yhot = height / 2;
            else if (!strncmp(base, "right", len))
                *xhot = width - 1, *yhot = 0;
        }
    }
}

static void icecur_help(int code, const char* prog)
{
    fprintf(stderr, "Usage: %s [options] <input_image> <output_cursor>\n",
            prog);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-x number: the X-hotspot position\n");
    fprintf(stderr, "\t-y number: the Y-hotspot position\n");
    fflush(stderr);
    _exit(code);
}

int main(int argc, char **argv)
{
    int opt, xhot = -1, yhot = -1;

    while ((opt = getopt(argc, argv, "x:y:h?")) != -1) {
        switch (opt) {
            case 'x': xhot = atoi(optarg); break;
            case 'y': yhot = atoi(optarg); break;
            default: icecur_help(1, *argv); break;
        }
    }

    if (optind + 2 != argc) {
        icecur_help(1, *argv);
    }

    const char *input_image = argv[optind];
    const char *output_cursor = argv[optind + 1];

    XcursorImage* cursor = load(input_image);
    if (cursor == NULL) {
        return 1;
    }

    if (xhot < 0 || xhot >= int(cursor->width) ||
        yhot < 0 || yhot >= int(cursor->height)) {
        read_hotspot(*argv, input_image, &xhot, &yhot);
    }
    if (xhot < 0 || xhot >= int(cursor->width) ||
        yhot < 0 || yhot >= int(cursor->height)) {
        guess_hotspot(input_image, &xhot, &yhot,
                      int(cursor->width), int(cursor->height));
        guess_hotspot(output_cursor, &xhot, &yhot,
                      int(cursor->width), int(cursor->height));
    }
    if (xhot >= int(cursor->width)) {
        xhot = int(cursor->width) - 1;
    }
    if (xhot < 0) {
        xhot = 0;
    }
    if (yhot >= int(cursor->height)) {
        yhot = int(cursor->height) - 1;
    }
    if (yhot < 0) {
        yhot = 0;
    }

    cursor->xhot = xhot;
    cursor->yhot = yhot;

    XcursorImages images = { 1, &cursor, NULL };
    if (XcursorFilenameSaveImages(output_cursor, &images) == XcursorFalse) {
        fprintf(stderr, "%s: Failed to save cursor to '%s'\n",
                *argv, output_cursor);
        XcursorImageDestroy(cursor);
        return 1;
    }

    // Cleanup
    XcursorImageDestroy(cursor);

    printf("Cursor written to '%s'\n", output_cursor);
    return 0;
}

