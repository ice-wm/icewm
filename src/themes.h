#ifndef THEMES_H
#define THEMES_H

#include "ymenu.h"
#include "yaction.h"

class IApp;
class YSMListener;

class DTheme {
public:
    DTheme(mstring theme, int top) : fTheme(theme), fTop(top) { }
    mstring theme() const { return fTheme; }
    int topdir() const { return fTop; }
    YAction action() const { return fAction; }
private:
    mstring fTheme;
    int fTop;
    YAction fAction;
};

class ThemesMenu: public YMenu, private YActionListener {
public:
    ThemesMenu(IApp *app, YSMListener *smListener);
    virtual ~ThemesMenu();

private:
    void updatePopup() override;
    void deactivatePopup() override;
    void actionPerformed(YAction action, unsigned modifiers) override;
    void cleanup();
    upath themesTop(int top);
    void scanThemes(int top);
    void makeMenus();
    void open(DTheme* dtheme);

    YMenuItem* newThemeItem(mstring label, mstring theme, int top);
    YMenuItem* addThemeToMenu(DTheme* dtheme, YMenu* menu);

    bool findThemeAlternatives(upath path, mstring name,
                               YMenuItem* item, int top);

    bool handleTimer(YTimer* timer) override;
    bool updateItem(YMenuItem* item);
    bool createIcon(YMenuItem* item, DTheme* dtheme);

    IApp* app;
    YSMListener* smActionListener;
    int iconIndex;
    int nestIndex;
    lazy<YTimer> iconTimer;
    lazy<YTimer> cleanupTimer;
    typedef DTheme* ArrayElmt;
    typedef YArray<ArrayElmt> ArrayType;
    ArrayType themeArray, alternatives;

    upath libThemes, cfgThemes, prvThemes;
};

#endif

// vim: set sw=4 ts=4 et:
