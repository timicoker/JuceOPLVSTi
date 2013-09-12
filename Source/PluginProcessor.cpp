#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "EnumFloatParameter.h"
#include "IntFloatParameter.h"

//==============================================================================
JuceOplvstiAudioProcessor::JuceOplvstiAudioProcessor()
{
	Opl = new Hiopl(44100);	// 1 second at 44100
	Opl->SetSampleRate(44100);
	Opl->EnableWaveformControl();
	Opl->EnableSustain(1, 1);
	Opl->EnableSustain(1, 2);

	const String waveforms[] = {"Sine", "Half Sine", "Abs Sine", "Quarter Sine"};
	params.push_back(new EnumFloatParameter("Carrier Wave",
		StringArray(waveforms, sizeof(waveforms)/sizeof(String)))
	); setParameter(params.size()-1, 0.0f);
	params.push_back(new EnumFloatParameter("Modulator Wave",
		StringArray(waveforms, sizeof(waveforms)/sizeof(String)))
	); setParameter(params.size()-1,0.0f);
	/*
	const String levels[] = {"-0.75 dB", "-1.5 dB", "-3 dB", "-6 dB", "-12 dB", "-24 dB"};
	params.push_back(new EnumFloatParameter("Carrier Attenuation",
		StringArray(levels, sizeof(levels)/sizeof(String)))
	); setParameter(params.size()-1,0.0f);
	params.push_back(new EnumFloatParameter("Modulator Attenuation",
		StringArray(levels, sizeof(levels)/sizeof(String)))
	); setParameter(params.size()-1,0.75f);
	*/
	params.push_back(new IntFloatParameter("Carrier Attenuation", 0, 63));
	setParameter(params.size()-1,0.0f);
	params.push_back(new IntFloatParameter("Modulator Attenuation", 0, 63));
	setParameter(params.size()-1,0.75f);

	const String frq_multipliers[] = {
		"x0.5", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x10", "x12", "x12", "x15", "x15"
	};
	params.push_back(new EnumFloatParameter("Carrier Frequency Multiplier",
		StringArray(frq_multipliers, sizeof(frq_multipliers)/sizeof(String)))
	); setParameter(params.size()-1, 0.08f);
	params.push_back(new EnumFloatParameter("Modulator Frequency Multiplier",
		StringArray(frq_multipliers, sizeof(frq_multipliers)/sizeof(String)))
	); setParameter(params.size()-1, 0.15f);

	params.push_back(new IntFloatParameter("Modulator Feedback", 0, 7));
	setParameter(params.size()-1,0.0f);
	
	params.push_back(new IntFloatParameter("Carrier Attack", 0, 15));
	setParameter(params.size()-1,0.5f);
	params.push_back(new IntFloatParameter("Carrier Decay", 0, 15));
	setParameter(params.size()-1,0.25f);
	params.push_back(new IntFloatParameter("Carrier Sustain", 0, 15));
	setParameter(params.size()-1,0.125f);
	params.push_back(new IntFloatParameter("Carrier Release", 0, 15));
	setParameter(params.size()-1,0.5f);
	params.push_back(new IntFloatParameter("Modulator Attack", 0, 15));
	setParameter(params.size()-1,0.5f);
	params.push_back(new IntFloatParameter("Modulator Decay", 0, 15));
	setParameter(params.size()-1,0.25f);
	params.push_back(new IntFloatParameter("Modulator Sustain", 0, 15));
	setParameter(params.size()-1,0.25f);
	params.push_back(new IntFloatParameter("Modulator Release", 0, 15));
	setParameter(params.size()-1,0.25f);

}

JuceOplvstiAudioProcessor::~JuceOplvstiAudioProcessor()
{
}

//==============================================================================
const String JuceOplvstiAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int JuceOplvstiAudioProcessor::getNumParameters()
{
	return params.size();
}

float JuceOplvstiAudioProcessor::getParameter (int index)
{
    return params[index]->getParameter();
}

void JuceOplvstiAudioProcessor::setParameter (int index, float newValue)
{
	FloatParameter* p = params[index];
	p->setParameter(newValue);
	String name = p->getName();
	int osc = 2;	// Carrier
	if (name.startsWith("Modulator")) {
		osc = 1;
	}
	if (name.endsWith("Wave")) {
		Opl->SetWaveform(1, osc, (Waveform)((EnumFloatParameter*)p)->getParameterIndex());
	} else if (name.endsWith("Attenuation")) {
		//Opl->SetAttenuation(1, osc, 0x1<<((EnumFloatParameter*)p)->getParameterIndex());
		Opl->SetAttenuation(1, osc, ((IntFloatParameter*)p)->getParameterValue());
	} else if (name.endsWith("Frequency Multiplier")) {
		Opl->SetFrequencyMultiple(1, osc, (FreqMultiple)((EnumFloatParameter*)p)->getParameterIndex());
	} else if (name.endsWith("Attack")) {
		Opl->SetEnvelopeAttack(1, osc, ((IntFloatParameter*)p)->getParameterValue());
	} else if (name.endsWith("Decay")) {
		Opl->SetEnvelopeDecay(1, osc, ((IntFloatParameter*)p)->getParameterValue());
	} else if (name.endsWith("Sustain")) {
		Opl->SetEnvelopeSustain(1, osc, ((IntFloatParameter*)p)->getParameterValue());
	} else if (name.endsWith("Release")) {
		Opl->SetEnvelopeRelease(1, osc, ((IntFloatParameter*)p)->getParameterValue());
	} else if (name.endsWith("Feedback")) {
		Opl->SetModulatorFeedback(1, ((IntFloatParameter*)p)->getParameterValue());
	}

}

const String JuceOplvstiAudioProcessor::getParameterName (int index)
{
	return params[index]->getName();
}

const String JuceOplvstiAudioProcessor::getParameterText (int index)
{
    return params[index]->getParameterText();
}

const String JuceOplvstiAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String JuceOplvstiAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool JuceOplvstiAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool JuceOplvstiAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool JuceOplvstiAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JuceOplvstiAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JuceOplvstiAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double JuceOplvstiAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JuceOplvstiAudioProcessor::getNumPrograms()
{
    return 0;
}

int JuceOplvstiAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JuceOplvstiAudioProcessor::setCurrentProgram (int index)
{
}

const String JuceOplvstiAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void JuceOplvstiAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void JuceOplvstiAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	//Opl->SetSampleRate((int)sampleRate);
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void JuceOplvstiAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void JuceOplvstiAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	buffer.clear(0, 0, buffer.getNumSamples());
	MidiBuffer::Iterator midi_buffer_iterator(midiMessages);

	MidiMessage midi_message(0);
	int sample_number;
	while (midi_buffer_iterator.getNextEvent(midi_message,sample_number)) {
		if (midi_message.isNoteOn()) {
			//note on at sample_number samples after 
			//the beginning of the current buffer
			float noteHz = (float)MidiMessage::getMidiNoteInHertz(midi_message.getNoteNumber());
			Opl->KeyOn(1, noteHz);
		}
		else if (midi_message.isNoteOff()) {
			Opl->KeyOff(1);
		}
	}
	Opl->Generate(buffer.getNumSamples(), buffer.getSampleData(0));
}

//==============================================================================
bool JuceOplvstiAudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* JuceOplvstiAudioProcessor::createEditor()
{
    return new JuceOplvstiAudioProcessorEditor (this);
}

//==============================================================================
void JuceOplvstiAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JuceOplvstiAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JuceOplvstiAudioProcessor();
}
