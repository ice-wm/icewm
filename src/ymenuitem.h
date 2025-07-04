#ifndef YMENUITEM_H
#define YMENUITEM_H

#include "yicon.h"
#include "yaction.h"

class YMenu;

class YMenuItem {
public:
    YMenuItem(const mstring &name, int hotCharPos, const mstring &param, YAction action, YMenu *submenu);
    YMenuItem(const mstring &name);
    YMenuItem();
    ~YMenuItem();

    mstring& getName() { return fName; }
    mstring& getParam() { return fParam; }
    YAction getAction() const { return fAction; }
    YMenu *getSubmenu() const { return fSubmenu; }
    bool haveName() const { return fName.nonempty(); }
    bool haveName(const mstring& name) const { return fName.equals(name); }

    int getHotChar() const {
        return (fName != null && fHotCharPos >= 0) ? fName.charAt(fHotCharPos) : -1;
    }

    int getHotCharPos() const { return fHotCharPos; }

    ref<YIcon> getIcon() const { return fIcon; }
    void setChecked(bool c) { fChecked = c; }
    int isChecked() const { return fChecked; }
    int isEnabled() const { return fEnabled; }
    void setEnabled(bool e) { fEnabled = e; }
    void setSubmenu(YMenu *submenu) { fSubmenu = submenu; }

    int queryHeight(int &top, int &bottom, int &pad);

    int getIconWidth() const;
    int getNameWidth();
    int getParamWidth();

    bool isSeparator() { return getName() == null && getSubmenu() == nullptr; }

    void setIcon(ref<YIcon> icon) { fIcon = icon; }
private:
    mstring fName;
    mstring fParam;
    YAction fAction;
    int fHotCharPos;
    YMenu *fSubmenu;
    ref<YIcon> fIcon;
    bool fChecked;
    bool fEnabled;
};

#endif

// vim: set sw=4 ts=4 et:
