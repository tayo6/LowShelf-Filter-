#include "PluginEditor.h"

//==============================================================================
LowShelfFilterAudioProcessorEditor::LowShelfFilterAudioProcessorEditor(
    LowShelfFilterAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&customLookAndFeel);

    //========================================================================
    // Rotary Slider
    //========================================================================
    lowShelfSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lowShelfSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lowShelfSlider.setRange(0.0, 100.0, 0.1);
    lowShelfSlider.setValue(0.0, juce::dontSendNotification);
    lowShelfSlider.setDoubleClickReturnValue(true, 0.0);

    // Standard VST interaction feel
    lowShelfSlider.setVelocityBasedMode(true);
    lowShelfSlider.setVelocityModeParameters(1.0, 1, 0.1, true);
    lowShelfSlider.setSkewFactor(1.0);

    // Arc: 0 at 7 o'clock (120 deg), 100 at 5 o'clock (60 deg)
    // In JUCE: start = 60 deg (5 o'clock), end = 120 deg (7 o'clock)
    // Clockwise from 60 to 120 passes through 90 (6 o'clock) = bottom arc
    lowShelfSlider.setRotaryParameters(
        juce::MathConstants<float>::pi / 3.0f,          // 60 deg (5 o'clock)
        juce::MathConstants<float>::pi * 2.0f / 3.0f,  // 120 deg (7 o'clock)
        true);

    lowShelfSlider.setTooltip("Low Shelf Filter Amount (0-100%)");
    addAndMakeVisible(lowShelfSlider);

    //========================================================================
    // Parameter Attachment
    //========================================================================
    lowShelfAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(),
            LowShelfFilterAudioProcessor::getLowShelfParamID(),
            lowShelfSlider);

    //========================================================================
    // Labels
    //========================================================================
    titleLabel.setText("LOW SHELF", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xFF333333));
    addAndMakeVisible(titleLabel);

    valueLabel.setText("0", juce::dontSendNotification);
    valueLabel.setJustificationType(juce::Justification::centred);
    valueLabel.setFont(juce::Font(26.0f, juce::Font::bold));
    valueLabel.setColour(juce::Label::textColourId, juce::Colour(0xFF333333));
    addAndMakeVisible(valueLabel);

    unitLabel.setText("%", juce::dontSendNotification);
    unitLabel.setJustificationType(juce::Justification::centred);
    unitLabel.setFont(juce::Font(12.0f, juce::Font::plain));
    unitLabel.setColour(juce::Label::textColourId, juce::Colour(0xFF888888));
    addAndMakeVisible(unitLabel);

    //========================================================================
    // Window Setup
    //========================================================================
    setSize(360, 360);
    setResizable(false, false);

    startTimerHz(30);
}

LowShelfFilterAudioProcessorEditor::~LowShelfFilterAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

//==============================================================================
void LowShelfFilterAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
    g.setColour(juce::Colour(0xFFE8E8E8));
    g.drawRect(getLocalBounds(), 1);
}

void LowShelfFilterAudioProcessorEditor::resized()
{
    auto b = getLocalBounds();

    titleLabel.setBounds(b.removeFromTop(42));
    unitLabel.setBounds(b.removeFromBottom(18));
    valueLabel.setBounds(b.removeFromBottom(32));

    int margin = 16;
    b.reduce(margin, margin);

    int sz = juce::jmin(b.getWidth(), b.getHeight());
    int x  = b.getX() + (b.getWidth()  - sz) / 2;
    int y  = b.getY() + (b.getHeight() - sz) / 2;
    lowShelfSlider.setBounds(x, y, sz, sz);
}

//==============================================================================
void LowShelfFilterAudioProcessorEditor::timerCallback()
{
    updateValueDisplay();
}

void LowShelfFilterAudioProcessorEditor::updateValueDisplay()
{
    int v = static_cast<int>(lowShelfSlider.getValue());
    juce::String t = juce::String(v);
    if (valueLabel.getText() != t)
        valueLabel.setText(t, juce::dontSendNotification);
}
