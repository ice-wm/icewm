/*
 * IceWM
 *
 * Copyright (C) 1998-2001 Marko Macek
 */

#include "config.h"
#include "obj.h"
#include "objmenu.h"
#include "default.h"
#include "browse.h"
#include "wmmgr.h"
#include "wmprog.h"
#include "yicon.h"
#include "wmapp.h"
#include "base.h"
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

BrowseMenu::BrowseMenu(IApp* app, upath path)
    : app(app)
    , fPath(path)
    , fModTime(None)
{
    setActionListener(this);
}

BrowseMenu::~BrowseMenu() {
    for (const ActionName& an : names) {
        delete[] an.name;
    }
}

void BrowseMenu::actionPerformed(YAction action, unsigned) {
    for (const ActionName& an : names) {
        if (action == an.action) {
            upath target(fPath + an.name);
            const char *args[] = { openCommand, target.string(), nullptr };
            app->runProgram(openCommand, args);
            break;
        }
    }
}

void BrowseMenu::updatePopup() {
    struct stat sb;

    if (fPath.stat(&sb) != 0)
        removeAll();
    else if (sb.st_mtime > fModTime) {
        fModTime = sb.st_mtime;
        loadItems();
    }
}

void BrowseMenu::loadItems() {
    removeAll();

    DIR* dir = opendir(fPath.string());
    if (dir == nullptr) {
        return;
    }

    YStringArray dirs, fils;
    struct stat st;

    for (struct dirent* ent; (ent = readdir(dir)) != nullptr; ) {
        if (ent->d_name[0] == '.') {
            continue;
        }
#if DT_DIR
        else if (ent->d_type != DT_UNKNOWN) {
            if (ent->d_type == DT_DIR) {
                dirs.append(ent->d_name);
            } else if (ent->d_type == DT_REG) {
                fils.append(ent->d_name);
            }
        }
#endif
        else if (fstatat(dirfd(dir), ent->d_name, &st, AT_SYMLINK_NOFOLLOW) == 0) {
            if (S_ISDIR(st.st_mode)) {
                dirs.append(ent->d_name);
            } else if (S_ISREG(st.st_mode)) {
                fils.append(ent->d_name);
            }
        }
    }
    closedir(dir);

    if (dirs.nonempty()) {
        ref<YIcon> icon = YIcon::getIcon("folder");
        dirs.sort();
        for (const char* name: dirs) {
            upath npath(fPath + name);
            ActionName an;
            an.name = newstr(name);
            YMenu* sub = new BrowseMenu(app, npath);
            YMenuItem *item = addItem(an.name, -1, an.action, sub);
            item->setIcon(icon);
            names.append(an);
        }
    }
    if (fils.nonempty()) {
        ref<YIcon> icon = YIcon::getIcon("file");
        fils.sort();
        for (const char* name: fils) {
            upath npath(fPath + name);
            ActionName an;
            an.name = newstr(name);
            YMenuItem *item = addItem(an.name, -1, an.action, nullptr);
            item->setIcon(icon);
            names.append(an);
        }
    }
    setSelectedItem(0);
}

// vim: set sw=4 ts=4 et:
