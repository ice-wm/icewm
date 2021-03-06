/*
 *  IceWM - Resource paths
 *  Copyright (C) 2001 The Authors of IceWM
 *
 *  Release under terms of the GNU Library General Public License
 *
 *  2001/03/24: Mathias Hasselmann <mathias.hasselmann@gmx.net>
 *  - initial version
 */

#include "config.h"
#include "ypaths.h"
#include "yapp.h"
#include "yprefs.h"
#include "intl.h"
#include <unistd.h>
#include <fcntl.h>

void YResourcePaths::addDir(upath dir) {
    if (dir.dirExists())
        fPaths.append(dir);
}

ref<YResourcePaths> YResourcePaths::subdirs(const char* subdir, bool themeOnly) {
    YResourcePaths* paths(new YResourcePaths);

    upath privDir(YApplication::getPrivConfDir());

    upath themeFile(themeName);
    mstring themeExt(themeFile.getExtension());
    upath themeDir(themeExt.isEmpty() ? themeFile : themeFile.parent());

    if (themeDir.isAbsolute()) {
        MSG(("Searching `%s' resources at absolute location", subdir));

        if (themeOnly) {
            paths->addDir(themeDir);
        } else {
            paths->addDir(privDir);
            paths->addDir(themeDir);
            paths->addDir(YApplication::getConfigDir());
            paths->addDir(YApplication::getLibDir());
        }
    } else {
        MSG(("Searching `%s' resources at relative locations", subdir));

        upath themes("/themes/");
        upath themesPlusThemeDir(themes + themeDir);

        if (themeOnly) {
            paths->addDir(privDir + themesPlusThemeDir);
            paths->addDir(YApplication::getConfigDir() + themesPlusThemeDir);
            paths->addDir(YApplication::getLibDir() + themesPlusThemeDir);

        } else {
            paths->addDir(privDir + themesPlusThemeDir);
            paths->addDir(privDir);
            paths->addDir(YApplication::getConfigDir() + themesPlusThemeDir);
            paths->addDir(YApplication::getConfigDir());
            paths->addDir(YApplication::getLibDir() + themesPlusThemeDir);
            paths->addDir(YApplication::getLibDir());
        }
    }

    if (nonempty(subdir))
        paths->verifyPaths(subdir);

    return ref<YResourcePaths>(paths);
}

void YResourcePaths::verifyPaths(const char* base) {
    for (int i = fPaths.getCount(); --i >= 0; ) {
        upath path(fPaths[i]);
        if (path.relative(base).isExecutable() == false) {
            fPaths.remove(i);
        }
    }
}

template<class Pict>
bool YResourcePaths::loadPictFile(upath file, ref<Pict>* pict) {
    if (file.isReadable()) {
        *pict = Pict::load(file);
        if (*pict != null) {
            return true;
        }
    }
    warn(_("Image not readable: %s"), file.string());
    return false;
}

ref<YPixmap> YResourcePaths::loadPixmapFile(const upath& file) {
    ref<YPixmap> p;
    return loadPictFile(file, &p) && p->pixmap() ? p : null;
}

ref<YImage> YResourcePaths::loadImageFile(const upath& file) {
    ref<YImage> p;
    return loadPictFile(file, &p) && p->valid() ? p : null;
}

template<class Pict>
bool YResourcePaths::loadPict(upath baseName, ref<Pict>* pict) const {
    for (upath& base : *this) {
        upath path(base + baseName);
        if (path.fileExists() && loadPictFile(path, pict))
            return true;
    }
#ifdef DEBUG
    if (debug)
        warn(_("Could not find image %s"), baseName.string());
#endif
    return false;
}


ref<YPixmap> YResourcePaths::loadPixmap(upath base, upath name) const {
    ref<YPixmap> p;
    return loadPict(base + name, &p) && p->pixmap() ? p : null;
}

ref<YImage> YResourcePaths::loadImage(upath base, upath name) const {
    ref<YImage> p;
    return loadPict(base + name, &p) && p->valid() ? p : null;
}

// vim: set sw=4 ts=4 et:
