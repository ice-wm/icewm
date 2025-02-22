#ifndef WMKEY_H
#define WMKEY_H

#include <X11/Xlib.h>

struct WMKey {
    const char* name;
    unsigned key;
    unsigned short mod, xm[2];
    unsigned char kc[2];
    bool initial;

    WMKey() : name(""), key(0), mod(0), initial(true) {
        xm[0] = xm[1] = kc[0] = kc[1] = 0;
    }
    WMKey(char* s) : name(s), key(0), mod(0), initial(false) {
        xm[0] = xm[1] = kc[0] = kc[1] = 0;
        parse();
    }
    WMKey(unsigned k, unsigned short m, const char* s) :
        name(s), key(k), mod(m), initial(true) {
        xm[0] = xm[1] = kc[0] = kc[1] = 0;
    }

    bool operator==(const WMKey& o) const { return key == o.key && mod == o.mod; }
    bool operator!=(const WMKey& o) const { return key != o.key || mod != o.mod; }
    bool operator==(const XKeyEvent& x) const;
    bool operator==(const XButtonEvent& b) const;
    bool parse();
    bool set(const char* arg);
    void grab(int handle);
};

#endif
