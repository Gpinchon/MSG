#include <MSG/Renderer.hpp>
#include <MSG/Renderer/RenderBuffer.hpp>

#include <QOpenGLContext>
#include <QQuickWindow>
#include <QSGTexture>

Platform::Ctx* CreateMSGContext(const QOpenGLContext* a_SharedContext);

Msg::Renderer::Handle CreateMSGRenderer(const QString& a_Name, QQuickWindow* a_Window)
{
    auto rendererI     = a_Window->rendererInterface();
    auto sharedContext = reinterpret_cast<QOpenGLContext*>(rendererI->getResource(a_Window, QSGRendererInterface::OpenGLContextResource));
    Msg::Renderer::CreateRendererInfo info;
    info.applicationVersion = QGuiApplication::applicationVersion().toUInt();
    info.name               = a_Name.toStdString();
    if (sharedContext != nullptr)
        info.context = CreateMSGContext(sharedContext);
    Msg::Renderer::RendererSettings settings; // TODO Use QSettings
    return Msg::Renderer::Create(info, settings);
}

QSGTexture* MSGRenderBufferToQSGTexture(Msg::RenderBuffer::Handle& a_RenderBuffer, const QSize& a_Size, QQuickWindow* a_Window)
{
    auto oglHandle = Msg::RenderBuffer::GetNativeHandle(a_RenderBuffer);
    return QNativeInterface::QSGOpenGLTexture::fromNative(
        std::any_cast<unsigned>(oglHandle),
        a_Window, a_Size,
        QQuickWindow::TextureHasAlphaChannel);
}