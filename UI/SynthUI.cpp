#include "SynthUI.h"
#include "ui_SynthUI.h"
#include "Controller.h"

SynthUi::SynthUi(Controller* controllerRef, QWidget *parent)
    : controller(controllerRef)
    , QMainWindow(parent)
    , ui(new Ui::SynthUi) {
    ui->setupUi(this);
}

SynthUi::~SynthUi() {
    delete ui;
}

void SynthUi::SetController(Controller* controllerRef) {
    controller = controllerRef;
}

void SynthUi::SetAudioDataA(unsigned char *data) {
    ui->waveformRendererA->SetAudioData(data);
}

void SynthUi::on_waveformDialA_valueChanged(int value)
{
    controller->on_waveformDialA_valueChanged(value);
}

void SynthUi::on_volumeFaderA_valueChanged(int value)
{
    controller->on_volumeFaderA_valueChanged(value);
}