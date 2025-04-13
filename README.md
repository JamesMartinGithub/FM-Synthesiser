# FM-Synthesiser
(2025) Standalone synthesiser with dual oscillators, modulation, biquad filter, and polyphony. Written in C++ using XAudio2 API and QT GUI framework.

## Technologies/Techniques Used:
- OpenGL waveform rendering
- MVC design using QT GUI with custom widgets
- Efficient signal processing writing to XAudio2 buffers
- Artifact-less audio with consistent phase

## Features:
- Waveform visualiser
- Modulation types: FM, AM, RM, SYNC, SYNC (smoothed), and Quantize
- Interchangeable modulator and carrier
- ADSR controls
- Biquad filter with lowpass, highpass, and bandpass modes, and independant oscillator filtering
- 4 selectable waveforms: sine, square, triangle, and sawtooth
- Octave and semitone controls
- Oscillator specific and master volume faders
- Up to 8-voice unison with variable detune and stereo spread
- Stereo pan control
- Source code supports variable buffer size, samplerate, channel count, and polyphony

## Gallery:
<img src="https://github.com/user-attachments/assets/3e060762-c4a0-48a3-8afd-e68f0a153f91" width="311" height="174" style="display:none">
<img src="https://github.com/user-attachments/assets/16c67ec8-b5ac-41cf-9951-4815ab3ee9a8" width="311" height="174" style="display:none">
<img src="https://github.com/user-attachments/assets/4a0999fe-56fd-4c86-90fb-0f8daa5fac3b" width="311" height="174" style="display:none">
