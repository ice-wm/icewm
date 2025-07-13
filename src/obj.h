#ifndef __OBJ_H
#define __OBJ_H

#include "mstring.h"

class IApp;
class YIcon;

class DObject {
public:
    DObject(mstring name, ref<YIcon> icon) :
        fName(name), fIcon(icon) { }
    virtual ~DObject() { }

    mstring getName() const { return fName; }
    ref<YIcon> getIcon() const { return fIcon; }

    virtual void open() = 0;

private:
    mstring fName;
    ref<YIcon> fIcon;
};

class ObjectMenu;

class ObjectContainer {
public:
    virtual void addObject(DObject *object) = 0;
    virtual void addSeparator() = 0;
    virtual void addContainer(mstring name, ref<YIcon> icon, ObjectMenu *container) = 0;
protected:
    virtual ~ObjectContainer() {}
};


#endif

// vim: set sw=4 ts=4 et:
