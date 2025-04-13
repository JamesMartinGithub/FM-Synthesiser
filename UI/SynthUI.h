#ifndef SYNTHUI_H
#define SYNTHUI_H

#include <QMainWindow>
#include "WaveRenderer.h"
#include "ParameterKnob.h"

namespace Ui {
class SynthUi;
}
class Controller;
class QSpinBox;

class SynthUi : public QMainWindow
{
    Q_OBJECT

public:
    explicit SynthUi(Controller* controllerRef, QWidget *parent = nullptr);
    ~SynthUi();
    void SetAudioData(unsigned char* data);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
private:
    void InitialiseSpinBox(QSpinBox* spinBox);
    void ModulationArrowClicked(bool isA, bool isUp);
    void FilterArrowClicked(bool isUp);
    char KeyToChar(int key);
    QString TimeToText(float time);
    QString PercentToText(float percent);
    QString PanToText(int pan);
    QString FrequencyToText(float frequency);

    Ui::SynthUi *ui;
    Controller* controller;
    std::map<int, std::string> modulationModes = {{0, "OFF"}, {1, "FM"}, {2, "AM"}, {3, "RM"}, {4, "SYC"}, {5, "SYW"}, {6, "QTZ"}};
    int modulationMode[2] = {0};
    std::map<int, std::string> filterModes = {{0, "LP"}, {1, "HP"}, {2, "BP"}};
    int filterMode = 0;
private slots:
    void on_volumeFaderMaster_valueChanged(int value);
    void on_adsrAttackDial_valueChanged(int value);
    void on_adsrDecayDial_valueChanged(int value);
    void on_adsrSustainDial_valueChanged(int value);
    void on_adsrReleaseDial_valueChanged(int value);
    void on_waveformDialA_valueChanged(int value);
    void on_volumeFaderA_valueChanged(int value);
    void on_unisonSelectorA_valueChanged(int arg1);
    void on_detuneDialA_valueChanged(int value);
    void on_unisonWidthDialA_valueChanged(int value);
    void on_volumeFaderB_valueChanged(int value);
    void on_unisonSelectorB_valueChanged(int arg1);
    void on_detuneDialB_valueChanged(int value);
    void on_unisonWidthDialB_valueChanged(int value);
    void on_waveformDialB_valueChanged(int value);
    void on_modulationUpA_clicked();
    void on_modulationDownA_clicked();
    void on_modulationDialA_valueChanged(int value);
    void on_modulationUpB_clicked();
    void on_modulationDownB_clicked();
    void on_modulationDialB_valueChanged(int value);
    void on_octaveSelectorA_valueChanged(int arg1);
    void on_semitoneSelectorA_valueChanged(int arg1);
    void on_octaveSelectorB_valueChanged(int arg1);
    void on_semitoneSelectorB_valueChanged(int arg1);
    void on_panDialA_valueChanged(int value);
    void on_panDialB_valueChanged(int value);
    void on_filterUp_clicked();
    void on_filterDown_clicked();
    void on_filterToggleA_toggled(bool checked);
    void on_filterToggleB_toggled(bool checked);
    void on_filterFrequencyDial_valueChanged(int value);
    void on_filterResonanceDial_valueChanged(int value);
};

#endif // SYNTHUI_H
