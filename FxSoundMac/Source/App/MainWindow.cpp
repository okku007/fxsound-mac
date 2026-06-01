#include "MainWindow.h"

MainWindow::MainWindow(const juce::String& name)
    : juce::DocumentWindow(name,
        juce::Desktop::getInstance().getDefaultLookAndFeel()
            .findColour(juce::ResizableWindow::backgroundColourId),
        juce::DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    auto* placeholder = new juce::Label({}, "FxSoundMac");
    placeholder->setJustificationType(juce::Justification::centred);
    placeholder->setSize(480, 320);
    setContentOwned(placeholder, true);
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
