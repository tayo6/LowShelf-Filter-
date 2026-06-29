#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LowShelfFilterAudioProcessor::LowShelfFilterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                       .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                       ),
                       apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

LowShelfFilterAudioProcessor::~LowShelfFilterAudioProcessor() = default;

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
LowShelfFilterAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        getLowShelfParamID(),
        "Low Shelf",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel("%")
            .withStringFromValueFunction([](float value, int) {
                return juce::String(static_cast<int>(value));
            })
    ));

    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String LowShelfFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LowShelfFilterAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool LowShelfFilterAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool LowShelfFilterAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double LowShelfFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LowShelfFilterAudioProcessor::getNumPrograms()
{
    return 1;
}

int LowShelfFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LowShelfFilterAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String LowShelfFilterAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void LowShelfFilterAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void LowShelfFilterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    lowShelfFilter.prepare(spec);

    *lowShelfFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate, 100.0f, 0.707f, 1.0f);
}

void LowShelfFilterAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LowShelfFilterAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void LowShelfFilterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                 juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    updateFilter();

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    lowShelfFilter.process(context);
}

//==============================================================================
void LowShelfFilterAudioProcessor::updateFilter()
{
    float paramValue = apvts.getRawParameterValue(getLowShelfParamID())->load();

    // 0% -> 0 dB, 100% -> -12 dB
    float gainDb = -12.0f * (paramValue / 100.0f);
    float gainLinear = juce::Decibels::decibelsToGain(gainDb);

    *lowShelfFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        currentSampleRate, 100.0f, 0.707f, gainLinear);
}

//==============================================================================
bool LowShelfFilterAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* LowShelfFilterAudioProcessor::createEditor()
{
    return new LowShelfFilterAudioProcessorEditor(*this);
}

//==============================================================================
void LowShelfFilterAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void LowShelfFilterAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LowShelfFilterAudioProcessor();
}
