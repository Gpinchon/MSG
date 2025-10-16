#pragma once

#include <MSG/Renderer/Handles.hpp>

#include <QQuickItem>

namespace Msg {
class QtItem : public QQuickItem {
    Q_OBJECT
    QML_NAMED_ELEMENT(MSGItem)

public:
    QSGNode* updatePaintNode(QSGNode* a_OldNode, UpdatePaintNodeData*) override;
    void componentComplete() override;
    /** @brief the QML element is registered under MSG and is named MSGItem */
    static void RegisterQMLType();

signals:
    /**
     * @brief This signal is emitted after the renderer has been created.
     * You can allocate your resources here.
     * @attention This signal is always emitted from the QSGRenderThread.
     */
    void rendererInitialized();
    /**
     * @brief This signal is emitted when the renderer is about to be destroyed.
     * You should free resources here.
     * @attention This signal is always emitted from the QSGRenderThread
     */
    void rendererInvalidated();
    /**
     * @brief This signal is emitted when the RenderBuffer is updated/recreated
     * @param a_NewSize the new size of the RenderBuffer
     */
    void renderBufferUpdated(const QSize& a_NewSize);

protected:
    Msg::Renderer::Handle renderer;
    Msg::RenderBuffer::Handle renderBuffer;

private slots:
    void createRenderer();
    void destroyRenderer();
    void render();
    void waitRenderer();

private:
    void _updateRenderBuffer();
    QSize _renderBufferSize;
    bool _renderBufferUpdated = false;
};
}
