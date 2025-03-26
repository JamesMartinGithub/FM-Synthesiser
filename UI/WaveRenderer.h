#ifndef SYNTH_WAVERENDERER_H
#define SYNTH_WAVERENDERER_H

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtOpenGL/QOpenGLFunctions_4_0_Core>

class WaveRenderer : public QOpenGLWidget, protected QOpenGLFunctions_4_0_Core {
    Q_OBJECT
public:
    explicit WaveRenderer(QWidget *parent = nullptr);
    void initializeGL() override;
    void paintGL() override;
    void SetAudioData(unsigned char* data);
private:
    static const int sampleCount = 1000;
    float vertices[sampleCount] = {0.0f};
};

#endif //SYNTH_WAVERENDERER_H