#include "CustomLookAndFeel.h"

//==============================================================================
CustomLookAndFeel::CustomLookAndFeel()
{
    setColour(juce::Slider::thumbColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::trackColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::white);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

CustomLookAndFeel::~CustomLookAndFeel() = default;

//==============================================================================
void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                          int x, int y,
                                          int width, int height,
                                          float sliderPosProportional,
                                          float /*rotaryStartAngle*/,
                                          float /*rotaryEndAngle*/,
                                          juce::Slider& /*slider*/)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
    auto centre = bounds.getCentre();

    // White background
    g.fillAll(backgroundColour);

    //========================================================================
    // Arc geometry: bottom arc from ~5 o'clock to ~7 o'clock
    //
    // JUCE angle convention: 0 = 3 o'clock, positive = clockwise
    //   5 o'clock = +60 deg   ->  pi/3
    //   6 o'clock = +90 deg   ->  pi/2
    //   7 o'clock = +120 deg  ->  2*pi/3
    //
    // We use a 90-degree arc (45 -> 135) for comfortable interaction while
    // keeping the ends near the 5 and 7 o'clock positions as requested.
    //========================================================================
    constexpr float arcStartAngle = juce::MathConstants<float>::pi / 4.0f;        // 45 deg
    constexpr float arcEndAngle   = juce::MathConstants<float>::pi * 3.0f / 4.0f; // 135 deg

    // Centre the 200x174 arc, shifted down slightly for visual balance
    float arcX = centre.x - arcWidth  * 0.5f;
    float arcY = centre.y - arcHeight * 0.5f + 10.0f;
    auto arcBounds = juce::Rectangle<float>(arcX, arcY, arcWidth, arcHeight);

    // Current angle from slider position [0,1]
    float currentAngle = arcStartAngle
                       + (arcEndAngle - arcStartAngle) * sliderPosProportional;

    // Draw track, filled arc, thumb, and value text
    drawArcTrack(g, arcBounds, arcStartAngle, arcEndAngle);

    if (sliderPosProportional > 0.0f)
        drawFilledArc(g, arcBounds, arcStartAngle, currentAngle, sliderPosProportional);

    drawThumb(g, centre, arcBounds, currentAngle, sliderPosProportional);
    drawValueText(g, bounds, sliderPosProportional);
}

//==============================================================================
void CustomLookAndFeel::drawArcTrack(juce::Graphics& g,
                                      const juce::Rectangle<float>& bounds,
                                      float startAngle, float endAngle) const
{
    juce::Path track;
    track.addArc(bounds.getX(), bounds.getY(),
                 bounds.getWidth(), bounds.getHeight(),
                 startAngle, endAngle, true);

    g.setColour(trackColour);
    g.strokePath(track, juce::PathStrokeType(arcThickness,
                                              juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded));
}

//==============================================================================
void CustomLookAndFeel::drawFilledArc(juce::Graphics& g,
                                       const juce::Rectangle<float>& bounds,
                                       float startAngle, float endAngle,
                                       float /*sliderPos*/) const
{
    const int numSegments = juce::jmax(24, static_cast<int>((endAngle - startAngle) * 40.0f));
    float angleStep = (endAngle - startAngle) / numSegments;

    for (int i = 0; i < numSegments; ++i)
    {
        float segStart = startAngle + angleStep * i;
        float segEnd   = juce::jmin(segStart + angleStep, endAngle);

        juce::Path seg;
        seg.addArc(bounds.getX(), bounds.getY(),
                   bounds.getWidth(), bounds.getHeight(),
                   segStart, segEnd, true);

        float t = static_cast<float>(i) / numSegments;
        g.setColour(getGradientColour(t));
        g.strokePath(seg, juce::PathStrokeType(arcThickness,
                                                  juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));
    }
}

//==============================================================================
void CustomLookAndFeel::drawThumb(juce::Graphics& g,
                                   const juce::Point<float>& /*centre*/,
                                   const juce::Rectangle<float>& arcBounds,
                                   float angle, float sliderPos) const
{
    float rx = arcBounds.getWidth()  * 0.5f;
    float ry = arcBounds.getHeight() * 0.5f;
    float cx = arcBounds.getCentreX();
    float cy = arcBounds.getCentreY();

    float thumbX = cx + rx * std::cos(angle);
    float thumbY = cy + ry * std::sin(angle);

    // Glow / shadow
    float glowR = thumbRadius * 1.6f;
    g.setColour(getGradientColour(sliderPos).withAlpha(0.25f));
    g.fillEllipse(thumbX - glowR, thumbY - glowR, glowR * 2.0f, glowR * 2.0f);

    // Body
    g.setColour(getGradientColour(sliderPos));
    g.fillEllipse(thumbX - thumbRadius, thumbY - thumbRadius,
                  thumbRadius * 2.0f, thumbRadius * 2.0f);

    // White border (matches SVG rounded-cap aesthetic)
    g.setColour(thumbBorderColour);
    g.drawEllipse(thumbX - thumbRadius, thumbY - thumbRadius,
                  thumbRadius * 2.0f, thumbRadius * 2.0f, 2.0f);

    // Inner highlight
    float innerR = thumbRadius * 0.45f;
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.fillEllipse(thumbX - innerR, thumbY - innerR - 1.0f,
                  innerR * 2.0f, innerR * 2.0f);
}

//==============================================================================
void CustomLookAndFeel::drawValueText(juce::Graphics& g,
                                       const juce::Rectangle<float>& bounds,
                                       float sliderPos) const
{
    auto textBounds = bounds;
    textBounds.removeFromTop(bounds.getHeight() * 0.72f);

    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(18.0f, juce::Font::bold));

    int value = static_cast<int>(sliderPos * 100.0f);
    g.drawText(juce::String(value), textBounds, juce::Justification::centred, false);
}

//==============================================================================
juce::Slider::SliderLayout CustomLookAndFeel::getSliderLayout(juce::Slider& slider)
{
    juce::Slider::SliderLayout layout;
    layout.sliderBounds = slider.getLocalBounds();
    layout.textBoxBounds = juce::Rectangle<int>();
    return layout;
}

//==============================================================================
juce::Colour CustomLookAndFeel::getGradientColour(float normalizedPos) const
{
    normalizedPos = juce::jlimit(0.0f, 1.0f, normalizedPos);

    if (normalizedPos < 0.5f)
        return gradientLow.interpolatedWith(gradientMid, normalizedPos * 2.0f);

    return gradientMid.interpolatedWith(gradientHigh, (normalizedPos - 0.5f) * 2.0f);
}

//==============================================================================
// Setters
//==============================================================================
void CustomLookAndFeel::setArcThickness(float thickness)
{
    arcThickness = juce::jmax(1.0f, thickness);
}

void CustomLookAndFeel::setArcSize(float width, float height)
{
    arcWidth  = juce::jmax(10.0f, width);
    arcHeight = juce::jmax(10.0f, height);
}

void CustomLookAndFeel::setBackgroundColour(juce::Colour colour)
{
    backgroundColour = colour;
}

void CustomLookAndFeel::setTrackColour(juce::Colour colour)
{
    trackColour = colour;
}

void CustomLookAndFeel::setThumbSize(float radius)
{
    thumbRadius = juce::jmax(2.0f, radius);
}

void CustomLookAndFeel::setGradientColours(juce::Colour low, juce::Colour mid, juce::Colour high)
{
    gradientLow  = low;
    gradientMid  = mid;
    gradientHigh = high;
}
