#ifndef WMDIALOG_H
#define WMDIALOG_H

#include "ywindow.h"
#include "yaction.h"

class YActionButton;
class IApp;

class CtrlAltDelete: public YWindow, public YActionListener {
public:
    CtrlAltDelete(IApp *app, YWindow *parent);
    virtual ~CtrlAltDelete();

    void paint(Graphics& g, const YRect& r) override;
    bool handleKey(const XKeyEvent& key) override;
    void actionPerformed(YAction action, unsigned) override;
    void configure(const YRect2& rect) override;
    void repaint() override;
    void handleVisibility(const XVisibilityEvent&) override;
    void handleButton(const XButtonEvent& button) override;

    void activate();
    void deactivate();

private:
    int indexFocus();

    IApp *app;
    enum { Count = 13, };
    YActionButton* buttons[Count];
};

#endif

// vim: set sw=4 ts=4 et:
