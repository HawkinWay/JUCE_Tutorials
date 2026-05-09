juce_add_gui_app(SimpleNoise
		PRODUCT_NAME "SimpleNoise"
)

target_sources(SimpleNoise
		PRIVATE
		source/main.cpp
		include/SimpleNoise.h
)

target_link_libraries(SimpleNoise
		PRIVATE
		juce::juce_audio_utils
)

juce_generate_juce_header(SimpleNoise)