#ifndef WMACTION_H
#define WMACTION_H

class YAction;

enum WMAction {
    ICEWM_ACTION_NOP = 0,
    ICEWM_ACTION_LOCK = 1,
    ICEWM_ACTION_LOGOUT = 2,
    ICEWM_ACTION_CANCEL_LOGOUT = 3,
    ICEWM_ACTION_REBOOT = 4,
    ICEWM_ACTION_SHUTDOWN = 5,
    ICEWM_ACTION_ABOUT = 6,
    ICEWM_ACTION_WINDOWLIST = 7,
    ICEWM_ACTION_RESTARTWM = 8,
    ICEWM_ACTION_SUSPEND = 9,
    ICEWM_ACTION_WINOPTIONS = 10,
    ICEWM_ACTION_RELOADKEYS = 11,
    ICEWM_ACTION_ICEWMBG = 12,
    ICEWM_ACTION_REFRESH = 13,
    ICEWM_ACTION_HIBERNATE = 14,
    ICEWM_ACTION_TOOLBAR = 15,
    LAST_ICEWM_ACTION = 15,
};

enum RebootShutdown {
    Logout = 0,
    Reboot = 1,
    Shutdown = 2,
};

enum EAction {
    actionNull               = 0,

    actionLock               = ICEWM_ACTION_LOCK,
    actionLogout             = ICEWM_ACTION_LOGOUT,
    actionCancelLogout       = ICEWM_ACTION_CANCEL_LOGOUT,
    actionReboot             = ICEWM_ACTION_REBOOT,
    actionShutdown           = ICEWM_ACTION_SHUTDOWN,
    actionAbout              = ICEWM_ACTION_ABOUT,
    actionWindowList         = ICEWM_ACTION_WINDOWLIST,
    actionRestart            = ICEWM_ACTION_RESTARTWM,
    actionSuspend            = ICEWM_ACTION_SUSPEND,
    actionWinOptions         = ICEWM_ACTION_WINOPTIONS,
    actionReloadKeys         = ICEWM_ACTION_RELOADKEYS,
    actionIcewmbg            = ICEWM_ACTION_ICEWMBG,
    actionRefresh            = ICEWM_ACTION_REFRESH,
    actionHibernate          = ICEWM_ACTION_HIBERNATE,
    actionToolbar            = ICEWM_ACTION_TOOLBAR,

    actionCascade            = 101,
    actionArrange            = 103,
    actionTileVertical       = 105,
    actionTileHorizontal     = 107,
    actionUndoArrange        = 109,
    actionArrangeIcons       = 111,
    actionMinimizeAll        = 113,
    actionHideAll            = 115,
    actionShowDesktop        = 117,

    actionHide               = 119,
    actionShow               = 121,
    actionRaise              = 123,
    actionLower              = 125,
    actionDepth              = 127,
    actionMove               = 129,
    actionSize               = 131,
    actionMaximize           = 133,
    actionMaximizeVert       = 135,
    actionMinimize           = 137,
    actionRestore            = 139,
    actionRollup             = 141,
    actionClose              = 143,
    actionKill               = 145,
    actionOccupyAllOrCurrent = 147,
    actionFullscreen         = 151,
    actionToggleTray         = 153,
    actionCollapseTaskbar    = 155,

    actionRestartXterm       = 169,
    actionRun                = 179,
    actionExit               = 181,

    actionFocusClickToFocus  = 183,
    actionFocusExplicit      = 185,
    actionFocusMouseSloppy   = 187,
    actionFocusMouseStrict   = 189,
    actionFocusQuietSloppy   = 191,
    actionFocusCustom        = 193,

    actionMaximizeHoriz      = 195,
    actionCut                = 201,
    actionCopy               = 203,
    actionPaste              = 205,
    actionSelectAll          = 207,
    actionPasteSelection     = 209,

    actionLayerDesktop       = 211,
    actionLayerOne           = 213,
    actionLayerBelow         = 215,
    actionLayerThree         = 217,
    actionLayerNormal        = 219,
    actionLayerFive          = 221,
    actionLayerOnTop         = 223,
    actionLayerSeven         = 225,
    actionLayerDock          = 227,
    actionLayerNine          = 229,
    actionLayerAboveDock     = 231,
    actionLayerEleven        = 233,
    actionLayerMenu          = 235,
    actionLayerThirteen      = 237,
    actionLayerFullscreen    = 239,
    actionLayerAboveAll      = 241,

    actionAboutClose         = 243,
    actionTileLeft           = 245,
    actionTileRight          = 246,
    actionTileTop            = 247,
    actionTileBottom         = 248,
    actionTileTopLeft        = 249,
    actionTileTopRight       = 250,
    actionTileBottomLeft     = 251,
    actionTileBottomRight    = 252,
    actionTileCenter         = 253,
    actionUntab              = 255,
    actionRename             = 257,
    actionSysDialog          = 259,

    actionHelpManual         = 267,
    actionHelpIcewm          = 269,
    actionHelpIcewmbg        = 271,
    actionHelpIcesound       = 273,
    actionHelpIcesh          = 275,
    actionHelpEnv            = 277,
    actionHelpKeys           = 279,
    actionHelpStartup        = 281,
    actionHelpToolbar        = 283,
    actionHelpWinoptions     = 285,

};

bool canShutdown(RebootShutdown reboot);
bool canSuspend();
bool canHibernate();
bool canLock();
/**
 * Basic check whether a shell command could possibly be run.
 */
bool couldRunCommand(const char *cmd);

#endif

// vim: set sw=4 ts=4 et:
