// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
// C++ port of org.javatb.search.ui.Theme
#pragma once

#include <QColor>
#include <QPalette>
#include <QString>
#include <QWidget>

namespace javatb {

/// Color theme definition for the UI.
struct Theme {
    QString name;
    QColor  containerBackground;
    QColor  containerForeground;
    QColor  componentBackground;
    QColor  componentForeground;

    // ── Predefined themes ────────────────────────────────────────────────────
    static const Theme DEFAULT;
    static const Theme ORANGE;
    static const Theme BLUE;
    static const Theme GREEN;
    static const Theme GIRLIE;
    static const Theme DESERT;

    /// Apply this theme's colors to a widget's palette.
    void applyTo(QWidget* w) const;
};

} // namespace javatb
