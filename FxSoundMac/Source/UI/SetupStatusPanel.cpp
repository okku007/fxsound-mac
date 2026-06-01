#include "SetupStatusPanel.h"

SetupStatusPanel::SetupStatusPanel()
{
    statusLabel.setJustificationType(juce::Justification::topLeft);
    statusLabel.setMinimumHorizontalScale(1.0f);
    addAndMakeVisible(statusLabel);
}

void SetupStatusPanel::setStatus(const AudioEngineStatus& status)
{
    statusLabel.setText(status.userMessage(), juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId,
                          status.isHealthy() ? juce::Colours::limegreen
                                             : juce::Colours::orange);
}

void SetupStatusPanel::resized()
{
    statusLabel.setBounds(getLocalBounds().reduced(8));
}
