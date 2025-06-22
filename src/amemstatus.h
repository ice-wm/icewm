#ifndef MEMSTATUS_H
#define MEMSTATUS_H

#if __linux__

#include <unistd.h>
#include "ypointer.h"

// graphed from the bottom up:
#define MEM_USER    0
#define MEM_BUFFERS 1
#define MEM_CACHED  2
#define MEM_FREE    3
#define MEM_STATES  4

class IAppletContainer;

typedef unsigned long long membytes;

class MemInfo {
public:
    void discard() {
        if (buf) {
            delete[] buf;
            buf = nullptr;
        }
    }
    void recycle() {
        if (buf) {
            *buf = '\0';
        }
    }
    MemInfo() : fildes(-1), buf(nullptr) { }
    ~MemInfo() {
        if (fildes > 2) ::close(fildes);
        if (buf) delete[] buf;
    }
    membytes parseField(const char* field);
    bool have() { return nonempty(buf) || read(); }
    bool read();
private:
    int fildes;
    char* buf;
    ssize_t len;
    const size_t size = 2049;
};

class MEMStatus:
    public IApplet,
    private Picturer,
    private YTimerListener,
    private YActionListener
{
public:
    MEMStatus(IAppletContainer* taskBar, YWindow *aParent);
    virtual ~MEMStatus();

    virtual void actionPerformed(YAction action, unsigned modifiers);
    virtual void handleClick(const XButtonEvent &up, int count);
    virtual bool handleTimer(YTimer *t);

    void updateStatus();
    void getStatus();
    virtual void updateToolTip();

private:
    static void printAmount(char* out, size_t outSize, membytes amount);

    YMulti<membytes> samples;
    YColorName color[MEM_STATES];
    lazy<YTimer> fUpdateTimer;

    bool picture();
    void fill(Graphics& g);
    void draw(Graphics& g);

    int statusUpdateCount;
    int unchanged;
    osmart<YMenu> fMenu;
    Pixmap fBackground;
    YRect fGeometry;
    MemInfo* fMemInfo;
    IAppletContainer* taskBar;
};
#endif

#endif

// vim: set sw=4 ts=4 et:
