#include "ParameterKnob.h"
#include <QPainter>
#include <QDebug>

ParameterKnob::ParameterKnob(QWidget *parent, double knobRadius, double knobMargin)
    : QDial(parent)
    , knobRadius_(knobRadius)
    , knobMargin_(knobMargin) {
}

void ParameterKnob::setKnobRadius(double radius) {
    knobRadius_ = radius;
}

double ParameterKnob::getKnobRadius() const {
    return knobRadius_;
}

void ParameterKnob::setKnobMargin(double margin) {
    knobMargin_ = margin;
}

double ParameterKnob::getKnobMargin() const {
    return knobMargin_;
}

void ParameterKnob::paintEvent(QPaintEvent *pe) {
    QPainter *painter = new QPainter(this);
    QString path = ParseStyleSheet();
    if (!path.isNull()) {
        QPixmap pixmap(path);
        int w = size().width();
        int h = size().height();
        int wSmall = w*0.75f;
        int hSmall = h*0.75f;
        double rotation;
        if (!wrapping()) {
            rotation = std::fmod((((double)value() / (double)maximum()) * 300.0) + 210.0, 360.0);
        } else {
            rotation = std::fmod((((double)value() / (double)maximum()) * 360.0) + 180.0, 360.0);
        }
        painter->translate((double)w / 2.0, (double)h / 2.0);
        painter->rotate(rotation);
        painter->translate(-(double)wSmall / 2.0, -(double)hSmall / 2.0);
        painter->drawPixmap(0,0,wSmall,hSmall, pixmap);
        painter->end();
    } else {
        qDebug() << "Invalid ParameterKnob stylesheet applied to " << objectName().toStdString();
    }
}

QString ParameterKnob::ParseStyleSheet() {
    std::string sheetString = styleSheet().toStdString();
    if (sheetString.substr(0, 6) == "image:") {
        QString path = QString::fromStdString(sheetString.substr(11, sheetString.length() - 13));
        return path;
    } else {
        NULL;
    }
}
