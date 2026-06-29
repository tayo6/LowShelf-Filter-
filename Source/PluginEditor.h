#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

//==============================================================================
/**
 * Low Shelf Filter Plugin Editor.
 *
 * 360 x 360 white background with custom rotary knob.
 */
class LowShelfFilterAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                            private juce::Timer
{
public:
    explicit LowShelfFilterAudioProcessorEditor(LowShelfFilterAudioProcessor&);
    ~LowShelfFilterAudioProcessorEditor() override;

    //============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    //============================================================================
    void timerCallback() override;
    void updateValueDisplay();

    //============================================================================
    LowShelfFilterAudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;

    juce::Slider lowShelfSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowShelfAttachment;

    juce::Label titleLabel;
    juce::Label valueLabel;
    juce::Label unitLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LowShelfFilterAudioProcessorEditor)
};
