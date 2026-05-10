#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

class SimpleNoise : public juce::AudioAppComponent {
public:
    SimpleNoise() {
        _levelSlider.setRange(0.0, 0.25);
        _levelSlider.setTextBoxStyle(juce::Slider::/*TextEntryBoxPosition::*/TextBoxRight, false, 100, 20);
        _levelLabel.setText("Noise Level", juce::/*NotificationType::*/dontSendNotification);

        addAndMakeVisible(_levelSlider);
        addAndMakeVisible(_levelLabel);

        setSize(800, 600);
        setAudioChannels(0,2);
    }

    ~SimpleNoise() {
        shutdownAudio();
    }

    void prepareToPlay (int samplePerBlockExpected, double sampleRate) override{
        juce::String message;
        message << "preparing to play audio...\n";
        message << "sampleRate: " << sampleRate << "\n";
        message << "samplesPerBlockExpected: " << samplePerBlockExpected << "\n";
        juce::Logger::getCurrentLogger()->writeToLog(message);
    }

    void releaseResources() override {
        juce::Logger::getCurrentLogger()->writeToLog("Releasing audio resources.");
    }

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override {

        auto level = static_cast<float>(_levelSlider.getValue());

        for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++) {

            auto *buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

            for (auto sample = 0; sample < bufferToFill.buffer->getNumSamples(); sample++) {

                //buffer[sample] = _random.nextFloat() * 0.25f - 0.125f;
                *buffer++ = _random.nextFloat() * level;

            }
        }
    }

    void resized() override {
        _levelSlider.setBounds(100, 10, getWidth() - 110, 20);
        _levelLabel.setBounds(10, 10, 90, 20);
    }

private:
    juce::Random _random;
    juce::Slider _levelSlider;
    juce::Label _levelLabel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleNoise)
};



