// Copyright 2008-2014 JavaTB Team. Licensed under Apache License, Version 2.0.
#include "Theme.hpp"
#include <QPalette>

namespace javatb {

const Theme Theme::DEFAULT {
    "Default",
    QColor(), QColor(), QColor(), QColor() // use system defaults
};

const Theme Theme::ORANGE {
    "Orange",
    QColor(0xFF, 0xCC, 0x80), QColor(Qt::black),
    QColor(0xFF, 0xE0, 0xB2), QColor(Qt::black)
};

const Theme Theme::BLUE {
    "Blue",
    QColor(0xBB, 0xDE, 0xFB), QColor(Qt::black),
    QColor(0xE3, 0xF2, 0xFD), QColor(Qt::black)
};

const Theme Theme::GREEN {
    "Green",
    QColor(0xA5, 0xD6, 0xA7), QColor(Qt::black),
    QColor(0xE8, 0xF5, 0xE9), QColor(Qt::black)
};

const Theme Theme::GIRLIE {
    "Girlie",
    QColor(0xFF, 0xCE, 0xD9), QColor(Qt::black),
    QColor(0xFF, 0xF0, 0xF5), QColor(Qt::black)
};

const Theme Theme::DESERT {
    "Desert",
    QColor(0xD2, 0xB4, 0x8C), QColor(Qt::black),
    QColor(0xF5, 0xDE, 0xB3), QColor(Qt::black)
};

void Theme::applyTo(QWidget* w) const {
    if (!w) return;
    // If this is the DEFAULT theme, leave the widget's palette untouched.
    if (name == "Default") return;

    QPalette pal = w->palette();
    if (containerBackground.isValid())
        pal.setColor(QPalette::Window, containerBackground);
    if (containerForeground.isValid())
        pal.setColor(QPalette::WindowText, containerForeground);
    if (componentBackground.isValid()) {
        pal.setColor(QPalette::Base,   componentBackground);
        pal.setColor(QPalette::Button, componentBackground);
    }
    if (componentForeground.isValid()) {
        pal.setColor(QPalette::Text,       componentForeground);
        pal.setColor(QPalette::ButtonText, componentForeground);
    }
    w->setPalette(pal);
    w->setAutoFillBackground(true);
}

} // namespace javatb
