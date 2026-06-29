#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/**
 * Custom LookAndFeel for a rotary knob based on the reference SVG.
 *
 * Design spec from SVG:
 *   - Background: white (#FFFFFF)
 *   - Arc track: grey (#999999), thick stroke with rounded caps
 *   - Filled arc: Green (#2ca02c) -> Yellow (#ffd42a) -> Red (#c83737)
 *   - Arc span: 0 at 7 o'clock, 100 at 5 o'clock (bottom arc via 6 o'clock)
 *   - Thumb: circular handle on the arc for dragging
 *   - Arc size: 200 x 174 px effective area
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
                   const juce::Rectangle<float>& arcBounds,
                   float angle, float sliderPos) const;

    void drawValueText(juce::Graphics& g,
                       const juce::Rectangle<float>& bounds,
                       float sliderPos) const;

    //============================================================================
    // Members
    //============================================================================
    float arcThickness  = 10.0f;   // Thicker arc matching SVG reference
    float arcWidth      = 200.0f;
    float arcHeight     = 174.0f;
    float thumbRadius   = 14.0f;   // Prominent circular drag handle

    juce::Colour backgroundColour   = juce::Colours::white;
    juce::Colour trackColour        = juce::Colour(0xFF999999);  // Grey from SVG

    // Gradient colours from SVG reference
    juce::Colour gradientLow  = juce::Colour(0xFF2ca02c);  // Green
    juce::Colour gradientMid  = juce::Colour(0xFFffd42a);  // Yellow
    juce::Colour gradientHigh = juce::Colour(0xFFc83737);  // Red

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};
