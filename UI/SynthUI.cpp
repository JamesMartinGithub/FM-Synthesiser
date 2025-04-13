#include "SynthUI.h"
#include "ui_SynthUI.h"
#include "Controller.h"
#include <QLineEdit>
#include <QKeyEvent>

SynthUi::SynthUi(Controller* controllerRef, QWidget *parent)
    : controller(controllerRef)
    , QMainWindow(parent)
    , ui(new Ui::SynthUi) {
    ui->setupUi(this);
    setFixedSize(700, 470);
    InitialiseSpinBox(ui->unisonSelectorA);
    InitialiseSpinBox(ui->unisonSelectorB);
    InitialiseSpinBox(ui->octaveSelectorA);
    InitialiseSpinBox(ui->octaveSelectorB);
    InitialiseSpinBox(ui->semitoneSelectorA);
    InitialiseSpinBox(ui->semitoneSelectorB);
}

SynthUi::~SynthUi() {
    delete ui;
}

void SynthUi::SetAudioData(unsigned char *data) {
    ui->waveformRenderer->SetAudioData(data);
}

void SynthUi::keyPressEvent(QKeyEvent *event) {
    if(!event->isAutoRepeat())
    {
        if (char keyChar = KeyToChar(event->key()); keyChar != '*') {
            controller->PlayNote(keyChar);
            //if (int held =  held != -1) {
            //    heldKeys[held] = keyChar;
            //}
        }
    }
}

void SynthUi::keyReleaseEvent(QKeyEvent *event) {
    if(!event->isAutoRepeat())
    {
        if (char keyChar = KeyToChar(event->key()); keyChar != '*') {
            controller->ReleaseNote(keyChar);
        }
    }
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

void SynthUi::FilterArrowClicked(bool isUp) {
    // Increment/Decrement filter mode cyclically
    filterMode += isUp ? 1 : -1;
    if (filterMode < 0) { filterMode = filterModes.size() - 1; }
    if (filterMode >= filterModes.size()) { filterMode = 0; }
    controller->SetFilterType(filterMode);
    // Update label
    QString text;
    text += filterModes[filterMode];
    ui->filterLabel->setText(text);
}

char SynthUi::KeyToChar(int key) {
    char keyChar = key;
    switch (key) {
        case Qt::Key_Z:
        case Qt::Key_S:
        case Qt::Key_X:
        case Qt::Key_D:
        case Qt::Key_C:
        case Qt::Key_V:
        case Qt::Key_G:
        case Qt::Key_B:
        case Qt::Key_H:
        case Qt::Key_N:
        case Qt::Key_J:
        case Qt::Key_M:
        case Qt::Key_Comma:
        case Qt::Key_L:
        case Qt::Key_Period:
        case Qt::Key_Semicolon:
        case Qt::Key_Slash:
            return keyChar;
        default:
            return '*';
    }
}

QString SynthUi::TimeToText(float time) {
    QString text;
    if (time >= 1.0f) { text += std::to_string(time).substr(0, 4) + "s"; }
    if (time < 1.0f && 0.1f <= time) { text += std::to_string(time * 1000).substr(0, 3) + "ms"; }
    if (time < 0.1f) { text += std::to_string(time * 1000).substr(0, 4) + "ms"; }
    return text;
}

QString SynthUi::PercentToText(float percent) {
    QString text;
    text += std::to_string((int)(percent * 100)) + "%";
    return text;
}

QString SynthUi::PanToText(int pan) {
    QString text;
    if (pan < 50) { text += std::to_string(100 - pan) + "L"; }
    if (pan == 50) { text += "C"; }
    if (pan > 50) { text += std::to_string(pan) + "R"; }
    return text;
}

QString SynthUi::FrequencyToText(float frequency) {
    QString text;
    text += std::to_string((int)frequency) + "Hz";
    return text;
}
/// Master
void SynthUi::on_volumeFaderMaster_valueChanged(int value) {
    controller->SetMasterVolume(value);
}

void SynthUi::on_adsrAttackDial_valueChanged(int value) {
    float fValue = controller->SetADSR(value, 'A');
    ui->adsrAttackLabel->setText(TimeToText(fValue));
}

void SynthUi::on_adsrDecayDial_valueChanged(int value) {
    float fValue = controller->SetADSR(value, 'D');
    ui->adsrDecayLabel->setText(TimeToText(fValue));
}

void SynthUi::on_adsrSustainDial_valueChanged(int value) {
    float fValue = controller->SetADSR(value, 'S');
    ui->adsrSustainLabel->setText(PercentToText(fValue));
}

void SynthUi::on_adsrReleaseDial_valueChanged(int value) {
    float fValue = controller->SetADSR(value, 'R');
    ui->adsrReleaseLabel->setText(TimeToText(fValue));
}

void SynthUi::on_filterUp_clicked() {
    FilterArrowClicked(true);
}

void SynthUi::on_filterDown_clicked() {
    FilterArrowClicked(false);
}

void SynthUi::on_filterToggleA_toggled(bool checked) {
    controller->SetFilterSelection(checked, true);
}

void SynthUi::on_filterToggleB_toggled(bool checked) {
    controller->SetFilterSelection(checked, false);
}

void SynthUi::on_filterFrequencyDial_valueChanged(int value) {
    float fValue = controller->SetFilterVariable(value, 'F');
    ui->filterFrequencyLabel->setText(FrequencyToText(fValue));
}

void SynthUi::on_filterResonanceDial_valueChanged(int value) {
    float fValue = controller->SetFilterVariable(value, 'R');
    ui->filterResonanceLabel->setText(PercentToText(fValue));
}
/// A
void SynthUi::on_octaveSelectorA_valueChanged(int arg1) {
    controller->SetOctaveSemitone(arg1, true, true);
}

void SynthUi::on_semitoneSelectorA_valueChanged(int arg1) {
    controller->SetOctaveSemitone(arg1, false, true);
}

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
    ui->detuneLabelA->setText(PercentToText((float)value / 100));
}

void SynthUi::on_unisonWidthDialA_valueChanged(int value) {
    controller->SetUnisonWidth(value, true);
    ui->unisonWidthLabelA->setText(PercentToText((float)value / 100));
}

void SynthUi::on_modulationUpA_clicked() {
    ModulationArrowClicked(true, true);
}

void SynthUi::on_modulationDownA_clicked() {
    ModulationArrowClicked(true, false);
}

void SynthUi::on_modulationDialA_valueChanged(int value) {
    controller->SetModulationValue(value, true);
    ui->modulationValueLabelA->setText(PercentToText((float)value / 100));
}

void SynthUi::on_panDialA_valueChanged(int value){
    controller->SetPan(value, true);
    ui->panLabelA->setText(PanToText(value));
}
/// B
void SynthUi::on_octaveSelectorB_valueChanged(int arg1) {
    controller->SetOctaveSemitone(arg1, true, false);
}

void SynthUi::on_semitoneSelectorB_valueChanged(int arg1) {
    controller->SetOctaveSemitone(arg1, false, false);
}

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
    ui->detuneLabelB->setText(PercentToText((float)value / 100));
}

void SynthUi::on_unisonWidthDialB_valueChanged(int value) {
    controller->SetUnisonWidth(value, false);
    ui->unisonWidthLabelB->setText(PercentToText((float)value / 100));
}

void SynthUi::on_modulationUpB_clicked() {
    ModulationArrowClicked(false, true);
}

void SynthUi::on_modulationDownB_clicked() {
    ModulationArrowClicked(false, false);
}

void SynthUi::on_modulationDialB_valueChanged(int value) {
    controller->SetModulationValue(value, false);
    ui->modulationValueLabelB->setText(PercentToText((float)value / 100));
}

void SynthUi::on_panDialB_valueChanged(int value){
    controller->SetPan(value, false);
    ui->panLabelB->setText(PanToText(value));
}
