#pragma once

#include<ranges>
#include<juce_audio_utils/juce_audio_utils.h>

class WaveformSynth : public juce::AudioAppComponent {
public:
    enum class WaveformType {
        sine = 1,
        sawtooth = 2,
        square = 3,
    };

    WaveformSynth() {
        frequencySlider.setRange(50.0, 5000.0);
        frequencySlider.setSkewFactorFromMidPoint (500.0);
        frequencySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxRight, false, 80, 20);
        frequencySlider.onValueChange = [this]() {
            if (currentSampleRate > 0.0) {
                updateAngleDelta();
            }
        };

        waveformBox.addItem("Sine", static_cast<int>(WaveformType::sine));
        waveformBox.addItem("Sawtooth", static_cast<int>(WaveformType::sawtooth));
        waveformBox.addItem("Square", static_cast<int>(WaveformType::square));
        waveformBox.setSelectedId(static_cast<int>(WaveformType::sine));

        addAndMakeVisible(frequencySlider);
        addAndMakeVisible(waveformBox);

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
        auto* left = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto* right = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
        for (const auto sample : std::views::iota(0, bufferToFill.numSamples)) {

            auto currentSample = 0.f;

            switch (waveformBox.getSelectedId()) {
                case static_cast<int>(WaveformType::sine):
                    currentSample = static_cast<float>(std::sin(currentAngle));
                    break;
               case  static_cast<int>(WaveformType::sawtooth):
                    currentSample = sawtooth(currentAngle);
                    break;
               case static_cast<int>(WaveformType::square):
                    currentSample = square(currentAngle);
                    break;
            }

            currentAngle += angleDelta;
            if (currentAngle >= juce::MathConstants<double>::twoPi)
                currentAngle -= juce::MathConstants<double>::twoPi;
            *left++ = currentSample * level;
            *right++ = currentSample * level;
        }
    }

    void releaseResources() override {

    }

    void updateAngleDelta() {
        const auto cyclesPerSample = frequencySlider.getValue() / currentSampleRate;   // calculate the number of cycles
        angleDelta = cyclesPerSample * juce::MathConstants<double>::twoPi;
    }

    void resized() override {
        frequencySlider.setBounds(100,80,getWidth() - 120,20);
        waveformBox.setBounds(10,80,80,20);
    }

    static float sawtooth(double phase) {
        const auto ft = phase / juce::MathConstants<double>::twoPi;
        const auto normalizedPhase = ft - std::floor(ft);
        return 2.f * normalizedPhase - 1.f;
    }

    static float square(double phase) {
        const auto ft = phase / juce::MathConstants<float>::twoPi;
        const auto normalizedPhase = ft - std::floor(ft);
        return (normalizedPhase < 0.5f) ? 1.f : -1.f;
    }

private:
    double currentSampleRate = 0.0;
    double currentAngle = 0.0;
    double angleDelta = 0.0;

    juce::Slider frequencySlider;
    juce::ComboBox waveformBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformSynth)
};