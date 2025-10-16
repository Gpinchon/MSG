#include <MSG/QtWindow.hpp>

#include <MSG/OGLContext/Win32/PlatformCtx.hpp>
#include <MSG/OGLContext/Win32/WGL.hpp>
#include <MSG/OGLContext/Win32/Win32.hpp>

#include <QOpenGLContext>
#include <QQuickGraphicsDevice>
#include <QThread>

QOpenGLContext* CreateOGLContext(std::any a_Window, QOpenGLContext* a_ShareContext = nullptr)
{
    auto hdc = Win32::GetDC(a_Window);
    WGL::InitOGL();
    Win32::SetPixelFormat(hdc, Win32::GetDefaultPixelFormat(hdc));
    auto hwnd  = a_Window;
    auto hglrc = WGL::CreateContext(hdc);
    Win32::ReleaseDC(a_Window, hdc);
    return QNativeInterface::QWGLContext::fromNative(
        std::any_cast<HGLRC>(hglrc),
        std::any_cast<HWND>(hwnd),
        a_ShareContext);
}

Msg::QtWindow::QtWindow()
{
    auto context = CreateOGLContext((HWND)winId());
    setGraphicsApi(QSGRendererInterface::OpenGL);
    setSurfaceType(QWindow::OpenGLSurface);
    setGraphicsDevice(QQuickGraphicsDevice::fromOpenGLContext(context));
    context->moveToThread(nullptr); // remove thread affinity
    connect(
        this, &QQuickWindow::sceneGraphInitialized,
        this, [this, context] {
            // set thread affinity to QSGRenderThread
            context->moveToThread(QThread::currentThread());
        },
        Qt::DirectConnection);
}

void Msg::QtWindow::exposeEvent(QExposeEvent* a_Event)
{
    // this is done to avoid an assert fail during QSGRenderThread initialization
    QGuiApplication::setAttribute(Qt::AA_DontCheckOpenGLContextThreadAffinity, true);
    QQuickView::exposeEvent(a_Event);
    QGuiApplication::setAttribute(Qt::AA_DontCheckOpenGLContextThreadAffinity, false);
}

void Msg::QtWindow::RegisterQMLType()
{
    qmlRegisterType<Msg::QtWindow>("MSG", 1, 0, "MSGWindow");
}
