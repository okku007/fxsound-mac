#include "MainWindow.h"
#include "../UI/MainComponent.h"
#include "../Support/AppSettings.h"

MainWindow::MainWindow(const juce::String& name)
    : juce::DocumentWindow(name,
        juce::Desktop::getInstance().getDefaultLookAndFeel()
            .findColour(juce::ResizableWindow::backgroundColourId),
        juce::DocumentWindow::minimiseButton | juce::DocumentWindow::closeButton)
{
    setUsingNativeTitleBar(true);
    setContentOwned(new MainComponent(), true);
    setResizable(false, false);
    centreWithSize(getWidth(), getHeight());
    setVisible(true);
}

void MainWindow::closeButtonPressed()
{
    if (AppSettings::closeQuitsApp())
    {
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }
    else
    {
        // Keep the app running with no visible window. Hiding the whole app means
        // macOS re-shows it (this window included) when its Dock icon is clicked —
        // no custom reopen handler required.
        juce::Process::hide();
    }
}
