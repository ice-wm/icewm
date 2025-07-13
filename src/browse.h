#ifndef BROWSE_H
#define BROWSE_H

class YSMListener;

class BrowseMenu: public YMenu, private YActionListener {
public:
    BrowseMenu(IApp *app, upath path);
    ~BrowseMenu();

    void updatePopup() override;

private:
    void actionPerformed(YAction action, unsigned modifiers = 0) override;
    void loadItems();

    IApp *app;
    upath fPath;
    time_t fModTime;

    typedef struct {
        YAction action;
        const char* name;
    } ActionName;
    YArray<ActionName> names;
};

#endif

// vim: set sw=4 ts=4 et:
