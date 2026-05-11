#pragma once

#include <ranges>
#include <juce_audio_utils/juce_audio_utils.h>

class SimpleNoisePan : public juce::AudioAppComponent {
public:
    SimpleNoisePan() {
        l_Slider.setRange(0.0, 0.25);
        l_Slider.setValue(0.125, juce::NotificationType::dontSendNotification);
        l_Slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxRight, false, 100, 20);

        r_Slider.setRange(0.0, 0.25);
        r_Slider.setValue(0.125, juce::NotificationType::dontSendNotification);
        r_Slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxRight, false, 100, 20);

        l_Label.setText("L",juce::NotificationType::dontSendNotification);
        r_Label.setText("R",juce::NotificationType::dontSendNotification);

        addAndMakeVisible(l_Slider);
        addAndMakeVisible(r_Slider);
        addAndMakeVisible(l_Label);
        addAndMakeVisible(r_Label);

        setSize(800, 600);
        setAudioChannels(0, 2);
    }

    ~SimpleNoisePan() {
        shutdownAudio();
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {

    }

    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override {

        auto l_level = static_cast<float>(l_Slider.getValue());
        auto r_level = static_cast<float>(r_Slider.getValue());


        for (auto channel : std::views::iota(0, bufferToFill.buffer->getNumChannels())) {

            auto* buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

            for (auto sample : std::views::iota(0, bufferToFill.numSamples)) {
// bufferToFill.buffer->getNumSamples() is whole buffer capacity, bufferToFill.numSamples is the samples we need to process this time

                if (channel == 0) {
                    *buffer++ = getRandomValue() * l_level;
                }else {
                    *buffer++ = getRandomValue() * r_level;
                }

            }

        }
#if 0
        for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++) {

            auto* buffer = bufferToFill.buffer->getWritePointer(channel);

            for (auto sample = 0; sample < bufferToFill.buffer->getNumSamples(); sample++) {

                if (channel == 0) {
                    *buffer++ = getRandomValue() * l_level;
                }else {
                    *buffer++ = getRandomValue() * r_level;
                }

            }
        }
#endif
    }

    float getRandomValue() noexcept {
        return _random.nextFloat() * 2.f - 1.f;
    }

    void releaseResources() override {

    }

    void resized() override {
        l_Slider.setBounds(100, 10, getWidth() - 110, 20);
        l_Label.setBounds(10, 10, 90, 20);

        r_Slider.setBounds(100, 40, getWidth() - 110, 20);
        r_Label.setBounds(10, 40, 90, 20);
    }
private:
    juce::Random _random;

    juce::Slider l_Slider;
    juce::Slider r_Slider;
    juce::Label l_Label;
    juce::Label r_Label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleNoisePan);
};