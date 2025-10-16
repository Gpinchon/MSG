#include <MSG/QtItem.hpp>

#include <MSG/Renderer.hpp>
#include <MSG/Renderer/RenderBuffer.hpp>

#include <QSGSimpleTextureNode>

Msg::Renderer::Handle CreateMSGRenderer(const QString& a_Name, QQuickWindow* a_Window);
QSGTexture* MSGRenderBufferToQSGTexture(Msg::RenderBuffer::Handle& a_RenderBuffer, const QSize& a_Size, QQuickWindow* a_Window);

QSGNode* Msg::QtItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    auto node    = static_cast<QSGSimpleTextureNode*>(oldNode);
    bool newNode = false;
    if (oldNode == nullptr) {
        node    = new QSGSimpleTextureNode;
        newNode = true;
    }
    if (newNode || _renderBufferUpdated) {
        if (node->texture() != nullptr)
            delete node->texture();
        node->setTexture(MSGRenderBufferToQSGTexture(renderBuffer, _renderBufferSize, window()));
        node->setOwnsTexture(false);
        node->setTextureCoordinatesTransform(QSGSimpleTextureNode::MirrorVertically);
        node->setRect(boundingRect());
        node->setSourceRect(0, 0, width(), height());
        _renderBufferUpdated = false;
    }
    return node;
}

void Msg::QtItem::componentComplete()
{
    QQuickItem::componentComplete();
    setFlag(QQuickItem::ItemHasContents, true);
    // all these MUST be executed on the QSGRenderThread
    connect(
        window(), &QQuickWindow::sceneGraphInitialized,
        this, &QtItem::createRenderer, Qt::ConnectionType::DirectConnection);
    connect(
        window(), &QQuickWindow::sceneGraphInvalidated,
        this, &QtItem::destroyRenderer, Qt::ConnectionType::DirectConnection);
    // start rendering a bit early to avoid blocking QSGThread
    connect(
        window(), &QQuickWindow::beforeSynchronizing,
        this, &QtItem::render, Qt::ConnectionType::DirectConnection);
    // block QSGRenderThread until Renderer is done
    connect(
        window(), &QQuickWindow::beforeRendering,
        this, &QtItem::waitRenderer, Qt::ConnectionType::DirectConnection);
    // schedule a new updatePaintNode on each frame
    connect(
        window(), &QQuickWindow::afterRendering,
        this, &QQuickItem::update);
    // schedule a new updatePolish on each frame
    connect(
        window(), &QQuickWindow::afterRendering,
        this, &QQuickItem::polish);
}

void Msg::QtItem::RegisterQMLType()
{
    qmlRegisterType<Msg::QtItem>("MSG", 1, 0, "MSGItem");
}

void Msg::QtItem::_updateRenderBuffer()
{
    if (size() != _renderBufferSize) {
        RenderBuffer::CreateRenderBufferInfo rbInfo;
        rbInfo.width  = width();
        rbInfo.height = height();
        renderBuffer  = RenderBuffer::Create(renderer, rbInfo);
        Renderer::SetActiveRenderBuffer(renderer, renderBuffer);
        _renderBufferSize    = QSize(width(), height());
        _renderBufferUpdated = true;
    }
}

void Msg::QtItem::createRenderer()
{
    renderer = CreateMSGRenderer(QGuiApplication::applicationVersion(), window());
    emit rendererInitialized();
}

void Msg::QtItem::destroyRenderer()
{
    emit rendererInvalidated();
    renderBuffer.reset();
    renderer.reset();
}

void Msg::QtItem::render()
{
    _updateRenderBuffer();
    Renderer::Update(renderer);
    Renderer::Render(renderer);
}

void Msg::QtItem::waitRenderer()
{
    // block until current frame is done
    Renderer::WaitGPU(renderer);
}
