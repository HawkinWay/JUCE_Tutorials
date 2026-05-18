#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

class AudioPlayer : public juce::AudioAppComponent,
                    public juce::ChangeListener,
                    public juce::Timer
{
public:
    enum class TransportState{
        Stopped,
        Starting,
        Playing,
        Stopping
    };

    AudioPlayer() : state(TransportState::Stopped){
        openButton.setButtonText("Open...");
        openButton.onClick = [this](){ openButtonClicked(); };

        playButton.setButtonText("Play");
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        playButton.onClick = [this](){ playButtonClicked(); };
        playButton.setEnabled(false);

        stopButton.setButtonText("Stop");
        stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        stopButton.onClick = [this](){ stopButtonClicked(); };
        stopButton.setEnabled(false);

        addAndMakeVisible(openButton);
        addAndMakeVisible(playButton);
        addAndMakeVisible(stopButton);

        formatManager.registerBasicFormats();
        transportSource.addChangeListener(this);


        setSize(300,200);
        setAudioChannels(0,2);
    }

    void changeListenerCallback(juce::ChangeBroadcaster* source) override{

    }


    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override{

    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override{

    }

    void releaseResources() override{
        transportSource.releaseResources();
    }

    void resized() override{

    }

private:
    void changeState(TransportState newState){

    }

    void openButtonClicked(){

    }

    void playButtonClicked(){

    }

    void stopButtonClicked(){

    }

    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;

    TransportState state;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
};