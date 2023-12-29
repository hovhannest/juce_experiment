#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_audio_utils/juce_audio_utils.h>
#include <iostream>

#include <array>
#include <span>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent final 
    : public juce::AudioAppComponent
    , public juce::ChangeListener
{
public:
    //==============================================================================
    MainComponent();
    
    ~MainComponent() override;

    void prepareToPlay (int ,
                        double ) override
    {   
    }

    void releaseResources() override
    {
    }

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        auto* device = deviceManager.getCurrentAudioDevice();

        auto activeInputChannels  = device->getActiveInputChannels();
        auto activeOutputChannels = device->getActiveOutputChannels();

        using OutputStreamType = std::span<float>;
        using InputStreamType = std::span<const float>;
        struct OutputStreemInfo
        {
            OutputStreamType stream;
            int channel;
        };
        struct InputStreemInfo
        {
            InputStreamType stream;
            int channel;
        };
        std::array<std::optional<InputStreemInfo>, 32>  inputs;
        std::array<std::optional<OutputStreemInfo>, 32> outputs;
        size_t i = 0;
        const size_t sampleRate = static_cast<const size_t>(bufferToFill.numSamples);
        forEach(activeOutputChannels, [&](int channel) {
            if(i >= 32) [[unlikely]] return;
            auto* outBuffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);
            outputs[i] = OutputStreemInfo{OutputStreamType(outBuffer, sampleRate), channel};

            for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                outBuffer[sample] = random.nextFloat() * 0.025f;
            ++i;
        });
        i = 0;
        forEach(activeInputChannels, [&bufferToFill, &inputs, &i, sampleRate](int channel) {
            if(i >= 32) [[unlikely]] return;
            auto* inBuffer  = bufferToFill.buffer->getReadPointer  (channel,
                                                                            bufferToFill.startSample);
            inputs[i] = InputStreemInfo{InputStreamType(inBuffer, sampleRate), channel};
            ++i;
        });

return;
        auto maxInputChannels  = activeInputChannels.countNumberOfSetBits();
        auto maxOutputChannels = activeOutputChannels.countNumberOfSetBits();

        for (auto channel = 0; channel < maxOutputChannels; ++channel)
        {
            if ((! activeOutputChannels[channel]) )
            {
                bufferToFill.buffer->clear (channel, bufferToFill.startSample, bufferToFill.numSamples);
            }
            else
            {
                auto actualInputChannel = maxInputChannels == 0 ? 0 : channel % maxInputChannels;

                if (! activeInputChannels[channel])
                {
                    bufferToFill.buffer->clear (channel, bufferToFill.startSample, bufferToFill.numSamples);

                    auto* outBuffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);

                    for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                        outBuffer[sample] = random.nextFloat() * 0.025f;
                }
                else
                {
                    auto* inBuffer  = bufferToFill.buffer->getReadPointer  (actualInputChannel,
                                                                            bufferToFill.startSample);
                    auto* outBuffer = bufferToFill.buffer->getWritePointer (channel, bufferToFill.startSample);

                    for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
                        outBuffer[sample] = (inBuffer[sample]+0.1f) * random.nextFloat() * 0.25f;
                }
            }
        }
    }

    void paint (juce::Graphics& g) override;
    void resized() override;
    
private:
    template<typename F>
    void forEach(juce::BigInteger channels, F f)
    {
        int maxInputChannels  = channels.countNumberOfSetBits();

        for(int channel = 0; channel < maxInputChannels; ++channel)
        {
            f(channel);
        }
    }

private:
    void changeListenerCallback (juce::ChangeBroadcaster*) override;

    void dumpDeviceInfo()
    {
        auto getListOfActiveBits = [] (const juce::BigInteger& b) -> juce::String 
        {
            juce::StringArray bits;

            for (auto i = 0; i <= b.getHighestBit(); ++i)
                if (b[i])
                    bits.add (juce::String (i));

            return bits.joinIntoString (", ");
        };
        logMessage ("--------------------------------------");
        logMessage ("Current audio device type: " + (deviceManager.getCurrentDeviceTypeObject() != nullptr
                                                     ? deviceManager.getCurrentDeviceTypeObject()->getTypeName()
                                                     : "<none>"));

        if (auto* device = deviceManager.getCurrentAudioDevice())
        {
            logMessage ("Current audio device: "   + device->getName().quoted());
            logMessage ("Sample rate: "    + juce::String (device->getCurrentSampleRate()) + " Hz");
            logMessage ("Block size: "     + juce::String (device->getCurrentBufferSizeSamples()) + " samples");
            logMessage ("Bit depth: "      + juce::String (device->getCurrentBitDepth()));
            logMessage ("Input channel names: "    + device->getInputChannelNames().joinIntoString (", "));
            logMessage ("Active input channels: "  + getListOfActiveBits (device->getActiveInputChannels()));
            logMessage ("Output channel names: "   + device->getOutputChannelNames().joinIntoString (", "));
            logMessage ("Active output channels: " + getListOfActiveBits (device->getActiveOutputChannels()));
        }
        else
        {
            logMessage ("No audio device open");
        }
    }
    

    void logMessage (const juce::String& m)
    {
        std::cout << m << std::endl;
    }

private:
    //==============================================================================
    // Your private member variables go here...

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)

private:
    juce::Random random;
    juce::AudioDeviceSelectorComponent audioSetupComp;
};
