#ifndef YCURSOR_H
#define YCURSOR_H

class YCursor {
public:
    void init(const char* p, unsigned g, const char* x) {
        cursor = 0; glyph = g; path = p; xname = x; }
    operator Cursor() { return cursor ? Cursor(cursor) : load(); }
    void discard();
    ~YCursor() { discard(); }

private:
    static Cursor load(const char* path);

    unsigned cursor;
    unsigned glyph;
    const char* path;
    const char* xname;
    Cursor load();
};

#endif

// vim: set sw=4 ts=4 et:
