#!/usr/bin/env python3
"""
Patch juce_Windowing_mac.mm to guard CGWindowListCreateImage behind
MAC_OS_X_VERSION_MAX_ALLOWED < 150000.  That API was removed in macOS 15 / Xcode 26 SDK.
"""
import sys, pathlib, re

target = pathlib.Path(sys.argv[1])
text = target.read_text()

OLD = """\
static Image createNSWindowSnapshot (NSWindow* nsWindow)
{
    JUCE_AUTORELEASEPOOL
    {
        // CGWindowListCreateImage is replaced by functions in the ScreenCaptureKit framework, but
        // that framework is only available from macOS 12.3 onwards.
        // A suitable @available check should be added once the minimum build OS is 12.3 or greater,
        // so that ScreenCaptureKit can be weak-linked.
       #if defined (MAC_OS_VERSION_14_0) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_14_0
        JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wdeprecated-declarations")
        #define JUCE_DEPRECATION_IGNORED 1
       #endif

        CGImageRef screenShot = CGWindowListCreateImage (CGRectNull,
                                                         kCGWindowListOptionIncludingWindow,
                                                         (CGWindowID) [nsWindow windowNumber],
                                                         kCGWindowImageBoundsIgnoreFraming);

       #if JUCE_DEPRECATION_IGNORED
        JUCE_END_IGNORE_WARNINGS_GCC_LIKE
        #undef JUCE_DEPRECATION_IGNORED
       #endif

        NSBitmapImageRep* bitmapRep = [[NSBitmapImageRep alloc] initWithCGImage: screenShot];

        Image result (Image::ARGB, (int) [bitmapRep size].width, (int) [bitmapRep size].height, true);

        selectImageForDrawing (result);
        [bitmapRep drawAtPoint: NSMakePoint (0, 0)];
        releaseImageAfterDrawing();

        [bitmapRep release];
        CGImageRelease (screenShot);

        return result;
    }
}"""

NEW = """\
static Image createNSWindowSnapshot (NSWindow* nsWindow)
{
   #if MAC_OS_X_VERSION_MAX_ALLOWED < 150000
    JUCE_AUTORELEASEPOOL
    {
        // CGWindowListCreateImage is replaced by functions in the ScreenCaptureKit framework, but
        // that framework is only available from macOS 12.3 onwards.
        // A suitable @available check should be added once the minimum build OS is 12.3 or greater,
        // so that ScreenCaptureKit can be weak-linked.
       #if defined (MAC_OS_VERSION_14_0) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_VERSION_14_0
        JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wdeprecated-declarations")
        #define JUCE_DEPRECATION_IGNORED 1
       #endif

        CGImageRef screenShot = CGWindowListCreateImage (CGRectNull,
                                                         kCGWindowListOptionIncludingWindow,
                                                         (CGWindowID) [nsWindow windowNumber],
                                                         kCGWindowImageBoundsIgnoreFraming);

       #if JUCE_DEPRECATION_IGNORED
        JUCE_END_IGNORE_WARNINGS_GCC_LIKE
        #undef JUCE_DEPRECATION_IGNORED
       #endif

        NSBitmapImageRep* bitmapRep = [[NSBitmapImageRep alloc] initWithCGImage: screenShot];

        Image result (Image::ARGB, (int) [bitmapRep size].width, (int) [bitmapRep size].height, true);

        selectImageForDrawing (result);
        [bitmapRep drawAtPoint: NSMakePoint (0, 0)];
        releaseImageAfterDrawing();

        [bitmapRep release];
        CGImageRelease (screenShot);

        return result;
    }
   #else
    // CGWindowListCreateImage was removed in macOS 15 SDK. Return empty image.
    ignoreUnused (nsWindow);
    return {};
   #endif
}"""

if NEW in text:
    print(f"Already patched: {target}")
    sys.exit(0)

if OLD not in text:
    print("ERROR: patch target not found — JUCE source may have changed", file=sys.stderr)
    sys.exit(1)

patched = text.replace(OLD, NEW, 1)
target.write_text(patched)
print(f"Patched {target}")
