#pragma once

#include <QWindow>

namespace Msg::Renderer {
class Handle;
}

namespace Msg {
struct CreateWindowInfo;
}

namespace Msg {
class QWindow : public ::QWindow {
public:
    QWindow(const Renderer::Handle& a_Renderer, const CreateWindowInfo& a_Info);
};
}