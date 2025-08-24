/*
 * IceWM - Colored cursor support
 *
 * Copyright (C) 2002 The Authors of IceWM
 *
 * initially by Oleastre
 * C++ style implementation by tbf
 */

#include "config.h"
#include "yxapp.h"
#include "ycursor.h"

#ifdef CONFIG_XPM
#include <X11/xpm.h>
#elif defined CONFIG_IMLIB2
#include <Imlib2.h>
typedef Imlib_Image Image;
#elif defined CONFIG_GDK_PIXBUF_XLIB
#include <gdk-pixbuf-xlib/gdk-pixbuf-xlib.h>
typedef GdkPixbuf* Image;
#else
#error "Need either XPM or Imlib2 for cursors."
#endif

#include "intl.h"

class YCursorPixmap {
public:
    YCursorPixmap(const char* path);
    ~YCursorPixmap();

    Pixmap pixmap() const { return fPixmap; }
    Pixmap mask() const { return fMask; }
    const XColor& background() const { return fBackground; }
    const XColor& foreground() const { return fForeground; }

#ifdef CONFIG_XPM

    bool isValid() { return fValid; }
    unsigned int width() const { return fAttributes.width; }
    unsigned int height() const { return fAttributes.height; }
    unsigned int hotspotX() const { return fAttributes.x_hotspot; }
    unsigned int hotspotY() const { return fAttributes.y_hotspot; }

#elif defined CONFIG_IMLIB2

    bool isValid() { return fImage && fPixmap; }
    void context() const { imlib_context_set_image(fImage);
                           imlib_context_set_drawable(xapp->root()); }
    unsigned int width() const { return fWidth; }
    unsigned int height() const { return fHeight; }
    unsigned int hotspotX() const { return fHotspotX; }
    unsigned int hotspotY() const { return fHotspotY; }
    void readHotspot(const char* path);

#elif defined CONFIG_GDK_PIXBUF_XLIB

    bool isValid() { return fImage && fPixmap; }
    unsigned int width() const { return fWidth; }
    unsigned int height() const { return fHeight; }
    unsigned int hotspotX() const { return fHotspotX; }
    unsigned int hotspotY() const { return fHotspotY; }
    void readHotspot(const char* path);

#endif

private:
    Pixmap fPixmap, fMask;
    XColor fForeground, fBackground;

#ifdef CONFIG_XPM

    bool fValid;
    XpmAttributes fAttributes;

#elif defined CONFIG_IMLIB2

    unsigned int fWidth, fHeight;
    unsigned int fHotspotX, fHotspotY;
    Image fImage;

#elif defined CONFIG_GDK_PIXBUF_XLIB

    unsigned int fWidth, fHeight;
    unsigned int fHotspotX, fHotspotY;
    Image fImage;

#endif
};

#ifdef CONFIG_XPM
//
// === use libXpm to load the cursor pixmap ===
//
YCursorPixmap::YCursorPixmap(const char* path):
    fPixmap(None), fMask(None),
    fValid(false)
{
    fAttributes.valuemask = XpmVisual|XpmColormap|XpmCloseness|XpmColorKey|
                            XpmReturnPixels|XpmSize|XpmHotspot|XpmDepth;
    fAttributes.visual    = xapp->visual();
    fAttributes.colormap  = xapp->colormap();
    fAttributes.depth     = xapp->depth();
    fAttributes.closeness = 65535;
    fAttributes.x_hotspot = 0;
    fAttributes.y_hotspot = 0;
    fAttributes.width = 0;
    fAttributes.height = 0;
    fAttributes.color_key = XPM_COLOR;

    int const rc(XpmReadFileToPixmap(xapp->display(), desktop->handle(),
                                     path,
                                     &fPixmap, &fMask, &fAttributes));

    if (rc != XpmSuccess)
        warn(_("Loading of pixmap \"%s\" failed: %s"),
               path, XpmGetErrorString(rc));
    /*else if (fAttributes.npixels != 2)
        warn("Invalid cursor pixmap: \"%s\" contains too many unique colors",
               path);*/
    else {
        fBackground.pixel = fAttributes.pixels[0];
        fForeground.pixel = fAttributes.pixels[1];

        XQueryColor(xapp->display(), xapp->colormap(), &fBackground);
        XQueryColor(xapp->display(), xapp->colormap(), &fForeground);

        fValid = true;
    }
}

#elif defined CONFIG_IMLIB2
//
// === use Imlib to load the cursor pixmap ===
//
YCursorPixmap::YCursorPixmap(const char* path):
    fPixmap(None), fMask(None),
    fWidth(0), fHeight(0),
    fHotspotX(0), fHotspotY(0)
{
    fImage = imlib_load_image_immediately_without_cache(path);
    if (fImage == nullptr) {
        warn(_("Loading of pixmap \"%s\" failed"), path);
        return;
    }
    context();
    fWidth = imlib_image_get_width();
    fHeight = imlib_image_get_height();

    unsigned inback = 0;
    unsigned infore = 0;
    DATA32 backgrnd = 0;
    DATA32 foregrnd = 0;
    DATA32* data = imlib_image_get_data_for_reading_only();
    DATA32* stop = data + width() * height();
    bool alpha = imlib_image_has_alpha();
    for (DATA32* p = data; p < stop; ++p) {
        unsigned char a = (unsigned char)((*p >> 24) & 0xFF);
        unsigned char r = (unsigned char)((*p >> 16) & 0xFF);
        unsigned char g = (unsigned char)((*p >> 8) & 0xFF);
        unsigned char b = (unsigned char)(*p & 0xFF);
        unsigned intens = r + g + b;
        if (alpha == 0 || 100 < a) {
            if (inback == 0 || (intens << 8) / a < inback) {
                inback = (intens << 8) / a;
                backgrnd = *p;
            }
            if (infore == 0 || a * intens > infore) {
                infore = a * intens;
                foregrnd = *p;
            }
        }
    }

    fForeground.pixel = None;
    fForeground.red = (unsigned short)(((foregrnd >> 16) & 0xFF) << 8);
    fForeground.green = (unsigned short)(((foregrnd >> 8) & 0xFF) << 8);
    fForeground.blue = (unsigned short)((foregrnd & 0xFF) << 8);
    XAllocColor(xapp->display(), xapp->colormap(), &fForeground);

    fBackground.pixel = None;
    fBackground.red = (unsigned short)(((backgrnd >> 16) & 0xFF) << 8);
    fBackground.green = (unsigned short)(((backgrnd >> 8) & 0xFF) << 8);
    fBackground.blue = (unsigned short)((backgrnd & 0xFF) << 8);
    XAllocColor(xapp->display(), xapp->colormap(), &fBackground);

    readHotspot(path);

    imlib_render_pixmaps_for_whole_image(&fPixmap, &fMask);
}

#elif defined CONFIG_GDK_PIXBUF_XLIB

YCursorPixmap::YCursorPixmap(const char* path):
    fPixmap(None), fMask(None),
    fWidth(0), fHeight(0),
    fHotspotX(0), fHotspotY(0),
    fImage(nullptr)
{
    fImage = gdk_pixbuf_new_from_file(path, NULL);
    if (fImage == nullptr) {
        warn(_("Loading of pixmap \"%s\" failed"), path);
        return;
    }

    fWidth = unsigned(gdk_pixbuf_get_width(fImage));
    fHeight = unsigned(gdk_pixbuf_get_height(fImage));
    int chans = gdk_pixbuf_get_n_channels(fImage);
    bool alpha = gdk_pixbuf_get_has_alpha(fImage);
    int rowstride = gdk_pixbuf_get_rowstride(fImage);
    guchar* data = gdk_pixbuf_get_pixels(fImage);

    unsigned inback = 0;
    unsigned infore = 0;
    struct { unsigned char r, g, b; }
        backgrnd = { 0, 0, 0 },
        foregrnd = { 0, 0, 0 };
    for (unsigned row = 0; row < fHeight; ++row) {
        guchar* p = data + row * rowstride;
        for (unsigned col = 0; col < fWidth; ++col, p += chans) {
            unsigned char a = alpha ? p[3] : 255;
            unsigned char r = p[0];
            unsigned char g = p[1];
            unsigned char b = p[2];
            unsigned intens = r + g + b;
            if (100 < a) {
                if (inback == 0 || (intens << 8) / a < inback) {
                    inback = (intens << 8) / a;
                    backgrnd.r = r;
                    backgrnd.g = g;
                    backgrnd.b = b;
                }
                if (infore == 0 || a * intens > infore) {
                    infore = a * intens;
                    foregrnd.r = r;
                    foregrnd.g = g;
                    foregrnd.b = b;
                }
            }
        }
    }

    fForeground.pixel = None;
    fForeground.red = (unsigned short)(foregrnd.r << 8);
    fForeground.green = (unsigned short)(foregrnd.g << 8);
    fForeground.blue = (unsigned short)(foregrnd.b << 8);
    XAllocColor(xapp->display(), xapp->colormap(), &fForeground);

    fBackground.pixel = None;
    fBackground.red = (unsigned short)(backgrnd.r << 8);
    fBackground.green = (unsigned short)(backgrnd.g << 8);
    fBackground.blue = (unsigned short)(backgrnd.b << 8);
    XAllocColor(xapp->display(), xapp->colormap(), &fBackground);

    readHotspot(path);

    gdk_pixbuf_xlib_render_pixmap_and_mask(fImage, &fPixmap, &fMask, 10);
}
#endif

#ifndef CONFIG_XPM
#if defined CONFIG_IMLIB2 || defined CONFIG_GDK_PIXBUF_XLIB
void YCursorPixmap::readHotspot(const char* path) {
    // --- find the hotspot by reading the xpm header manually ---
    FILE* xpm = fopen(path, "rb");
    if (xpm == nullptr)
        warn(_("BUG? Imlib was able to read \"%s\""), path);
    else {
        while (fgetc(xpm) != '{'); // --- that's safe since imlib accepted ---

        for (int c;;) switch (c = fgetc(xpm)) {
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
                    fHotspotX = (x < 0 ? 0 : x);
                    fHotspotY = (y < 0 ? 0 : y);
                } else if (tokens != 4)
                    warn(_("BUG? Malformed XPM header but Imlib "
                           "was able to parse \"%s\""), path);

                fclose(xpm);
                return;
            }
            default:
                if (c == EOF)
                    warn(_("BUG? Unexpected end of XPM file but Imlib "
                           "was able to parse \"%s\""), path);
                else
                    warn(_("BUG? Unexpected character but Imlib "
                           "was able to parse \"%s\""), path);

                fclose(xpm);
                return;
        }
    }
}
#endif
#endif

YCursorPixmap::~YCursorPixmap() {

#ifdef CONFIG_XPM
    if (fPixmap && xapp) {
        XFreePixmap(xapp->display(), fPixmap);
        fPixmap = None;
    }
    if (fMask && xapp) {
        XFreePixmap(xapp->display(), fMask);
        fMask = None;
    }
    if (fValid) {
        XpmFreeAttributes(&fAttributes);
        fValid = false;
    }

#elif defined CONFIG_IMLIB2
    if (fImage) {
        context();
        if (fPixmap) {
            imlib_free_pixmap_and_mask(fPixmap);
            fPixmap = fMask = None;
        }
        imlib_free_image();
        fImage = nullptr;
    }

#elif defined CONFIG_GDK_PIXBUF_XLIB
    if (fPixmap && xapp) {
        XFreePixmap(xapp->display(), fPixmap);
        fPixmap = None;
    }
    if (fMask && xapp) {
        XFreePixmap(xapp->display(), fMask);
        fMask = None;
    }
    if (fImage) {
        g_object_unref(G_OBJECT(fImage));
        fImage = nullptr;
    }

#endif
}

static Pixmap createMask(int w, int h) {
    return XCreatePixmap(xapp->display(), desktop->handle(), w, h, 1);
}

Cursor YCursor::load(const char* path) {
    Cursor fCursor = None;
    YCursorPixmap pixmap(path);

    if (pixmap.isValid()) {
        // === convert coloured pixmap into a bilevel one ===
        Pixmap bilevel(createMask(pixmap.width(), pixmap.height()));

        // --- figure out which plane we have to copy ---
        unsigned long pmask(1UL << (xapp->depth() - 1));

        if (pixmap.foreground().pixel &&
            pixmap.foreground().pixel != pixmap.background().pixel)
            while ((pixmap.foreground().pixel & pmask) ==
                   (pixmap.background().pixel & pmask)) pmask >>= 1;
        else if (pixmap.background().pixel)
            while ((pixmap.background().pixel & pmask) == 0) pmask >>= 1;

        GC gc; XGCValues gcv; // --- copy one plane by using a bilevel GC ---
        gcv.function = (pixmap.foreground().pixel &&
                       (pixmap.foreground().pixel & pmask))
                     ? GXcopyInverted : GXcopy;
        gc = XCreateGC (xapp->display(), bilevel, GCFunction, &gcv);

        XFillRectangle(xapp->display(), bilevel, gc, 0, 0,
                       pixmap.width(), pixmap.height());

        XCopyPlane(xapp->display(), pixmap.pixmap(), bilevel, gc,
                   0, 0, pixmap.width(), pixmap.height(), 0, 0, pmask);
        XFreeGC(xapp->display(), gc);

        // === allocate a new pixmap cursor ===
        XColor foreground(pixmap.foreground()),
               background(pixmap.background());

        fCursor = XCreatePixmapCursor(xapp->display(),
                                      bilevel, pixmap.mask(),
                                      &foreground, &background,
                                      pixmap.hotspotX(), pixmap.hotspotY());

        XFreePixmap(xapp->display(), bilevel);
    }
    return fCursor;
}

Cursor YCursor::load() {
    if (path) {
        cursor = unsigned(load(path));
        delete[] path; path = nullptr;
    }
    if (glyph && !cursor) {
        cursor = unsigned(XCreateFontCursor(xapp->display(), glyph));
    }
    return Cursor(cursor);
}

void YCursor::discard() {
    if (cursor && xapp) {
        XFreeCursor(xapp->display(), Cursor(cursor));
        cursor = 0;
    }
    if (path) {
        delete[] path; path = nullptr;
    }
}

// vim: set sw=4 ts=4 et:
