#pragma once

#include<ranges>
#include<juce_audio_utils/juce_audio_utils.h>

class WaveformSynth : public juce::AudioAppComponent {
public:
    WaveformSynth() {
        frequencySlider.setRange(50.0, 5000.0);
        //frequencySlider.setSkewFactorFromMidPoint (500.0);
        frequencySlider.onValueChange = [this]() {
            if (currentSampleRate > 0.0)
                updateAngleDelta();
        };

        addAndMakeVisible(frequencySlider);

        setSize(600,100);
        setAudioChannels(0,2);
    }

    ~WaveformSynth() {
        shutdownAudio();
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {
        juce::ignoreUnused(samplesPerBlockExpected);
        currentSampleRate = sampleRate;
        updateAngleDelta();
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override {
        auto level = 0.125f;
        auto* left = bufferToFill.buffer->getWritePointer(0,bufferToFill.startSample);
        auto* right = bufferToFill.buffer->getWritePointer(1,bufferToFill.startSample);
        for (const auto sample : std::views::iota(0, bufferToFill.numSamples)) {
            auto currentSample = static_cast<float>(std::sin(angleDelta));
            currentAngle += angleDelta;
            *left++ = currentSample * level;
            *right++ = currentSample * level;
        }
    }

    void releaseResources() override {

    }

    void updateAngleDelta() {
        auto cyclesPerSample = frequencySlider.getValue() / currentSampleRate;   // calculate the number of cycles
        angleDelta = cyclesPerSample * juce::MathConstants<double>::twoPi;
    }

    void resized() override {
        frequencySlider.setBounds(getBounds());
    }

private:
    double currentSampleRate = 0.0;
    double currentAngle = 0.0;
    double angleDelta = 0.0;

    juce::Slider frequencySlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformSynth)
};