#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

class AudioPlayer : public juce::AudioAppComponent,
                    public juce::ChangeListener
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

    ~AudioPlayer() override{
        shutdownAudio();
    }

    void changeListenerCallback(juce::ChangeBroadcaster* source) override{
        if(source == &transportSource){
            if(transportSource.isPlaying()){
                changeState(TransportState::Playing);
            }
            else{
                changeState(TransportState::Stopped);
            }
        }
    }


    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override{
        transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override{
        if(readerSource.get() == nullptr){
            bufferToFill.clearActiveBufferRegion();
            return;
        }
        transportSource.getNextAudioBlock(bufferToFill);
    }

    void releaseResources() override{
        transportSource.releaseResources();
    }

    void resized() override{
        openButton.setBounds(10, 10, getWidth() - 20, 20);
        playButton.setBounds(10, 40, getWidth() - 20, 20);
        stopButton.setBounds(10, 70, getWidth() - 20, 20);
    }

private:
    void changeState(TransportState newState){
        if(state != newState){
            state = newState;
            switch(state){
                case TransportState::Stopped:
                    stopButton.setEnabled(false);
                    playButton.setEnabled(true);
                    transportSource.setPosition(0.0);
                    break;
                case TransportState::Starting:
                    playButton.setEnabled(false);
                    transportSource.start();
                    break;
                case TransportState::Playing:
                    stopButton.setEnabled(true);
                    break;
                case TransportState::Stopping:
                    transportSource.stop();
                    break;
            }
        }
    }

    void openButtonClicked(){
        chooser = std::make_unique<juce::FileChooser>(
                    "Select a file(wav, mp3) to play...",
                    juce::File {},
                    "*.wav;*.mp3"
                );

        auto chooserFlags = juce::FileBrowserComponent::openMode
                          | juce::FileBrowserComponent::canSelectFiles;

        chooser->launchAsync(chooserFlags,
                             [this](const juce::FileChooser& fc){
                                auto file = fc.getResult();
                                if(file != juce::File {}){
                                    auto* reader = formatManager.createReaderFor(file);
                                    if(reader != nullptr){
                                        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                                        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                                        playButton.setEnabled(true);
                                        readerSource.reset(newSource.release());
                                    }
                                }
                             });
    }

    void playButtonClicked(){
        changeState(TransportState::Starting);
    }

    void stopButtonClicked(){
        changeState(TransportState::Stopping);
    }

    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;

    TransportState state;
    std::unique_ptr<juce::FileChooser> chooser;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;    // have benefit of being exception-safe when AudioTransportSource::setSource()
    juce::AudioTransportSource transportSource;
};