/*
 * IceWM
 *
 * Copyright (C) 1997-2002 Marko Macek
 */
#include "config.h"
#include "themes.h"
#include "ymenuitem.h"
#include "prefs.h"
#include "yprefs.h"
#include "wmapp.h"
#include "wmconfig.h"
#include "appnames.h"
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include "intl.h"

const unsigned utf32ellipsis = 0x2026;
extern YFont menuFont;

void ThemesMenu::open(DTheme* dtheme) {
    WMConfig::setDefaultTheme(dtheme->theme());

    const char *bg[] = { ICEWMBGEXE, "-r", nullptr };
    int pid = app->runProgram(bg[0], bg);
    app->waitProgram(pid);

    smActionListener->handleSMAction(ICEWM_ACTION_RESTARTWM);
}

ThemesMenu::ThemesMenu(IApp* app, YSMListener* smListener)
    : app(app)
    , smActionListener(smListener)
{
}

void ThemesMenu::cleanup() {
    removeAll();
    themeArray.clear();
    alternatives.clear();
    libThemes = null;
    cfgThemes = null;
    prvThemes = null;
    iconTimer = null;
    cleanupTimer = null;
}

void ThemesMenu::deactivatePopup() {
    YMenu::deactivatePopup();
    cleanupTimer->setTimer(12L, this, true);
}

void ThemesMenu::actionPerformed(YAction action, unsigned modifiers) {
    for (DTheme* dtheme : themeArray) {
        if (action == dtheme->action()) {
            return open(dtheme);
        }
    }
    for (DTheme* dtheme : alternatives) {
        if (action == dtheme->action()) {
            return open(dtheme);
        }
    }
}

void ThemesMenu::updatePopup() {
    cleanup();

    for (int i = 0; i <= 2; ++i) {
        scanThemes(i);
    }

    makeMenus();

    YMenuItem* im = newThemeItem(_("Default"), CONFIG_DEFAULT_THEME, 2);
    if (im) {
        addSeparator();
        add(im);
    }

    iconIndex = nestIndex = 0;
    iconTimer->setTimer(0L, this, true);
    setActionListener(this);
}

upath ThemesMenu::themesTop(int index) {
    const char subdir[] = "/themes/";
    switch (index) {
        case 0:
            if (prvThemes == null)
                prvThemes = YApplication::getPrivConfDir() + subdir;
            return prvThemes;
        case 1:
            if (cfgThemes == null)
                cfgThemes = YApplication::getConfigDir() + subdir;
            return cfgThemes;
        case 2:
            if (libThemes == null)
                libThemes = YApplication::getLibDir() + subdir;
            return libThemes;
        default:
            return null;
    }
}

void ThemesMenu::scanThemes(int top) {
    upath path = themesTop(top);
    if (path == null) {
        return;
    }
    DIR* dir = opendir(path.string());
    if (dir == nullptr) {
        return;
    }

    for (struct dirent* ent; (ent = readdir(dir)) != nullptr; ) {
        if (ent->d_name[0] == '.') {
            continue;
        }
#if DT_DIR
        if (ent->d_type != DT_UNKNOWN && ent->d_type != DT_DIR) {
            continue;
        }
#endif
        mstring def(ent->d_name, "/default.theme");
        if (faccessat(dirfd(dir), def.c_str(), R_OK, 0) == 0) {
            mstring name(ent->d_name);
            bool found = false;
            int lo = 0, hi = themeArray.getCount(), col = 0;
            while (lo < hi) {
                int pv = (lo + hi) / 2;
                col = name.collate(themeArray[pv]->theme(), true);
                if (col < 0)
                    hi = pv;
                else if (col > 0)
                    lo = pv + 1;
                else {
                    col = name.compareTo(themeArray[pv]->theme());
                    if (col < 0)
                        hi = pv;
                    else if (col > 0)
                        lo = pv + 1;
                    else
                        lo = hi = pv, found = true;
                }
            }
            if (found == false) {
                DTheme* thm = new DTheme(name, top);
                if (thm) {
                    themeArray.insert((col < 0) ? hi : lo, thm);
                }
            }
        }
    }
    closedir(dir);
}

ThemesMenu::~ThemesMenu() {
}

YMenuItem* ThemesMenu::newThemeItem(
    mstring label,
    mstring theme,
    int top)
{
    YMenuItem* item = nullptr;
    DTheme* dtheme = new DTheme(theme, top);
    if (dtheme) {
        item = new YMenuItem(label, -3, null, dtheme->action(), nullptr);
        if (item) {
            if (theme == themeName)
                item->setChecked(true);
            alternatives += dtheme;
        } else {
            delete dtheme;
        }
    }
    return item;
}

YMenuItem* ThemesMenu::addThemeToMenu(DTheme* dtheme, YMenu* menu) {
    YMenuItem* im = menu->addItem(dtheme->theme(), -3, null,
                                  dtheme->action());
    if (im) {
        if (dtheme->theme() == themeName) {
            im->setChecked(true);
        }
    }
    return im;
}

void ThemesMenu::makeMenus() {
    bool nesting = inrange(nestedThemeMenuMinNumber, 1, themeArray.getCount() - 1);
    bool canUcodeEl = menuFont != null && menuFont->supports(utf32ellipsis);
    char subName[5];
    memcpy(&subName, canUcodeEl ? "X\xe2\x80\xa6" : "X...", 5);

    if (nesting == false) {
        for (DTheme* dtheme : themeArray) {
            addThemeToMenu(dtheme, this);
        }
        return;
    }

    int letter = -1;
    YArray<pair<int,int>> pairs;
    for (int i = 0; i < themeArray.getCount(); ++i) {
        DTheme* dtheme = themeArray[i];
        int low = tolower((unsigned char) dtheme->theme()[0]);
        if (low >= 0) {
            if (low != letter) {
                letter = low;
                pairs += pair<int,int>(i, i);
            } else {
                pairs.last().right = i;
            }
        }
    }

    for (const pair<int,int>& pair : pairs) {
        if (pair.left == pair.right) {
            DTheme* dtheme = themeArray[pair.left];
            addThemeToMenu(dtheme, this);
        } else {
            YMenu* subMenu = this;
            YMenuItem* subItem = nullptr;
            for (int i = pair.left; i <= pair.right; ++i) {
                DTheme* dtheme = themeArray[i];
                if (i == pair.left) {
                    subName[0] = toupper((unsigned char) dtheme->theme()[0]);
                    subMenu = new YMenu();
                    subItem = addItem(subName, -3, actionNull, subMenu);
                }
                YMenuItem* im = addThemeToMenu(dtheme, subMenu);
                if (im->isChecked()) {
                    subItem->setChecked(true);
                }
            }
        }
    }
}

bool ThemesMenu::findThemeAlternatives(
    upath path,
    mstring name,
    YMenuItem* item,
    int top)
{
    bool added = false;
    DIR* dir = opendir(path.string());
    if (dir == nullptr) {
        return false;
    }
    for (struct dirent* ent; (ent = readdir(dir)) != nullptr; ) {
        if (ent->d_name[0] == '.') {
            continue;
        }
#if DT_REG
        if (ent->d_type != DT_UNKNOWN && ent->d_type != DT_REG) {
            continue;
        }
#endif
        size_t length = strlen(ent->d_name);
        if (length <= 6 ||
            strcmp(ent->d_name + length - 6, ".theme") ||
            strcmp(ent->d_name, "default.theme") == 0 ||
            faccessat(dirfd(dir), ent->d_name, R_OK, 0)) {
            continue;
        }

        YMenu* sub = item->getSubmenu();
        if (sub == nullptr) {
            sub = new YMenu();
            item->setSubmenu(sub);
        }
        if (sub) {
            mstring label(ent->d_name, length - 6);
            mstring theme(name, "/", ent->d_name);
            YMenuItem *im = newThemeItem(label, theme, top);
            if (im) {
                im->setIcon(item->getIcon());
                sub->add(im);
                if (im->isChecked())
                    item->setChecked(true);
            }
            added = true;
        }
    }
    closedir(dir);
    return added;
}

bool ThemesMenu::createIcon(YMenuItem* item, DTheme* dtheme) {
    bool update = false;
    upath path(themesTop(dtheme->topdir()) + dtheme->theme());
    const char* buttons[] = {
        "minimizeI.xpm", "minimize.xpm",
        "minimizeI.png", "minimize.png",
        "closeI.xpm", "close.xpm",
        "closeI.png", "close.png",
    };
    for (const char* button : buttons) {
        upath file(path + button);
        ref<YImage> img(YImage::load(file));
        if (img != null) {
            ref<YImage> sm, lg, hg;
            unsigned w = img->width();
            unsigned h = max(min(w, img->height()), img->height() / 2);
            if (w >= hugeIconSize) {
                int x = (w - hugeIconSize) / 2;
                unsigned d = h > hugeIconSize ? hugeIconSize : h;
                int y = h > d ? (h - d) / 2 : 0;
                hg = img->subimage(x, y, hugeIconSize, d);
            }
            else if (w >= largeIconSize) {
                int x = (w - largeIconSize) / 2;
                unsigned d = h > largeIconSize ? largeIconSize : h;
                int y = h > d ? (h - d) / 2 : 0;
                lg = img->subimage(x, y, largeIconSize, d);
            }
            else if (w >= smallIconSize) {
                int x = (w - smallIconSize) / 2;
                unsigned d = h > smallIconSize ? smallIconSize : h;
                int y = h > d ? (h - d) / 2 : 0;
                sm = img->subimage(x, y, smallIconSize, d);
            }
            else
                sm = img->subimage(0, 0, w, h);
            ref<YIcon> icon(new YIcon(sm, lg, hg));
            item->setIcon(icon);
            update = true;
            break;
        }
    }
    return update;
}

bool ThemesMenu::updateItem(YMenuItem* item) {
    bool update = false, iconed = false;
    for (DTheme* dtheme : themeArray) {
        if (dtheme->action() == item->getAction()) {
            if (item->getIcon() == null) {
                iconed = createIcon(item, dtheme);
                upath path(themesTop(dtheme->topdir()) + dtheme->theme());
                update = findThemeAlternatives(path, dtheme->theme(),
                                               item, dtheme->topdir());
            }
            break;
        }
    }
    return (iconed | update);
}

bool ThemesMenu::handleTimer(YTimer* timer) {
    if (timer == iconTimer) {
        YMenu* menu = nullptr;
        if (iconIndex < itemCount()) {
            YMenuItem* item = getItem(iconIndex);
            if (item->getAction() == actionNull
                && (menu = item->getSubmenu()) != nullptr
                && nestIndex < menu->itemCount()) {
                item = menu->getItem(nestIndex);
            }
            if (item->getAction() != actionNull) {
                if (updateItem(item)) {
                    if (menu) {
                        if (menu->visible())
                            menu->repaintItem(nestIndex);
                    } else if (visible()) {
                        repaintItem(iconIndex);
                    }
                }
            }
        }
        if (menu == nullptr || ++nestIndex >= menu->itemCount()) {
            if (menu && nestIndex == menu->itemCount() && menu->visible()) {
                menu->sizePopup(menu->width() + 20);
            }
            ++iconIndex;
            nestIndex = 0;
        }
        if (iconIndex == itemCount()) {
            sizePopup(width() + 20);
        }
        return iconIndex < itemCount();
    }
    if (timer == cleanupTimer) {
        cleanup();
        return false;
    }
    return YMenu::handleTimer(timer);
}

// vim: set sw=4 ts=4 et:
