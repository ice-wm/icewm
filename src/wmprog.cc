/*
 * IceWM
 *
 * Copyright (C) 1997-2002 Marko Macek
 */
#include "config.h"

#include "wmprog.h"
#include "prefs.h"
#include "wmwinmenu.h"
#include "wmapp.h"
#include "themes.h"
#include "browse.h"
#include "appnames.h"
#include "wmpref.h"
#include "wmswitch.h"
#include "intl.h"
#include <sys/stat.h>
#include <time.h>

DFile::DFile(IApp *app, mstring name, ref<YIcon> icon, upath path):
    DObject(app, name, icon), fPath(path)
{
}

DFile::~DFile() {
}

void DFile::open() {
    const char *args[] = { openCommand, fPath.string(), nullptr };
    app()->runProgram(openCommand, args);
}

RProgram::RProgram(bool restart, const char* resource,
                   const char* command, YStringArray& args)
    : fRestart(restart)
    , fRes(resource)
    , fPid(None)
    , fCmd(command)
    , fArgs(args)
{
    if (fArgs.isEmpty() || fArgs.getString(fArgs.getCount() - 1))
        fArgs.append(nullptr);
}

RProgram::~RProgram() {
    delete[] fCmd;
    delete[] fRes;
}

void RProgram::open(YSMListener* smActionListener, unsigned mods) {
    if (fRestart)
        smActionListener->restartClient(fCmd, fArgs.getCArray());
    else if (fRes)
        smActionListener->runOnce(fRes, &fPid, fCmd, fArgs.getCArray());
    else
        smActionListener->runProgram(fCmd, fArgs.getCArray(), -1);
}

DProgram::DProgram(
    IApp* app,
    YSMListener* smActionListener,
    const char* name,
    ref<YIcon> icon,
    bool restart,
    const char* wmclass,
    const char* command,
    YStringArray& args)
    : DObject(app, name, icon)
    , RProgram(restart, wmclass, command, args)
    , smActionListener(smActionListener)
{
}

void DProgram::open() {
    RProgram::open(smActionListener, None);
}

KProgramArrayType keyProgs;

KProgram::KProgram(const char* key, const char* resource,
                   const char* command, YStringArray& args)
    : RProgram(false, resource, command, args)
    , wm(newstr(key))
{
}

KProgram::~KProgram() {
    if (wm.initial == false)
        delete[] const_cast<char *>(wm.name);
}

class MenuProgSwitchItems: public ISwitchItems {
    MenuProgMenu *menu;
    int zTarget;
    const WMKey* wmkey;

public:
    MenuProgSwitchItems(RProgram* prog, const WMKey* wmkey) :
        ISwitchItems(), zTarget(0), wmkey(wmkey) {
        menu = new MenuProgMenu(wmapp, wmapp,
                                nullptr /* no wmaction handling*/,
                                "switch popup internal menu",
                                prog->cmd(), prog->args());
    }
    ~MenuProgSwitchItems() {
        delete menu;
    }
    virtual void updateList() override {
        menu->refresh();
        zTarget = 0;
    }
    virtual int getCount() override {
        return menu->itemCount();
    }
    virtual bool isEmpty() override {
        return menu->itemCount() == 0;
    }
    virtual bool isKey(const XKeyEvent& key) override {
        return *wmkey == key;
    }
    unsigned modifiers() override {
        return wmkey->mod;
    }
    virtual bool setWMClass(char* wmclass) override {
        if (wmclass) free(wmclass); // unimplemented
        return false;
    }
    virtual char* getWMClass() override {
        return nullptr;
    }

    // move the focused target up or down and return the new focused element
    virtual void moveTarget(bool zdown) override {
        int count = getCount();
        zTarget += zdown ? 1 : -1;
        if (zTarget >= count)
            zTarget = 0;
        if (zTarget < 0)
            zTarget = max(0, count - 1);
        // no further gimmicks
    }
    // move the focused target up or down and return the new focused element
    virtual void setTarget(int where) override {
        zTarget = clamp(where, 0, getCount() - 1);
    }

    // set target cursor and implementation specific stuff in the beginning
    virtual void begin(bool zdown) override {
        updateList();
        zTarget = 0;
        moveTarget(zdown);
    }

    virtual void cancelItem() override {
    }
    virtual void acceptItem() override {
        YMenuItem* item = menu->getItem(zTarget);
        if (item) {
            menu->actionPerformed(item->getAction(), 0);
        }
    }

    virtual int getActiveItem() override {
        return zTarget;
    }
    virtual mstring getTitle(int idx) override {
        return inrange(idx, 0, getCount() - 1) ?
            menu->getItem(idx)->getName() : null;
    }
    virtual ref<YIcon> getIcon(int idx) override {
        return inrange(idx, 0, getCount() - 1) ?
            menu->getItem(idx)->getIcon() : null;
    }
};

SProgram::SProgram(const char* key, const char* resource,
                   const char* command, YStringArray& args)
    : KProgram(key, resource, command, args)
    , pSwitchWindow(nullptr)
{
}

SProgram::~SProgram() {
    delete pSwitchWindow;
}

void SProgram::open(YSMListener* smActionListener, unsigned mods) {
    if (pSwitchWindow == nullptr) {
        ISwitchItems* items = new MenuProgSwitchItems(this, &wm);
        pSwitchWindow = new SwitchWindow(desktop, items, quickSwitchVertical);
    }
    pSwitchWindow->begin(true, mods);
}

MenuFileMenu::MenuFileMenu(
    IApp *app,
    YSMListener *smActionListener,
    YActionListener *wmActionListener,
    mstring name,
    YWindow *parent)
    :
    ObjectMenu(wmActionListener, parent),
    MenuLoader(app, smActionListener, wmActionListener),
    fName(name),
    fModTime(0),
    app(app)
{
}

MenuFileMenu::~MenuFileMenu() {
}

void MenuFileMenu::updatePopup() {
    if (!autoReloadMenus && fPath != null)
        return;

    upath np = app->findConfigFile(upath(fName));
    bool rel = false;


    if (fPath == null) {
        fPath = np;
        rel = true;
    } else {
        if (np == null || np.equals(fPath)) {
            fPath = np;
            rel = true;
        } else
            np = null;
    }

    if (fPath == null) {
        refresh();
    } else {
        struct stat sb;
        if (stat(fPath.string(), &sb) != 0) {
            fPath = null;
            refresh();
        } else if (sb.st_mtime > fModTime || rel) {
            fModTime = sb.st_mtime;
            refresh();
        }
    }
}

void MenuFileMenu::refresh() {
    removeAll();
    if (fPath != null)
        loadMenus(fPath, this);
}

MenuProgMenu::MenuProgMenu(
    IApp *app,
    YSMListener *smActionListener,
    YActionListener *wmActionListener,
    mstring name,
    const char* command,
    YStringArray &args,
    long timeout,
    YWindow *parent)
    :
    ObjectMenu(wmActionListener, parent),
    MenuLoader(app, smActionListener, wmActionListener),
    fName(name),
    fCommand(newstr(command)),
    fArgs(args),
    fModTime(0),
    fTimeout(timeout)
{
}

MenuProgMenu::~MenuProgMenu() {
    delete[] fCommand;
}

void MenuProgMenu::updatePopup() {
    time_t now = seconds();
    if (fModTime == 0 || (0 < fTimeout && now >= fModTime + fTimeout)) {
        refresh();
        fModTime = now;
    }
}

void MenuProgMenu::refresh()
{
    removeAll();
    if (nonempty(fCommand))
        progMenus(fCommand, fArgs.getCArray(), this);
}

StartMenu::StartMenu(
    IApp *app,
    YSMListener *smActionListener,
    YActionListener *wmActionListener,
    const char *name,
    YWindow *parent)
    : MenuFileMenu(app, smActionListener, wmActionListener, name, parent),
    smActionListener(smActionListener),
    wmActionListener(wmActionListener)
{
}

bool StartMenu::handleKey(const XKeyEvent &key) {
    // If meta key, close the popup
    if (key.type == KeyPress) {
        KeySym k = keyCodeToKeySym(key.keycode);
        int m = KEY_MODMASK(key.state);

        if ((k == xapp->Win_L ||
             k == XK_Multi_key ||
             k == xapp->Win_R) && m == 0)
        {
            cancelPopup();
            return true;
        }
    }
    return MenuFileMenu::handleKey(key);
}

void StartMenu::updatePopup() {
    MenuFileMenu::updatePopup();
}

FocusMenu::FocusMenu() {
    struct FocusModelNameAction {
        FocusModel mode;
        const char *name;
        YAction action;
    } foci[] = {
        { FocusClick, _("_Click to focus"), actionFocusClickToFocus },
        { FocusExplicit, _("_Explicit focus"), actionFocusExplicit },
        { FocusSloppy, _("_Sloppy mouse focus"), actionFocusMouseSloppy },
        { FocusStrict, _("S_trict mouse focus"), actionFocusMouseStrict },
        { FocusQuiet, _("_Quiet sloppy focus"), actionFocusQuietSloppy },
        { FocusCustom, _("Custo_m"), actionFocusCustom },
    };
    for (size_t k = 0; k < ACOUNT(foci); ++k) {
        YMenuItem *item = addItem(foci[k].name, -2, null, foci[k].action);
        if (wmapp->getFocusMode() == foci[k].mode) {
            item->setEnabled(false);
            item->setChecked(true);
        }
    }
}

HelpMenu::HelpMenu() {
    addItem(_("_Manual"), -2, null, actionHelpManual, "help");
    addItem(_("_Icewm(1)"), -2, null, actionHelpIcewm, "help");
    addItem(_("Icewm_Bg(1)"), -2, null, actionHelpIcewmbg, "help");
    addItem(_("Ice_Sound(1)"), -2, null, actionHelpIcesound, "help");
    addItem("icesh", -2, null, actionHelpIcesh, "help");
    addItem("icewm-env", -2, null, actionHelpEnv, "help");
    addItem("icewm-keys", -2, null, actionHelpKeys, "help");
    addItem("icewm-startup", -2, null, actionHelpStartup, "help");
    addItem("icewm-toolbar", -2, null, actionHelpToolbar, "help");
    addItem("icewm-winoptions", -2, null, actionHelpWinoptions, "help");
}

void StartMenu::refresh() {
    MenuFileMenu::refresh();

    addSeparator();

/// TODO #warning "make this into a menuprog (ala gnome.cc), and use mime"
    if (nonempty(openCommand)) {
        upath path[] = { upath::root(), YApplication::getHomeDir() };
        ObjectMenu* sub;
        for (const upath& p : path) {
            sub = new BrowseMenu(app, smActionListener, wmActionListener, p);
            DFile *file = new DFile(app, p, null, p);
            addObject(file, "folder", sub, false);
        }
        addSeparator();
    }

    if (showPrograms) {
        ObjectMenu *programs = new MenuFileMenu(app, smActionListener,
                                    wmActionListener, "programs", nullptr);
        ///    if (programs->itemCount() > 0)
        addSubmenu(_("Programs"), 0, programs, "programs");
    }

    if (showRun && nonempty(runDlgCommand)) {
        addItem(_("_Run..."), -2, null, actionRun, "run");
    }

    if (showWindowList) {
        addItem(_("_Windows"), -2, actionWindowList, windowListMenu, "windows");
    }

    YMenu* settings = new YMenu();

    if (!showTaskBar && showAbout) {
        settings->addItem(_("_About"), -2, actionAbout, nullptr, "about");
    }

    if (showHelp) {
        HelpMenu* help = new HelpMenu();
        settings->addSubmenu(_("_Help"), -2, help, "help");
    }

    if (showFocusModeMenu) {
        FocusMenu *focus = new FocusMenu();
        settings->addSubmenu(_("_Focus"), -2, focus, "focus");
    }

    if (showSettingsMenu) {
        PrefsMenu *prefs = new PrefsMenu();
        settings->addSubmenu(_("_Preferences"), -2, prefs, "pref");
    }

    if (showThemesMenu) {
        YMenu* themes = new ThemesMenu(app, smActionListener);
        settings->addSubmenu(_("_Themes"), -2, themes, "themes");
    }

    if (settings->itemCount()) {
        addSubmenu(_("Se_ttings"), -2, settings, "settings");
    } else {
        delete settings;
    }

    if (showLogoutMenu) {
        addSeparator();
        if (showLogoutSubMenu)
            addItem(_("_Logout..."), -2, actionLogout, logoutMenu, "logout");
        else
            addItem(_("_Logout..."), -2, null, actionLogout, "logout");
    }
}

// vim: set sw=4 ts=4 et:
