#ifndef FONT_MACRO_H
#define FONT_MACRO_H

#ifndef CFGDIR
#error config.h not included
#endif

#if !defined(FONTS_ADOBE) \
 && !defined(FONTS_LUCIDA) \
 && !defined(FONTS_DEJAVU) \
 && !defined(FONTS_DROID)
#define FONTS_DEJAVU 1
// #define FONTS_ADOBE 1
// #define FONTS_LUCIDA 1
// #define FONTS_DROID 1
#endif

#define FONT2(pf,px,pt)     pf #px "-" #pt "-*-*-*-*-*-*"       // "iso10646-1"

#if defined(FONTS_DEJAVU)
#define FONT(px,pt)          FONT2("-misc-dejavu sans-medium-r-normal--", px, *)
#define BOLDFONT(px,pt)        FONT2("-misc-dejavu sans-bold-r-normal--", px, *)
#define TTFONT(px,pt)   FONT2("-misc-dejavu sans mono-medium-r-normal--", px, *)
#define BOLDTTFONT(px,pt) FONT2("-misc-dejavu sans mono-bold-r-normal--", px, *)

#elif defined(FONTS_ADOBE)
#define FONT(px,pt)   FONT2("-adobe-helvetica-medium-r-normal--", px, pt)
#define BOLDFONT(px,pt) FONT2("-adobe-helvetica-bold-r-normal--", px, pt)
#define TTFONT(px,pt)   FONT2("-adobe-courier-medium-r-normal--", px, pt)
#define BOLDTTFONT(px,pt) FONT2("-adobe-courier-bold-r-normal--", px, pt)

#elif defined(FONTS_LUCIDA)
#define FONT(px,pt)     FONT2("-b&h-lucida-medium-r-normal-*-", px, pt)
#define BOLDFONT(px,pt) FONT2("-b&h-lucida-bold-r-normal-*-", px, pt)
#define TTFONT(px,pt)   FONT2("-b&h-lucidatypewriter-medium-r-normal-*-", px, pt)
#define BOLDTTFONT(px,pt) FONT2("-b&h-lucidatypewriter-bold-r-normal-*-", px, pt)

#elif defined(FONTS_DROID)
#define FONT(px,pt)     FONT2("-misc-droid sans-medium-r-normal--", px, *)
#define BOLDFONT(px,pt)   FONT2("-misc-droid sans-bold-r-normal--", px, *)
#define TTFONT(px,pt)   FONT2("-misc-droid sans mono-medium-r-normal--", px, *)
#define BOLDTTFONT(px,pt) FONT2("-misc-droid sans mono-bold-r-normal--", px, *)

#endif

#define PREF_SANS "DejaVu Sans"
#define ANY_SANS "Sans"
#define SFX_MONO " Mono:monospace"

#define FQUOT(x) #x
#define FSIZE(x) ":size=" FQUOT(x)

// defines for themeable defaults; if font not available, try to fallback to any compatible font, then try without boldness
#define FSANS   PREF_SANS FSIZE(12) "," ANY_SANS FSIZE(12)
#define FBOLD   PREF_SANS FSIZE(12) ":bold," ANY_SANS FSIZE(12) ":bold," ANY_SANS FSIZE(12)
#define FSMAL   PREF_SANS FSIZE(10) ":bold," ANY_SANS FSIZE(10) ":bold," ANY_SANS FSIZE(10)
#define FMONO   PREF_SANS SFX_MONO FSIZE(12) "," ANY_SANS SFX_MONO FSIZE(12)
#define FMONB   PREF_SANS SFX_MONO FSIZE(12) ":bold," ANY_SANS SFX_MONO FSIZE(12) FSANS ":bold," ANY_SANS SFX_MONO FSIZE(12)

#endif
