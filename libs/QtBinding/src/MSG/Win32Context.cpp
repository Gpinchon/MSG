#include <MSG/OGLContext/Win32/PlatformCtx.hpp>
#include <MSG/OGLContext/Win32/WGL.hpp>
#include <MSG/OGLContext/Win32/Win32.hpp>

#include <QOpenGLContext>

Platform::Ctx* CreateMSGContext(const QOpenGLContext* a_SharedContext)
{
    auto qNativeInterface = a_SharedContext->nativeInterface<QNativeInterface::QWGLContext>();
    auto sharedHGLRC      = qNativeInterface->nativeContext();
    auto wndclassWrapper  = Win32::WNDClassWrapper::Create(Win32::DefaultWindowClassName);
    auto hwndWrapper      = Win32::HWNDWrapper::Create(wndclassWrapper, "");
    auto hdcWrapper       = Win32::HDCWrapper::Create(hwndWrapper);
    return new Platform::Ctx(hdcWrapper, sharedHGLRC, true);
}
