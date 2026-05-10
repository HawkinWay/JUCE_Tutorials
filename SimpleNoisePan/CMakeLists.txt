juce_add_gui_app(SimpleNoisePan
        PRODUCT_NAME "SimpleNoisePan"
)

target_sources(SimpleNoisePan
        PRIVATE
        source/main.cpp
        include/SimpleNoisePan.h
)

target_link_libraries(SimpleNoisePan
        PRIVATE
        juce::juce_audio_utils
)

juce_generate_juce_header(SimpleNoisePan)