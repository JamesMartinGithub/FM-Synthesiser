#ifndef SYNTHUI_H
#define SYNTHUI_H

#include <QMainWindow>
#include "WaveRenderer.h"

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
    void SetController(Controller* controllerRef);
    void SetAudioData(unsigned char* data);
private:
    void InitialiseSpinBox(QSpinBox* spinBox);
    void ModulationArrowClicked(bool isA, bool isUp);

    Ui::SynthUi *ui;
    Controller* controller;
    std::map<int, std::string> modulationModes = {{0, "OFF"}, {1, "FM"}, {2, "AM"}, {3, "RM"}, {4, "SYC"}};
    int modulationMode[2] = {0};
private slots:
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
    void on_volumeFaderMaster_valueChanged(int value);
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
};

#endif // SYNTHUI_H
