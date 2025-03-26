#ifndef SYNTHUI_H
#define SYNTHUI_H

#include <QMainWindow>
#include "WaveRenderer.h"

namespace Ui {
class SynthUi;
}
class Controller;

class SynthUi : public QMainWindow
{
    Q_OBJECT

public:
    explicit SynthUi(Controller* controllerRef, QWidget *parent = nullptr);
    ~SynthUi();
    void SetController(Controller* controllerRef);
    void SetAudioDataA(unsigned char* data);
private:
    Ui::SynthUi *ui;
    Controller* controller;
private slots:
    void on_waveformDialA_valueChanged(int value);
    void on_volumeFaderA_valueChanged(int value);
};

#endif // SYNTHUI_H
