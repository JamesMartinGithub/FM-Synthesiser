cmake_minimum_required(VERSION 3.22)
project(Synth)


set(CMAKE_SYSTEM_VERSION 10.0)
set(CMAKE_CXX_STANDARD 20)
set(WIN_SDK_PATH "C:/Program Files (x86)/Windows Kits/10")

link_directories("C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x64")

add_executable(Synth main.cpp VoiceCallback.cpp VoiceCallback.h Oscillator.cpp Oscillator.h AudioSettings.h)

target_link_libraries(Synth xaudio2.lib)