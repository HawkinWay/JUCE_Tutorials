#include <JuceHeader.h>
#include "../include/SimpleNoisePan.h"

class SimpleNoisePanApplication : public juce::JUCEApplication {
public:
    SimpleNoisePanApplication() = default;

    const String getApplicationName() override { return "SimpleNoisePan"; }
    const String getApplicationVersion() override{ return "1.0.0"; };

    void initialise(const String &commandLineParameters) override {
        mainWindow.reset(new SimpleNoiseApplicationWindow("SimpleNoisePan", std::make_unique<SimpleNoisePan>(), *this));
    }

    void shutdown() override {
        mainWindow = nullptr;
    }
private:
    class SimpleNoiseApplicationWindow : public juce::DocumentWindow {
    public:
        SimpleNoiseApplicationWindow (const juce::String& name, std::unique_ptr<juce::Component> c, JUCEApplication& a)
            : DocumentWindow (name, juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                                .findColour (ResizableWindow::backgroundColourId),
                              juce::DocumentWindow::allButtons),
              app (a)
        {
            setUsingNativeTitleBar (true);

           #if JUCE_ANDROID || JUCE_IOS
            setContentOwned (new SafeAreaComponent { std::move (c) }, true);
            setFullScreen (true);
           #else
            setContentOwned (c.release(), true);
            setResizable (true, false);
            setResizeLimits (300, 250, 10000, 10000);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            app.systemRequestedQuit();
        }

       #if JUCE_ANDROID || JUCE_IOS
        class SafeAreaComponent : public juce::Component
        {
        public:
            explicit SafeAreaComponent (std::unique_ptr<Component> c)
                : content (std::move (c))
            {
                addAndMakeVisible (*content);
            }

            void resized() override
            {
                if (const auto* d = Desktop::getInstance().getDisplays().getDisplayForRect (getLocalBounds()))
                    content->setBounds (d->safeAreaInsets.subtractedFrom (getLocalBounds()));
            }

        private:
            std::unique_ptr<Component> content;
        };

        void parentSizeChanged() override
        {
            if (auto* c = getContentComponent())
                c->resized();
        }
       #endif

    private:
        JUCEApplication& app;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleNoiseApplicationWindow)
    };
    std::unique_ptr<SimpleNoiseApplicationWindow> mainWindow;
};

START_JUCE_APPLICATION (SimpleNoisePanApplication)
