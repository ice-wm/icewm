#ifndef WMKEY_H
#define WMKEY_H

struct WMKey {
    unsigned key;
    unsigned short mod, xmod;
    const char* name;
    unsigned char kc;
    bool initial;

    WMKey() : key(0), mod(0), xmod(0), name(""), kc(0), initial(true) { }
    WMKey(char* s) : key(0), mod(0), xmod(0), name(s), kc(0), initial(false) {
        parse(); }
    WMKey(KeySym k, unsigned short m, const char* s) :
        key(k), mod(m), xmod(0), name(s), kc(0), initial(true) { }

    bool eq(KeySym k, unsigned short m) const { return key == k && mod == m; }
    bool operator==(const WMKey& o) const { return eq(o.key, o.mod); }
    bool operator!=(const WMKey& o) const { return !eq(o.key, o.mod); }
    bool parse();
    bool set(const char* arg);
    void grab(int handle);
};

#endif
