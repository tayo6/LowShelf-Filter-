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

    // Fill white background
    g.fillAll(backgroundColour);

    //========================================================================
    // ARC GEOMETRY
    //========================================================================
    // 0 is at 7 o'clock, 100 is at 5 o'clock.
    // In JUCE angles (0 = 3 o'clock, positive = clockwise):
    //   5 o'clock = +60 degrees  = pi/3
    //   6 o'clock = +90 degrees  = pi/2
    //   7 o'clock = +120 degrees = 2*pi/3
    //
    // The arc is drawn clockwise from 5 o'clock to 7 o'clock via 6 o'clock.
    // arcStart = 60 deg (5 o'clock)  <- this is where value = 100
    // arcEnd   = 120 deg (7 o'clock) <- this is where value = 0
    //
    // As value increases from 0 to 100, the thumb moves from 120 to 60
    // (counter-clockwise along the arc), and the filled arc grows from
    // 7 o'clock toward 5 o'clock.
    //========================================================================
    constexpr float arcStartAngle = juce::MathConstants<float>::pi / 3.0f;        // 60 deg (5 o'clock)
    constexpr float arcEndAngle   = juce::MathConstants<float>::pi * 2.0f / 3.0f; // 120 deg (7 o'clock)

    // Centre the 200x174 arc, shifted down for visual balance
    auto centre = bounds.getCentre();
    float arcX = centre.x - arcWidth * 0.5f;
    float arcY = centre.y - arcHeight * 0.5f + 15.0f;
    auto arcBounds = juce::Rectangle<float>(arcX, arcY, arcWidth, arcHeight);

    // Current thumb angle: moves from 120 (7 o'clock) at 0
    // to 60 (5 o'clock) at 100
    float currentAngle = arcEndAngle
                       - (arcEndAngle - arcStartAngle) * sliderPosProportional;

    //========================================================================
    // DRAW ARC TRACK (background / unfilled portion)
    //========================================================================
    drawArcTrack(g, arcBounds, arcStartAngle, arcEndAngle);

    //========================================================================
    // DRAW FILLED ARC (gradient from 7 o'clock toward 5 o'clock)
    //========================================================================
    if (sliderPosProportional > 0.0f)
        drawFilledArc(g, arcBounds, currentAngle, arcEndAngle, sliderPosProportional);

    //========================================================================
    // DRAW CIRCULAR THUMB HANDLE
    //========================================================================
    drawThumb(g, arcBounds, currentAngle, sliderPosProportional);

    //========================================================================
    // DRAW VALUE TEXT
    //========================================================================
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
                                       float sliderPos) const
{
    // Draw the filled arc as segmented strokes for smooth gradient
    // The arc grows from 7 o'clock (endAngle) toward 5 o'clock (startAngle)
    const int numSegments = juce::jmax(30, static_cast<int>((endAngle - startAngle) * 40.0f));
    float angleStep = (endAngle - startAngle) / numSegments;

    for (int i = 0; i < numSegments; ++i)
    {
        float segStart = startAngle + angleStep * i;
        float segEnd   = juce::jmin(segStart + angleStep, endAngle);

        juce::Path seg;
        seg.addArc(bounds.getX(), bounds.getY(),
                   bounds.getWidth(), bounds.getHeight(),
                   segStart, segEnd, true);

        // Colour based on position along the arc
        // 0 = at 7 o'clock (endAngle), 1 = at 5 o'clock (startAngle)
        // But sliderPos is 0 at 7 o'clock, 1 at 5 o'clock
        // So we map segment position to the overall slider position
        float segPos = static_cast<float>(i) / numSegments;
        float colourT = sliderPos * segPos; // 0 at 7 o'clock, 1 at 5 o'clock

        g.setColour(getGradientColour(colourT));
        g.strokePath(seg, juce::PathStrokeType(arcThickness,
                                                  juce::PathStrokeType::curved,
                                                  juce::PathStrokeType::rounded));
    }
}

//==============================================================================
void CustomLookAndFeel::drawThumb(juce::Graphics& g,
                                   const juce::Rectangle<float>& arcBounds,
                                   float angle, float sliderPos) const
{
    // Calculate thumb position on the elliptical arc
    float rx = arcBounds.getWidth() * 0.5f;
    float ry = arcBounds.getHeight() * 0.5f;
    float cx = arcBounds.getCentreX();
    float cy = arcBounds.getCentreY();

    float thumbX = cx + rx * std::cos(angle);
    float thumbY = cy + ry * std::sin(angle);

    // Outer glow/shadow for depth
    float glowRadius = thumbRadius * 1.8f;
    juce::Colour glowColour = juce::Colours::black.withAlpha(0.15f);
    g.setColour(glowColour);
    g.fillEllipse(thumbX - glowRadius, thumbY - glowRadius,
                  glowRadius * 2.0f, glowRadius * 2.0f);

    // Outer ring (white border)
    float outerRadius = thumbRadius + 3.0f;
    g.setColour(juce::Colours::white);
    g.fillEllipse(thumbX - outerRadius, thumbY - outerRadius,
                  outerRadius * 2.0f, outerRadius * 2.0f);

    // Main thumb body (gradient colour)
    juce::Colour thumbColour = getGradientColour(sliderPos);
    g.setColour(thumbColour);
    g.fillEllipse(thumbX - thumbRadius, thumbY - thumbRadius,
                  thumbRadius * 2.0f, thumbRadius * 2.0f);

    // Inner highlight (3D effect)
    float highlightRadius = thumbRadius * 0.55f;
    g.setColour(juce::Colours::white.withAlpha(0.45f));
    g.fillEllipse(thumbX - highlightRadius,
                  thumbY - highlightRadius - 2.0f,
                  highlightRadius * 2.0f, highlightRadius * 2.0f);

    // Centre dot (darker shade for detail)
    float dotRadius = thumbRadius * 0.25f;
    g.setColour(thumbColour.darker(0.3f));
    g.fillEllipse(thumbX - dotRadius, thumbY - dotRadius,
                  dotRadius * 2.0f, dotRadius * 2.0f);
}

//==============================================================================
void CustomLookAndFeel::drawValueText(juce::Graphics& g,
                                       const juce::Rectangle<float>& bounds,
                                       float sliderPos) const
{
    // Value text below the arc
    auto textBounds = bounds;
    textBounds.removeFromTop(bounds.getHeight() * 0.72f);

    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(20.0f, juce::Font::bold));

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
