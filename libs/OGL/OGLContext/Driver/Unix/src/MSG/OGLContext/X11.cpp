#include <MSG/Debug.hpp>
#include <MSG/OGLContext/X11.hpp>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cassert>
#include <iostream>

int ErrorHandler(Display*, XErrorEvent* error)
{
    char errorstring[4096];
    XGetErrorText(error->display, error->error_code, errorstring, sizeof(errorstring));
    std::cerr << "X11 Error : " << errorstring << std::endl;
    abort();
    return 0;
}

std::any X11::CreateDefaultWindow(const std::any& a_X11Display)
{
    auto dpy      = std::any_cast<Display*>(a_X11Display);
    auto scr      = DefaultScreen(dpy);
    auto drawable = XCreateSimpleWindow(
        dpy, DefaultRootWindow(dpy),
        0, 0, /////////////////// X, Y
        1, 1, /////////////////// Width, Height
        0, ////////////////////// BorderWidth
        XBlackPixel(dpy, scr), // BorderColor
        XBlackPixel(dpy, scr) ///  BackgroupdColor
    );
    checkErrorFatal(drawable == 0, "XCreateWindow failed");
    return drawable;
}

std::any X11::OpenDisplay(const char* a_DisplayName)
{
    XSetErrorHandler(ErrorHandler);
    auto display = XOpenDisplay(a_DisplayName);
    checkErrorFatal(display == nullptr, "XOpenDisplay failed");
    return display;
}

void X11::CloseDisplay(const std::any& a_Display)
{
    auto display = std::any_cast<Display*>(a_Display);
    XCloseDisplay(display);
}

X11::DisplayWrapper::DisplayWrapper(const char* a_DisplayName)
    : handle(OpenDisplay(a_DisplayName))
{
}

X11::DisplayWrapper::~DisplayWrapper() { CloseDisplay(handle); }

X11::WindowWrapper::WindowWrapper(const std::shared_ptr<DisplayWrapper>& a_Display)
    : display(a_Display)
    , handle(CreateDefaultWindow(display->handle))
{
}

X11::WindowWrapper::~WindowWrapper()
{
    auto dpy      = std::any_cast<Display*>(display->handle);
    auto drawable = std::any_cast<XID>(handle);
    XDestroyWindow(dpy, drawable);
}