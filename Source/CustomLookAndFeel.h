#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/**
 * Custom LookAndFeel for a rotary knob based on SVG reference.
 *
 * SVG Reference:
 *   - Canvas: 95.25mm x 95.25mm, white background
 *   - Arc: dark red stroke (#782121), ~5.29mm thick, rounded caps
 *   - Arc geometry: elliptical, radius ~23.8mm, lower portion
 *
 * Implementation Spec:
 *   - Background: 360 x 360 px, white
 *   - Effective arc: 200 x 174 px
 *   - Arc span: ~5 o'clock to ~7 o'clock (bottom arc via 6 o'clock)
 *   - Gradient: Green (0) -> Yellow (mid) -> Red (100)
 *   - Rounded line caps (stroke-linecap: round)
 *   - Interactive radial slider, standard VST feel
 */
class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    ~CustomLookAndFeel() override;

    //============================================================================
    // juce::LookAndFeel_V4 overrides
    //============================================================================
    void drawRotarySlider(juce::Graphics& g,
                          int x, int y,
                          int width, int height,
                          float sliderPosProportional,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          juce::Slider& slider) override;

    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override;

    //============================================================================
    // Configuration
    //============================================================================
    void setArcThickness(float thickness);
    void setArcSize(float width, float height);
    void setBackgroundColour(juce::Colour colour);
    void setTrackColour(juce::Colour colour);
    void setThumbSize(float radius);
    void setGradientColours(juce::Colour low, juce::Colour mid, juce::Colour high);

private:
    //============================================================================
    // Helpers
    //============================================================================
    juce::Colour getGradientColour(float normalizedPos) const;

    void drawArcTrack(juce::Graphics& g,
                      const juce::Rectangle<float>& bounds,
                      float startAngle, float endAngle) const;

    void drawFilledArc(juce::Graphics& g,
                       const juce::Rectangle<float>& bounds,
                       float startAngle, float endAngle,
                       float sliderPos) const;

    void drawThumb(juce::Graphics& g,
                   const juce::Point<float>& centre,
                   const juce::Rectangle<float>& arcBounds,
                   float angle, float sliderPos) const;

    void drawValueText(juce::Graphics& g,
                       const juce::Rectangle<float>& bounds,
                       float sliderPos) const;

    //============================================================================
    // Members
    //============================================================================
    float arcThickness  = 6.0f;
    float arcWidth      = 200.0f;
    float arcHeight     = 174.0f;
    float thumbRadius   = 8.0f;

    juce::Colour backgroundColour   = juce::Colours::white;
    juce::Colour trackColour        = juce::Colour(0xFFE0E0E0);
    juce::Colour thumbBorderColour  = juce::Colours::white;

    juce::Colour gradientLow  = juce::Colour(0xFF22C55E);  // Green
    juce::Colour gradientMid  = juce::Colour(0xFFEAB308);  // Yellow
    juce::Colour gradientHigh = juce::Colour(0xFFEF4444);  // Red

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};
