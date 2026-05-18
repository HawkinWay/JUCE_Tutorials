juce_add_gui_app(AudioPlayer
        PRODUCT_NAME "AudioPlayer"
)

target_sources(AudioPlayer
        PRIVATE
        source/main.cpp
        include/AudioPlayer.h
)

target_link_libraries(AudioPlayer
        PRIVATE
        juce::juce_audio_utils
)

juce_generate_juce_header(AudioPlayer)
