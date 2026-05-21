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
        Pausing,
        Paused,
        Stopping
    };

    AudioPlayer() : state(TransportState::Stopped),
                    thumbnailCache(5),
                    thumbnail(512,formatManager,thumbnailCache)
     {
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

        forwardButton.setButtonText(">> 5s");
        forwardButton.setColour(juce::TextButton::buttonColourId, juce::Colours::gold);
        forwardButton.onClick = [this](){ forwardButtonClicked(); };
        forwardButton.setEnabled(false);
        forwardButton.setRepeatSpeed(500,100);  // if keep click more than 500ms, repeat onclick() every 100ms

        rewindButton.setButtonText("<< 5s");
        rewindButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
        rewindButton.onClick = [this](){ rewindButtonClicked();};
        rewindButton.setEnabled(false);
        rewindButton.setRepeatSpeed(500,100);

        progressBar.setRange(0.0, totalLength);
        progressBar.setValue(0.0);
        progressBar.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        progressBar.onValueChange = [this](){
            if(progressBar.isMouseButtonDown()){
                auto progressBarPosition = progressBar.getValue();
                transportSource.setPosition(progressBarPosition);
            }
        };

        addAndMakeVisible(openButton);
        addAndMakeVisible(playButton);
        addAndMakeVisible(stopButton);
        addAndMakeVisible(forwardButton);
        addAndMakeVisible(rewindButton);
        addAndMakeVisible(progressBar);

        formatManager.registerBasicFormats();
        transportSource.addChangeListener(this);
        thumbnail.addChangeListener(this);

        setSize(500,300);
        setAudioChannels(0,2);
    }

    ~AudioPlayer() override{
        transportSource.setSource(nullptr);
        shutdownAudio();
    }

    void timerCallback() override{
        if(transportSource.isPlaying()){
            auto currentPosition = transportSource.getCurrentPosition();
            progressBar.setValue(currentPosition, juce::dontSendNotification);
            // transportSource.setPosition(currentPosition++);      error
        }
        repaint();
//  error
//        else if((state == TransportState::Stopping) || (state == TransportState::Pausing)){
//            stopTimer();
//        }
    }

    void changeListenerCallback(juce::ChangeBroadcaster* source) override{
        if(source == &transportSource){
            if(transportSource.isPlaying()){
                changeState(TransportState::Playing);
            }
            else if((state == TransportState::Stopping) || (state == TransportState::Playing)){
                changeState(TransportState::Stopped);
            }
            else if(TransportState::Pausing == state){
                changeState(TransportState::Paused);
            }
        }
        if(source == &thumbnail){
            repaint();
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

    void paint(juce::Graphics& g) override{
        juce::Rectangle<int> thumbnailBounds(10, 160, getWidth() - 20, getHeight() - 170);
        if(thumbnail.getNumChannels() == 0){
            paintIfNoFileLoaded(g, thumbnailBounds);
        }
        else{
            paintIfFileLoaded(g, thumbnailBounds);
        }
    }

    void resized() override{
        openButton.setBounds(10, 10, getWidth() - 20, 20);
        playButton.setBounds(10, 40, getWidth() - 20, 20);
        stopButton.setBounds(10, 70, getWidth() - 20, 20);
        rewindButton.setBounds(10, 100, getWidth() / 2 - 20, 20);
        const int rewindX = rewindButton.getWidth() + 30;
        forwardButton.setBounds(rewindX , 100, getWidth() / 2 - 20, 20);
        progressBar.setBounds(10, 130, getWidth() - 20, 20);
    }

private:
    void changeState(TransportState newState){
        if(state != newState){
            state = newState;
            switch(state){
                case TransportState::Stopped:
                    stopTimer();
                    progressBar.setValue(0.0);
                    playButton.setButtonText("Play");
                    stopButton.setButtonText("Stop");
                    stopButton.setEnabled(false);
                    // playButton.setEnabled(true);
                    transportSource.setPosition(0.0);
                    break;
                case TransportState::Starting:
                    // playButton.setEnabled(false);
                    transportSource.start();
                    break;
                case TransportState::Playing:
                    startTimerHz(30);
                    playButton.setButtonText("Pause");
                    stopButton.setButtonText("Stop");
                    stopButton.setEnabled(true);
                    break;
                case TransportState::Pausing:
                    transportSource.stop();
                    break;
                case TransportState::Paused:
                    stopTimer();
                    playButton.setButtonText("Resume");
                    stopButton.setButtonText("Return to Zero");
                    break;
                case TransportState::Stopping:
                    transportSource.stop();
                    break;
            }
        }
    }

    void paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds){
        g.setColour(juce::Colours::darkgrey);
        g.fillRect(thumbnailBounds);
        g.setColour(juce::Colours::white);
        g.drawFittedText("No File Loaded...", thumbnailBounds, juce::Justification::centred, 1);
    }

    void paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds){
        g.setColour(juce::Colours::darkgrey);
        g.fillRect(thumbnailBounds);

        g.setColour(juce::Colours::black);
        auto audioLength =thumbnail.getTotalLength();
        thumbnail.drawChannels(g,
                               thumbnailBounds,
                               0.0,
                               thumbnail.getTotalLength(),
                               1.f
                               );

        if(audioLength > 0.0) {
            g.setColour(juce::Colours::red);
            auto audioPosition = transportSource.getCurrentPosition();
            auto drawPosition = (audioPosition / audioLength) * thumbnailBounds.getWidth() + thumbnailBounds.getX();
            g.drawLine(static_cast<float>(drawPosition), static_cast<float>(thumbnailBounds.getY()),
                       static_cast<float>(drawPosition), static_cast<float>(thumbnailBounds.getBottom()), 1.f);
        }
    }

    void openButtonClicked(){
        if(state == TransportState::Paused){
            changeState(TransportState::Stopped);
        }

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
                                        transportSource.stop();
                                        transportSource.setSource(nullptr);
                                        readerSource.reset();
                                        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);

                                        totalLength = transportSource.getLengthInSeconds();
                                        progressBar.setRange(0.0, totalLength);
                                        playButton.setEnabled(true);
                                        forwardButton.setEnabled(true);
                                        rewindButton.setEnabled(true);

                                        thumbnail.setSource(new juce::FileInputSource(file));
                                        // readerSource.reset(newSource.release());
                                        readerSource = std::move(newSource);
                                    }
                                }
                             });
    }

    void playButtonClicked(){
        if((state == TransportState::Stopped) || (state == TransportState::Paused))
            changeState(TransportState::Starting);
        else if(state == TransportState::Playing)
            changeState(TransportState::Pausing);
    }

    void stopButtonClicked(){
        if(state == TransportState::Paused)
            changeState(TransportState::Stopped);
        else
            changeState(TransportState::Stopping);
        // progressBar.setValue(0.0);
    }

    void forwardButtonClicked(){
        auto currentPosition = transportSource.getCurrentPosition();
        auto possibleSeconds = forwardButton.isMouseButtonDown() ? 1.0 : 5.0;
        auto newPosition = juce::jmin(currentPosition + possibleSeconds,totalLength);  // if position > totalLength, position = totalLength
        transportSource.setPosition(newPosition);
    }

    void rewindButtonClicked(){
        auto currentPosition = transportSource.getCurrentPosition();
        auto possibleSeconds = rewindButton.isMouseButtonDown() ? 1.0 : 5.0;
        auto newPosition = juce::jmax(0.0, currentPosition - possibleSeconds);  // if position < 0.0, position = 0.0
        transportSource.setPosition(newPosition);
    }

    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton forwardButton;
    juce::TextButton rewindButton;
    juce::Slider progressBar;

    double totalLength = 1.0;

    TransportState state;
    std::unique_ptr<juce::FileChooser> chooser;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;    // have benefit of being exception-safe when AudioTransportSource::setSource()
    juce::AudioTransportSource transportSource;

    juce::AudioThumbnailCache   thumbnailCache;
    juce::AudioThumbnail    thumbnail;  // this obj will access thumbnailCache and formatManager, so we write this below them
};
