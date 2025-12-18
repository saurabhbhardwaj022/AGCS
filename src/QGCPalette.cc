/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


/// @file
///     @author Don Gagne <don@thegagnes.com>

#include "QGCPalette.h"
#include "QGCApplication.h"
#include "QGCCorePlugin.h"

#include <QApplication>
#include <QPalette>

QList<QGCPalette*>   QGCPalette::_paletteObjects;

QGCPalette::Theme QGCPalette::_theme = QGCPalette::Light;

QMap<int, QMap<int, QMap<QString, QColor>>> QGCPalette::_colorInfoMap;

QStringList QGCPalette::_colors;

QGCPalette::QGCPalette(QObject* parent) :
    QObject(parent),
    _colorGroupEnabled(true)
{
    if (_colorInfoMap.isEmpty()) {
        _buildMap();
    }

    // We have to keep track of all QGCPalette objects in the system so we can signal theme change to all of them
    _paletteObjects += this;
}

QGCPalette::~QGCPalette()
{
    bool fSuccess = _paletteObjects.removeOne(this);
    if (!fSuccess) {
        qWarning() << "Internal error";
    }
}

void QGCPalette::_buildMap()
{
    // Light Theme: AVPL style - bright, orange/blue accents
    DECLARE_QGC_COLOR(window,               "#f4f7fb", "#ffffff", "#1A1D24", "#1A1D24")
    DECLARE_QGC_COLOR(windowShadeLight,     "#c0c0c0", "#e6e6e6", "#2A2E38", "#2A2E38")
    DECLARE_QGC_COLOR(windowShade,          "#d9d9d9", "#d9d9d9", "#23272F", "#23272F")
    DECLARE_QGC_COLOR(windowShadeDark,      "#C9C9C9", "#C9C9C9", "#1F1F28", "#1F1F28")
    DECLARE_QGC_COLOR(text,                 "#333333", "#000000", "#B0C4DE", "#FFFFFF")
    DECLARE_QGC_COLOR(warningText,          "#cc0808", "#cc0808", "#FF4136", "#FF4136")
    DECLARE_QGC_COLOR(button,               "#ffffff", "#e6f7ff", "#003C70", "#003C70")
    DECLARE_QGC_COLOR(buttonText,           "#000000", "#000000", "#FFFFFF", "#FFFFFF")
    DECLARE_QGC_COLOR(buttonHighlight,      "#ffcc99", "#FF851B", "#FF851B", "#FF851B")
    DECLARE_QGC_COLOR(buttonHighlightText,  "#000000", "#000000", "#000000", "#000000")
    DECLARE_QGC_COLOR(primaryButton,        "#FF4136", "#FF4136", "#FF4136", "#FF4136")
    DECLARE_QGC_COLOR(primaryButtonText,    "#ffffff", "#ffffff", "#000000", "#000000")
    DECLARE_QGC_COLOR(textField,            "#ffffff", "#ffffff", "#002244", "#001122")
    DECLARE_QGC_COLOR(textFieldText,        "#000000", "#000000", "#FFFFFF", "#FFFFFF")
    DECLARE_QGC_COLOR(mapButton,            "#0074D9", "#0074D9", "#0074D9", "#0074D9")
    DECLARE_QGC_COLOR(mapButtonHighlight,   "#FF851B", "#FF851B", "#FF851B", "#FF851B")
    DECLARE_QGC_COLOR(mapIndicator,         "#FFDC00", "#FFDC00", "#FFDC00", "#FFDC00")
    DECLARE_QGC_COLOR(mapIndicatorChild,    "#AAAAAA", "#AAAAAA", "#AAAAAA", "#AAAAAA")
    DECLARE_QGC_COLOR(colorGreen,           "#2ECC40", "#2ECC40", "#2ECC40", "#2ECC40")
    DECLARE_QGC_COLOR(colorOrange,          "#FF851B", "#FF851B", "#FF851B", "#FF851B")
    DECLARE_QGC_COLOR(colorRed,             "#FF4136", "#FF4136", "#FF4136", "#FF4136")
    DECLARE_QGC_COLOR(colorGrey,            "#AAAAAA", "#AAAAAA", "#AAAAAA", "#AAAAAA")
    DECLARE_QGC_COLOR(colorBlue,            "#0074D9", "#0074D9", "#0074D9", "#0074D9")
    DECLARE_QGC_COLOR(alertBackground,      "#FFDC00", "#FFDC00", "#FFDC00", "#FFDC00")
    DECLARE_QGC_COLOR(alertBorder,          "#AAAAAA", "#AAAAAA", "#AAAAAA", "#AAAAAA")
    DECLARE_QGC_COLOR(alertText,            "#000000", "#000000", "#000000", "#000000")
    DECLARE_QGC_COLOR(missionItemEditor,    "#FF557F", "#FF557F", "#B00058", "#B00058")
    DECLARE_QGC_COLOR(toolStripHoverColor,  "#FF4136", "#FF4136", "#FF4136", "#FF4136")
    DECLARE_QGC_COLOR(statusFailedText,     "#000000", "#000000", "#FF4136", "#FF4136")
    DECLARE_QGC_COLOR(statusPassedText,     "#000000", "#000000", "#2ECC40", "#2ECC40")
    DECLARE_QGC_COLOR(statusPendingText,    "#000000", "#000000", "#FFDC00", "#FFDC00")
    DECLARE_QGC_COLOR(toolbarBackground,    "#ffffff", "#ffffff", "#111111", "#111111")


    // Colors not affecting by theming
    DECLARE_QGC_NONTHEMED_COLOR(brandingPurple,     "#00FF00", "#00AA00")
    DECLARE_QGC_NONTHEMED_COLOR(brandingBlue,       "#0074D9", "#0074D9")
    DECLARE_QGC_NONTHEMED_COLOR(toolStripFGColor,   "#707070", "#ffffff")

    // Colors not affecting by theming or enable/disable
    DECLARE_QGC_SINGLE_COLOR(mapWidgetBorderLight,          "#6C757D")
    DECLARE_QGC_SINGLE_COLOR(mapWidgetBorderDark,           "#6C757D")
    DECLARE_QGC_SINGLE_COLOR(mapMissionTrajectory,          "#FF851B")
    DECLARE_QGC_SINGLE_COLOR(surveyPolygonInterior,         "#2ECC40")
    DECLARE_QGC_SINGLE_COLOR(surveyPolygonTerrainCollision, "#FF4136")
}


void QGCPalette::setColorGroupEnabled(bool enabled)
{
    _colorGroupEnabled = enabled;
    emit paletteChanged();
}

void QGCPalette::setGlobalTheme(Theme newTheme)
{
    // Mobile build does not have themes
    if (_theme != newTheme) {
        _theme = newTheme;
        _signalPaletteChangeToAll();
    }
}

void QGCPalette::_signalPaletteChangeToAll()
{
    // Notify all objects of the new theme
    foreach (QGCPalette* palette, _paletteObjects) {
        palette->_signalPaletteChanged();
    }
}

void QGCPalette::_signalPaletteChanged()
{
    emit paletteChanged();
}
