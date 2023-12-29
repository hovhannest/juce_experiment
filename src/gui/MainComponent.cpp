#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : audioSetupComp (deviceManager,
                        0,     // minimum input channels
                        256,   // maximum input channels
                        0,     // minimum output channels
                        256,   // maximum output channels
                        false, // ability to select midi inputs
                        false, // ability to select midi output device
                        false, // treat channels as stereo pairs
                        false) // hide advanced options
{
    addAndMakeVisible (audioSetupComp);
    setSize (600, 400);
    
    setAudioChannels (2, 2);
    deviceManager.addChangeListener (this);
    dumpDeviceInfo();
}

MainComponent::~MainComponent()
{
    deviceManager.removeChangeListener (this);
    shutdownAudio();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
        auto rect = getLocalBounds();

        audioSetupComp.setBounds (rect.removeFromLeft (proportionOfWidth (0.6f)));
        rect.reduce (10, 10);
}

void MainComponent::changeListenerCallback (juce::ChangeBroadcaster* cb)
{
    if(&deviceManager == cb)
    {
        dumpDeviceInfo();
        return;
    }
}