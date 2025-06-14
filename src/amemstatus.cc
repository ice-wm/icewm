/*
 * IceWM
 *
 * Copyright (C) 1998-2015 Marko Macek, Matthew Ogilvie
 *
 * Memory Status
 */
#include "config.h"
#include "ylib.h"
#include "wmapp.h"
#include "applet.h"
#include "amemstatus.h"

#if MEM_STATES

#include "ymenuitem.h"
#include "sysdep.h"
#include "default.h"
#include "ytimer.h"
#include "intl.h"

extern ref<YPixmap> taskbackPixmap;

bool MemInfo::read() {
    const char path[] = "/proc/meminfo";

    if (fildes >= 0 && lseek(fildes, 0, SEEK_SET) < 0) {
        if (ONCE)
            fail("seek %s", path);
        close(fildes);
        fildes = -1;
    }
    if (fildes < 0 && (fildes = open(path, O_RDONLY | O_CLOEXEC)) < 0) {
        if (ONCE)
            fail("open %s", path);
        return false;
    }

    if (buf == nullptr) {
        buf = new char[size];
        *buf = '\0';
    }

    len = ::read(fildes, buf, size - 1);
    if (len <= 0) {
        if (ONCE)
            fail("read %s", path);
        close(fildes);
        fildes = -1;
        delete[] buf;
        buf = nullptr;
    } else {
        buf[len] = '\0';
    }
    return buf && len;
}

MEMStatus::MEMStatus(IAppletContainer* taskBar, YWindow *aParent):
    IApplet(this, aParent),
    samples(taskBarMEMSamples, MEM_STATES),
    statusUpdateCount(0),
    unchanged(taskBarMEMSamples),
    fBackground(None),
    fMemInfo(new MemInfo),
    taskBar(taskBar)
{
    fUpdateTimer->setTimer(taskBarMEMDelay, this, true);

    color[MEM_USER] = &clrMemUser;
    color[MEM_BUFFERS] = &clrMemBuffers;
    color[MEM_CACHED] = &clrMemCached;
    color[MEM_FREE] = &clrMemFree;

    samples.clear();
    for (int i = 0; i < taskBarMEMSamples; i++) {
        samples[i][MEM_FREE] = 1;
    }
    setSize(taskBarMEMSamples, taskBarGraphHeight);
    getStatus();
    updateStatus();
    setTitle("MEM");
    show();
}

MEMStatus::~MEMStatus() {
    if (fBackground) {
        XFreePixmap(xapp->display(), fBackground);
    }
    delete fMemInfo;
}

bool MEMStatus::picture() {
    bool create = (hasPixmap() == false);

    for (int i = 0; i < MEM_STATES; ++i) {
        if (color[i] == false) {
            if (fGeometry != geometry()) {
                if (fBackground) {
                    XFreePixmap(xapp->display(), fBackground);
                }
                fBackground = createPixmap();
                fGeometry = geometry();

                Graphics gfx(fBackground, width(), height(), depth());
                ref<YImage> gradient(getGradient());
                if (gradient != null)
                    gfx.drawImage(gradient,
                                  x(), y(), width(), height(), 0, 0);
                else if (taskbackPixmap != null)
                    gfx.fillPixmap(taskbackPixmap,
                                   0, 0, width(), height(), x(), y());
                else
                    gfx.clear();
            }
            break;
        }
    }

    Graphics G(getPixmap(), width(), height(), depth());

    if (create)
        fill(G);

    return (statusUpdateCount && unchanged < taskBarMEMSamples)
        ? draw(G), true : create;
}

void MEMStatus::fill(Graphics& g) {
    if (color[MEM_FREE]) {
        g.setColor(color[MEM_FREE]);
        g.fillRect(0, 0, width(), height());
    } else {
        g.copyDrawable(fBackground, 0, 0, width(), height(), 0, 0);
    }
}

void MEMStatus::draw(Graphics& g) {
    int h = height();
    int first = max(0, taskBarMEMSamples - statusUpdateCount);
    if (0 < first && first < taskBarMEMSamples)
        g.copyArea(taskBarMEMSamples - first, 0, first, h, 0, 0);
    const int limit = (statusUpdateCount <= (1 + unchanged) / 2)
                    ? taskBarMEMSamples - statusUpdateCount : taskBarMEMSamples;
    statusUpdateCount = 0;

    for (int i = first; i < limit; i++) {
        membytes total = samples.sum(i);

        int y = h;
        for (int j = 0; j < MEM_STATES; j++) {
            int bar;
            if (j == MEM_STATES - 1) {
                bar = y;
            } else {
                bar = (int)((h * samples[i][j]) / total);
            }

            if (bar <= 0) {
                continue;
            }

            if (color[j]) {
                g.setColor(color[j]);
                g.drawLine(i, y - 1, i, y - bar);
            } else {
                g.copyDrawable(fBackground,
                               i, y - bar, width() - i, bar, i, y - bar);
            }
            y -= bar;
        }
    }
}

bool MEMStatus::handleTimer(YTimer *t) {
    if (t != fUpdateTimer)
        return false;
    updateStatus();
    if (toolTipVisible())
        updateToolTip();
    return true;
}

void MEMStatus::printAmount(char *out, size_t outSize,
                            membytes amount) {
    if (amount >> 30 >= 200) {
        snprintf( out, outSize, "%llu %.20s",
                  amount >> 30,
                  _("GB") );
    }
    else if (amount >> 20 >= 200) {
        snprintf( out, outSize, "%llu %.20s",
                  amount >> 20,
                  _("MB") );
    }
    else if (amount >> 10 >= 200) {
        snprintf( out, outSize, "%llu %.20s",
                  amount >> 10,
                  _("kB") );
    } else {
        snprintf( out, outSize, "%llu %.20s",
                  amount,
                  _("bytes") );
    }
    out[outSize - 1] = '\0';
}

void MEMStatus::updateToolTip() {
    membytes* cur = samples[taskBarMEMSamples - 1];
    membytes total = samples.sum(taskBarMEMSamples - 1);

    char totalStr[64];
    printAmount(totalStr, sizeof(totalStr), total);
    char freeStr[64];
    printAmount(freeStr, sizeof(freeStr), cur[MEM_FREE]);
    char userStr[64];
    printAmount(userStr, sizeof(userStr), cur[MEM_USER]);
    char buffersStr[64];
    printAmount(buffersStr, sizeof(buffersStr), cur[MEM_BUFFERS]);
    char cachedStr[64];
    printAmount(cachedStr, sizeof(cachedStr), cur[MEM_CACHED]);

    char *memmsg = cstrJoin(_("Memory Total: "), totalStr, "\n   ",
                            _("Free: "), freeStr, "\n   ",
                            _("Cached: "), cachedStr, "\n   ",
                            _("Buffers: "), buffersStr, "\n   ",
                            _("User: "), userStr,
                            NULL );
    setToolTip(memmsg);
    delete [] memmsg;
}

void MEMStatus::updateStatus() {
    samples.shiftLeft();
    getStatus();
    repaint();
}

membytes MemInfo::parseField(const char* needle) {
    ptrdiff_t needleLen = strlen(needle);
    for (const char* str(buf); str && (str = strstr(str, needle)) != nullptr; ) {
        if (str == buf || str[-1] == '\n') {
            char* endptr = nullptr;
            membytes result = strtoull(str + needleLen, &endptr, 10);

            while (*endptr != 0 && *endptr == ' ')
                endptr++;

            if (*endptr == 'k') {   // normal case
                result *= 1024;
            } else if (*endptr == 'M') {
                result *= 1024*1024;
            } else if (*endptr == 'G') {
                result *= 1024*1024*1024;
            }
            return result;
        }
        str = strchr(str + needleLen, '\n');
    }
    return 0;
}

void MEMStatus::getStatus() {
    membytes *cur = samples[taskBarMEMSamples - 1];
    samples.clear(taskBarMEMSamples - 1);
    cur[MEM_FREE] = 1;

    fMemInfo->recycle();
    if (fMemInfo->read()) {
        cur[MEM_BUFFERS] = fMemInfo->parseField("Buffers:");
        cur[MEM_CACHED] = fMemInfo->parseField("Cached:");
        cur[MEM_FREE] = fMemInfo->parseField("MemFree:");
        membytes total = fMemInfo->parseField("MemTotal:");
        if (total < 1)
            total = 1;
        if (total >= cur[MEM_BUFFERS] + cur[MEM_CACHED] + cur[MEM_FREE])
           cur[MEM_USER] = cur[MEM_BUFFERS] + cur[MEM_CACHED] + cur[MEM_FREE];
        else
           cur[MEM_USER] = 0;
    }
    fMemInfo->discard();

    ++statusUpdateCount;
    int last = taskBarMEMSamples - 1;
    bool same = 0 < last && 0 == samples.compare(last, last - 1);
    unchanged = same ? 1 + unchanged : 0;
}

void MEMStatus::actionPerformed(YAction action, unsigned modifiers) {
    if (action == actionClose) {
        hide();
        taskBar->relayout();
    }
    fMenu = nullptr;
}

void MEMStatus::handleClick(const XButtonEvent &up, int count) {
    if (up.button == Button3) {
        fMenu = new YMenu();
        fMenu->setActionListener(this);
        fMenu->addItem(_("MEM"), -2, null, actionNull)->setEnabled(false);
        fMenu->addSeparator();
        fMenu->addItem(_("_Disable"), -2, null, actionClose);
        fMenu->popup(nullptr, nullptr, nullptr, up.x_root, up.y_root,
                     YPopupWindow::pfCanFlipVertical |
                     YPopupWindow::pfCanFlipHorizontal |
                     YPopupWindow::pfPopupMenu);
    }
}

#endif

// vim: set sw=4 ts=4 et:
