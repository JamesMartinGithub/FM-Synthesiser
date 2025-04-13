cmake_minimum_required(VERSION 3.22)
project(Synth)

set(CMAKE_SYSTEM_VERSION 10.0)
set(CMAKE_CXX_STANDARD 20)
set(WIN_SDK_PATH "C:/Program Files (x86)/Windows Kits/10")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_PREFIX_PATH $ENV{QTDIR})
find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6OpenGLWidgets REQUIRED)

set(UI_DIR "UI")
include_directories(${UI_DIR})
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${UI_DIR})

link_directories("C:/Program Files (x86)/Windows Kits/10/Lib/10.0.22621.0/um/x64")
link_directories("C:/Qt/6.8.2/mingw_64/lib")

add_executable(Synth main.cpp Controller.cpp Controller.h Oscillator.cpp Oscillator.h VoiceCallback.cpp VoiceCallback.h AudioSettings.h UI/SynthUI.cpp UI/SynthUI.h UI/SynthUI.ui UI/WaveRenderer.cpp UI/WaveRenderer.h RandomGenerator.cpp RandomGenerator.h ADSR.h
    UI/Resources.qrc UI/ParameterKnob.cpp UI/ParameterKnob.h)
target_link_libraries(Synth xaudio2.lib Qt6::Widgets Qt6::OpenGLWidgets)
