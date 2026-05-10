#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

class SimpleNoise : public juce::AudioAppComponent {
public:
    SimpleNoise() : _targetLevel(0.125f) {
        
        _levelSlider.setRange(0.0, 0.25);
        _levelSlider.setValue(_targetLevel, juce::/*NotificationType::*/dontSendNotification);
        _levelSlider.setTextBoxStyle(juce::Slider::/*TextEntryBoxPosition::*/TextBoxRight, false, 100, 20);
        _levelSlider.onValueChange = [this]() {
            _targetLevel = static_cast<float>(_levelSlider.getValue());
            _samplesToTarget = _rampLengthSamples;
        };

        _levelLabel.setText("Noise Level", juce::/*NotificationType::*/dontSendNotification);

        addAndMakeVisible(_levelSlider);
        addAndMakeVisible(_levelLabel);

        setSize(800, 600);
        setAudioChannels(0,2);
    }

    ~SimpleNoise() {
        shutdownAudio();
    }

#if 0
    void prepareToPlay (int samplePerBlockExpected, double sampleRate) override{
        juce::String message;
        message << "preparing to play audio...\n";
        message << "sampleRate: " << sampleRate << "\n";
        message << "samplesPerBlockExpected: " << samplePerBlockExpected << "\n";
        juce::Logger::getCurrentLogger()->writeToLog(message);
    }
#endif

    void prepareToPlay(int, double) override {
        resetParameters();
    }

    void releaseResources() override {
        juce::Logger::getCurrentLogger()->writeToLog("Releasing audio resources.");
    }

#if 0
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
#endif
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override {

        auto numSamplesRemaining = bufferToFill.numSamples;     // the number of samples in  the buffer that should be R/W
        auto offset = 0;

        if (_samplesToTarget > 0) {
            auto levelIncrement = (_targetLevel - _currentLevel) / static_cast<float>(_samplesToTarget);

            // ramp length(_samplesToTarget) may longer than block size(numSamplesRemaining), may not
            auto numSamplesThisTime = juce::jmin(numSamplesRemaining, _samplesToTarget);
            
            for (auto sample = 0; sample < numSamplesThisTime; sample++) {
                
                for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++) {
                    bufferToFill.buffer->setSample(channel, sample, _random.nextFloat() * _currentLevel);            
                }
                _currentLevel += levelIncrement;
                _samplesToTarget--;
            }
            
            offset = numSamplesThisTime;
            numSamplesRemaining -= numSamplesThisTime;
            if (_samplesToTarget == 0)
                _currentLevel = _targetLevel;       
        }

        // if the block has not been processed completely, continue processing the remaining part
        if (numSamplesRemaining > 0) {
            
            for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel) {
                auto *buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample + offset);

                for (auto sample = 0; sample < numSamplesRemaining; ++sample)
                    *buffer++ = _random.nextFloat() * _currentLevel;
            
            }
        }

    }


    void resized() override {
        _levelSlider.setBounds(100, 10, getWidth() - 110, 20);
        _levelLabel.setBounds(10, 10, 90, 20);
    }

    void resetParameters() {
        _currentLevel = _targetLevel;
        _samplesToTarget = 0;
    }

private:
    juce::Random _random;
    juce::Slider _levelSlider;
    juce::Label _levelLabel;

    float _currentLevel;
    float _targetLevel;
    int _samplesToTarget;       // the number of samples that need to be smoothed

    // Regardless of how many objects are created, they all share this _rampLengthSamples
    // and its value is determined in compile-time and can't be modified
    static /*inline*/ constexpr auto _rampLengthSamples = 128;      // inline is implicit sine C++17

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleNoise)
};



