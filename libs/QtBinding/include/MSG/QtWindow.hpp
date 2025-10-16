#pragma once

#include <QQuickView>

namespace Msg {
/**
 * @brief Use this window as parent for all QObjects.
 * It ensures the Qt context is compatible with Msg::Renderer.
 */
class QtWindow : public QQuickView {
    Q_OBJECT
    QML_NAMED_ELEMENT(MSGWindow)
public:
    QtWindow();
    void exposeEvent(QExposeEvent*) override;
    /** @brief the QML element is registered under MSG and is named MSGWindow */
    static void RegisterQMLType();
};
}