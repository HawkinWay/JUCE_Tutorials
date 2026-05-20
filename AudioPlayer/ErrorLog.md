# Audio Player Error Log

---

## The Progress Bar

### 1. Choose juce::ProgressBar or juce::Slider ?

> juce::ProgressBar
> - a one-way display tool
> - similar to progress bar during downloading or installing software/files
> - doesn't accept mouse clicking or dragging

so here we use juce::Slider

### 2. progressBar.onValueChange

The progressBar can be changed automatically by during **audio playback** or **mouse input**.

```C++
// ERROR
progressBar.onValueChange = [this](){
    auto progressBarPosition = progressBar.getValue();
    transportSource.setPosition(progressBarPosition);
};
```

This a **Feedback Loop** trap

The code above ignored the influence of audio playback. When audio drives the progressBar,
it will call progressBar.onValueChange and then forcibly change audio position 

### 3. Timer 

```C++
startTimerHz(30);   /* 1s / 30 = 33ms */
```
called timeCallBack() approximately every 33ms

we should call startTimerHz() and stopTimer() in changeState()

### 4. Player bugs

When we solved 2. , we sloved 1. So we can remove the changes in 1.

1. when click stopButton, transportSource is 0.0, but processBar is not reseted to 0

```C++
void stopButtonClicked(){
        if(state == TransportState::Paused)
            changeState(TransportState::Stopped);
        else
            changeState(TransportState::Stopping);
        progressBar.setValue(0.0);  // add this to solve
}
```

2. when open new file, playButton is still "Resume" and stopButton is still "Return to zero", 
and progressBar is not reseted to 0

```C++
void openButtonClicked(){
        // add this to solve
        if(state == TransportState::Paused){
            changeState(TransportState::Stopped);
        }
        // ...
}

void changeState(TransportState newState){
        if(state != newState){
            state = newState;
            switch(state){
                case TransportState::Stopped:
                    stopTimer();
                    progressBar.setValue(0.0);  // add this to solve
                    // ......
            }
        }
}
```