#include <X11/X.h>
#include "wmkey.h"
#include "yxapp.h"
#include "yprefs.h"

#define IS_XF86KEY(key) (0x1008FE01U <= key && key <= 0x1008FFFFU)
#define IS_POINTER(key) (0x0000FEE0U <= key && key <= 0x0000FEFFU)

bool WMKey::set(const char* arg) {
    bool change = false;
    if (isEmpty(arg)) {
        if (nonempty(name)) {
            key = mod = 0;
            xm[0] = xm[1] = kc[0] = kc[1] = 0;
            name = "";
            change = true;
            initial = true;
        }
    }
    else {
        unsigned ks = 0;
        unsigned short mo = 0;
        if (YConfig::parseKey(arg, &ks, &mo)) {
            if (initial == false)
                delete[] const_cast<char *>(name);
            name = newstr(arg);
            initial = false;
            change = true;
            key = ks;
            mod = mo;
            xm[0] = xm[1] = kc[0] = kc[1] = 0;
        }
    }
    return change;
}

bool WMKey::parse() {
    if (nonempty(name)) {
        key = mod = 0;
        xm[0] = xm[1] = kc[0] = kc[1] = 0;
        return YConfig::parseKey(name, &key, &mod);
    } else {
        return false;
    }
}

void WMKey::grab(int handle) {
    if (mod && xm[0] == 0) {
        unsigned short km = 0;
        bool ok = true;
        if (mod & kfShift)
            km |= ShiftMask;
        if (mod & kfCtrl)
            km |= ControlMask;
        if (mod & kfAlt) {
            if (xapp->AltMask)
                km |= xapp->AltMask;
            else
                ok = false;
        }
        if (mod & kfMeta) {
            if (xapp->MetaMask)
                km |= xapp->MetaMask;
            else
                ok = false;
        }
        if (mod & kfSuper) {
            if (xapp->SuperMask)
                km |= xapp->SuperMask;
            else
                ok = false;
        }
        if (mod & kfHyper) {
            if (xapp->HyperMask)
                km |= xapp->HyperMask;
            else
                ok = false;
        }
        if (mod & kfAltGr) {
            if (xapp->ModeSwitchMask)
                km |= xapp->ModeSwitchMask;
            else
                ok = false;
        }
        if (km && ok) {
            xm[0] = xm[1] = km;
        } else {
            return;
        }
    }
    if (IS_POINTER(key)) {
        int button = key - XK_Pointer_Button1 + Button1;
        if (inrange(button, Button1, Button3)) {
            if (hasbits(mod, kfAlt | kfCtrl) &&
                modSuperIsCtrlAlt && xapp->WinMask) {
                xm[1] = xapp->WinMask | (xm[0] & ~(ControlMask | xapp->AltMask));
            }
            unsigned short mods[8];
            int count = 0;
            for (int i = 0; i < 2; ++i) {
                if (i == 0 || (xm[i] != xm[0] && xm[i])) {
                    mods[count++] = xm[i];
                    mods[count++] = xm[i] | LockMask;
                    if (xapp->NumLockMask) {
                        mods[count++] = xm[i] | xapp->NumLockMask;
                        mods[count++] = xm[i] | xapp->NumLockMask | LockMask;
                    }
                }
            }
            for (int i = 0; i < count; ++i) {
                XGrabButton(xapp->display(), unsigned(button), mods[i],
                            handle, True, ButtonPressMask,
                            GrabModeAsync, GrabModeAsync, None, None);
            }
        }
        return;
    }
    if (kc[0] == 0 && key) {
        YKeycodeMap map(xapp->getKeycodeMap());
        if (map) {
            const bool sh = ((' ' < key && key < 'a')
                          || ('z' < key && key <= 0xFF)
                          || IS_XF86KEY(key));
            int n = 0;
            for (int i = map.min; i <= map.max; i++) {
                if (map.map[(i - map.min) * map.per] == key) {
                    kc[n] = (unsigned char) (i & 0xFF);
                    if (++n >= 2)
                        break;
                }
                if (sh && map.map[(i - map.min) * map.per + 1] == key) {
                    xm[n] |= ShiftMask;
                    kc[n] = (unsigned char) (i & 0xFF);
                    if (++n >= 2)
                        break;
                }
            }
        } else {
            xapp->unshift(&key, &xm[0]);
            kc[0] = XKeysymToKeycode(xapp->display(), KeySym(key));
        }
    }
    for (int k = 0; k < 2 && kc[k]; ++k) {
        unsigned short mods[8];
        int count = 0;
        mods[count++] = xm[k];
        mods[count++] = xm[k] | LockMask;
        if (xapp->NumLockMask) {
            mods[count++] = xm[k] | xapp->NumLockMask;
            mods[count++] = xm[k] | LockMask | xapp->NumLockMask;
        }
        if (hasbits(mod, kfAlt | kfCtrl) && modSuperIsCtrlAlt && xapp->WinMask) {
            unsigned short wmod = xm[k];
            wmod &= ~(ControlMask | xapp->AltMask);
            wmod |= xapp->WinMask;
            mods[count++] = wmod;
            mods[count++] = wmod | LockMask;
            if (xapp->NumLockMask) {
                mods[count++] = wmod | xapp->NumLockMask;
                mods[count++] = wmod | LockMask | xapp->NumLockMask;
            }
        }
        for (int i = 0; i < count; ++i) {
            XGrabKey(xapp->display(), kc[k], mods[i], handle,
                     False, GrabModeAsync, GrabModeAsync);
        }
    }
}

bool WMKey::operator==(const XKeyEvent& x) const {
    return (x.keycode == kc[0] && KEY_MODMASK(x.state) == xm[0])
        || (x.keycode == kc[1] && KEY_MODMASK(x.state) == xm[1]);
}

bool WMKey::operator==(const XButtonEvent& b) const {
    return (key == b.button - Button1 + XK_Pointer_Button1
         && mod == desktop->VMod(KEY_MODMASK(b.state)));
}

