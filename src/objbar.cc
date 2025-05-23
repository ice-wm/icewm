/*
 * IceWM
 *
 * Copyright (C) 1999-2002 Marko Macek
 */
#include "config.h"
#include "objmenu.h"
#include "objbar.h"
#include "objbutton.h"
#include "wmtaskbar.h"
#include "wpixmaps.h"
#include "yprefs.h"

ObjectBar::ObjectBar(YWindow *parent): YWindow(parent) {
    setParentRelative();
}

ObjectBar::~ObjectBar() {
    ObjectButton::freeFont();
}


void ObjectBar::discard() {
    objects.clear();
    setSize(1, 1);
}

void ObjectBar::addButton(mstring name, ref<YIcon> icon, ObjectButton *button) {
    unsigned h = height();
    if (icon != null) {
        button->setIcon(icon, YIcon::smallSize());
        h = max(h, button->width() + 4);
        button->setSize(h, h);
    } else {
        button->setText(name);
        if (button->height() < h)
            button->setSize(button->width(), h);
        else if (button->height() > h)
            h = button->height();
    }

    objects.append(button);

    setSize(width() + button->width(), h);

    button->setTitle(name);
    button->realize();
}

void ObjectBar::paint(Graphics &g, const YRect& r) {
    if (taskbackPixmap == null && getGradient() == null) {
        g.setColor(taskBarBg);
        g.fillRect(r.x(), r.y(), r.width(), r.height());
    }
}

void ObjectBar::addObject(DObject *object) {
    ObjectButton *button = new ObjectButton(this, object);
    addButton(object->getName(), object->getIcon(), button);
}

void ObjectBar::addSeparator() {
    setSize(int(width()) <= 1 ? 4 : width() + 4, height());
    objects.append(nullptr);
}

void ObjectBar::addContainer(mstring name, ref<YIcon> icon, ObjectMenu *container) {
    if (container) {
        ObjectButton *button = new ObjectButton(this, container);
        addButton(name, icon, button);
    }
}

void ObjectBar::configure(const YRect2& r) {
    if (r.resized() && 1 < r.height()) {
        int left = 0;
        for (ObjectButton* obj : objects) {
            if (obj) {
                int x = rightToLeft ? r.width() - 1 - obj->width() - left : left;
                if (x != obj->x() || obj->y()) {
                    obj->setPosition(x, 0);
                }
                if (obj->height() < r.height()) {
                    obj->setSize(obj->width(), r.height());
                }
                left += obj->width();
            } else
                left += 4;
        }
    }
}

void ObjectBar::handleExpose(const XExposeEvent& e) {
    if (taskbackPixmap != null || getGradient() != null) {
        clearArea(e.x, e.y, e.width, e.height);
    }
}

void ObjectBar::handleButton(const XButtonEvent& up) {
    parent()->handleButton(up);
}

void ObjectBar::refresh() {
    for (ObjectButton* obj : objects) {
        if (obj) {
            obj->repaint();
        }
    }
}

// vim: set sw=4 ts=4 et:
