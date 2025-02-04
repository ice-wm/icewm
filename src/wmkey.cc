#include <X11/X.h>
#include "wmkey.h"
#include "yxapp.h"
#include "yprefs.h"

bool WMKey::set(const char* arg) {
    bool change = false;
    if (isEmpty(arg)) {
        if (nonempty(name)) {
            key = mod = xmod = kc = 0;
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
            xmod = kc = 0;
        }
    }
    return change;
}

bool WMKey::parse() {
    if (nonempty(name)) {
        key = mod = xmod = kc = 0;
        return YConfig::parseKey(name, &key, &mod);
    } else {
        return false;
    }
}

void WMKey::grab(int handle) {
    if (mod && xmod == 0) {
        unsigned short xm = 0;
        bool ok = true;
        if (mod & kfShift)
            xm |= ShiftMask;
        if (mod & kfCtrl)
            xm |= ControlMask;
        if (mod & kfAlt) {
            if (xapp->AltMask)
                xm |= xapp->AltMask;
            else
                ok = false;
        }
        if (mod & kfMeta) {
            if (xapp->MetaMask)
                xm |= xapp->MetaMask;
            else
                ok = false;
        }
        if (mod & kfSuper) {
            if (xapp->SuperMask)
                xm |= xapp->SuperMask;
            else
                ok = false;
        }
        if (mod & kfHyper) {
            if (xapp->HyperMask)
                xm |= xapp->HyperMask;
            else
                ok = false;
        }
        if (mod & kfAltGr) {
            if (xapp->ModeSwitchMask)
                xm |= xapp->ModeSwitchMask;
            else
                ok = false;
        }
        if (xm && ok) {
            xmod = xm;
        } else {
            return;
        }
    }
    if (kc == 0 && key) {
        xapp->unshift(&key, &xmod);
        kc = XKeysymToKeycode(xapp->display(), KeySym(key));
    }
    if (kc) {
        unsigned short mods[8];
        int count = 0;
        mods[count++] = xmod;
        mods[count++] = xmod | LockMask;
        if (xapp->NumLockMask) {
            mods[count++] = xmod | xapp->NumLockMask;
            mods[count++] = xmod | LockMask | xapp->NumLockMask;
        }
        if (hasbits(mod, kfAlt | kfCtrl) && modSuperIsCtrlAlt && xapp->WinMask) {
            unsigned short wmod = xmod;
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
            XGrabKey(xapp->display(), kc, mods[i], handle,
                     False, GrabModeAsync, GrabModeAsync);
        }
    }
}

