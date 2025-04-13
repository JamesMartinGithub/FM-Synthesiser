#ifndef SYNTH_PARAMETERKNOB_H
#define SYNTH_PARAMETERKNOB_H

#include <QtWidgets/QDial>
#include <string>

class ParameterKnob : public QDial {
    Q_OBJECT
    Q_PROPERTY(double knobRadius READ getKnobRadius WRITE setKnobRadius)
    Q_PROPERTY(double knobMargin READ getKnobMargin WRITE setKnobMargin)
public:
    ParameterKnob(QWidget *parent = nullptr, double knobRadius = 5, double knobMargin = 5);
    void setKnobRadius(double radius);
    double getKnobRadius() const;
    void setKnobMargin(double margin);
    double getKnobMargin() const;
private:
    virtual void paintEvent(QPaintEvent *pe) override;
    QString ParseStyleSheet();
    double knobRadius_;
    double knobMargin_;
};


#endif //SYNTH_PARAMETERKNOB_H
