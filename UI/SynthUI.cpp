#include "SynthUI.h"
#include "ui_SynthUI.h"
#include "Controller.h"
#include <QLineEdit>

SynthUi::SynthUi(Controller* controllerRef, QWidget *parent)
    : controller(controllerRef)
    , QMainWindow(parent)
    , ui(new Ui::SynthUi) {
    ui->setupUi(this);
    InitialiseSpinBox(ui->unisonSelectorA);
    InitialiseSpinBox(ui->unisonSelectorB);
}

SynthUi::~SynthUi() {
    delete ui;
}

void SynthUi::SetController(Controller* controllerRef) {
    controller = controllerRef;
}

void SynthUi::SetAudioData(unsigned char *data) {
    ui->waveformRenderer->SetAudioData(data);
}

void SynthUi::InitialiseSpinBox(QSpinBox* spinBox) {
    QLineEdit* spinBoxLineEdit = spinBox->findChild<QLineEdit*>();
    spinBoxLineEdit->setReadOnly(true);
    spinBoxLineEdit->setFocusPolicy(Qt::NoFocus);
    connect(spinBox, &QSpinBox::valueChanged, spinBox,
            [&, spinBoxLineEdit](){spinBoxLineEdit->deselect();}, Qt::QueuedConnection);
}

void SynthUi::ModulationArrowClicked(bool isA, bool isUp) {
    // Increment/Decrement modulation mode cyclically
    int i = isA ? 1 : 0;
    int iInv = isA ? 0 : 1;
    modulationMode[i] += isUp ? 1 : -1;
    if (modulationMode[i] < 0) { modulationMode[i] = modulationModes.size() - 1; }
    if (modulationMode[i] >= modulationModes.size()) { modulationMode[i] = 0; }
    // Set modulation mode to 0 if both modes use the other oscillator
    if ((modulationMode[i] == 1 || modulationMode[i] == 2 || modulationMode[i] == 3) &&
    (modulationMode[iInv] == 1 || modulationMode[iInv] == 2 || modulationMode[iInv] == 3)) {
        modulationMode[iInv] = 0;
        controller->SetModulationType(0, !isA);
    }
    controller->SetModulationType(modulationMode[i], isA);
    // Update labels
    QString text1, text2;
    text1 += modulationModes[modulationMode[1]];
    text2 += modulationModes[modulationMode[0]];
    ui->modulationLabelA->setText(text1);
    ui->modulationLabelB->setText(text2);
}
/// Master
void SynthUi::on_volumeFaderMaster_valueChanged(int value) {
    controller->SetMasterVolume(value);
}
/// A
void SynthUi::on_waveformDialA_valueChanged(int value) {
    controller->SetWaveform(value, true);
}

void SynthUi::on_volumeFaderA_valueChanged(int value) {
    controller->SetOscillatorVolume(value, true);
}

void SynthUi::on_unisonSelectorA_valueChanged(int arg1) {
    controller->SetUnison(arg1, true);
}

void SynthUi::on_detuneDialA_valueChanged(int value) {
    controller->SetDetune(value, true);
}

void SynthUi::on_unisonWidthDialA_valueChanged(int value) {
    controller->SetUnisonWidth(value, true);
}

void SynthUi::on_modulationUpA_clicked() {
    ModulationArrowClicked(true, true);
}

void SynthUi::on_modulationDownA_clicked() {
    ModulationArrowClicked(true, false);
}

void SynthUi::on_modulationDialA_valueChanged(int value) {
    controller->SetModulationValue(value, true);
}

void SynthUi::on_octaveSelectorA_valueChanged(int arg1) {
    controller->SetOctaveSemitone(arg1, true, true);
}

void SynthUi::on_semitoneSelectorA_valueChanged(int arg1) {
    controller->SetOctaveSemitone(arg1, false, true);
}
/// B
void SynthUi::on_waveformDialB_valueChanged(int value) {
    controller->SetWaveform(value, false);
}

void SynthUi::on_volumeFaderB_valueChanged(int value) {
    controller->SetOscillatorVolume(value, false);
}

void SynthUi::on_unisonSelectorB_valueChanged(int arg1) {
    controller->SetUnison(arg1, false);
}

void SynthUi::on_detuneDialB_valueChanged(int value) {
    controller->SetDetune(value, false);
}

void SynthUi::on_unisonWidthDialB_valueChanged(int value) {
    controller->SetUnisonWidth(value, false);
}

void SynthUi::on_modulationUpB_clicked() {
    ModulationArrowClicked(false, true);
}

void SynthUi::on_modulationDownB_clicked() {
    ModulationArrowClicked(false, false);
}

void SynthUi::on_modulationDialB_valueChanged(int value) {
    controller->SetModulationValue(value, false);
}

void SynthUi::on_octaveSelectorB_valueChanged(int arg1) {
    controller->SetOctaveSemitone(arg1, true, false);
}

void SynthUi::on_semitoneSelectorB_valueChanged(int arg1) {
    controller->SetOctaveSemitone(arg1, false, false);
}