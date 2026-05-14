cmake_minimum_required(VERSION 4.1.2)

project(JuceTutorials VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(JUCE)

add_subdirectory(SimpleNoise)
add_subdirectory(SimpleNoisePan)
add_subdirectory(WaveformSynth)