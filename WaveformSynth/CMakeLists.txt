juce_add_gui_app(WaveformSynth
        PRODUCT_NAME "WaveformSynth"
)

target_sources(WaveformSynth
        PRIVATE
        source/main.cpp
        include/WaveformSynth.h
)

target_link_libraries(WaveformSynth
        PRIVATE
        juce::juce_audio_utils
)

juce_generate_juce_header(WaveformSynth)
