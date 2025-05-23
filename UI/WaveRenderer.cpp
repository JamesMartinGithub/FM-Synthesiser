#include "WaveRenderer.h"

WaveRenderer::WaveRenderer(QWidget *parent) : QOpenGLWidget(parent) {
}

void WaveRenderer::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void WaveRenderer::paintGL() {
    // Clear window
    glClear(GL_COLOR_BUFFER_BIT);
    // Create waveBuffer and point to vertex array
    GLuint waveBuffer;
    glGenBuffers(1, &waveBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, waveBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Load waveBuffer and draw vertices
    glBindVertexArray(waveBuffer);
    glLineWidth(1.0f);
    glDrawArrays(GL_LINE_STRIP, 0, sampleCount / 2);
    glBindVertexArray(0);
}

void WaveRenderer::SetAudioData(unsigned char *data) {
    // Update vertex array (Setup for stereo data)
    for (int i = 0; i < sampleCount; i+= 2) {
        vertices[i] = (i * (4.0f / (float)sampleCount)) - 1;
        memcpy(&vertices[i + 1], &(data[2 * i]), 4);
    }
    update();
}