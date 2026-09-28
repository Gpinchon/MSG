#pragma once

#include <MSG/VKImage.hpp>

namespace Msg::RenderBuffer {
class Impl : public VKImage {
public:
    using VKImage::VKImage;
};
}
