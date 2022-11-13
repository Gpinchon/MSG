/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:08
*/
/*
#include "TextureArray.hpp"
#include "Debug.hpp" // for glCheckError
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type

TextureArray::TextureArray(const std::string& name, glm::ivec2 s, GLenum target, GLenum fi, unsigned capacity)
    : Texture(name)
{
    _size = s;
    _target = target;
    _internal_format = fi;
    _capacity = capacity;
    _array.resize(capacity);
}

std::shared_ptr<TextureArray> TextureArray::Create(const std::string& name, glm::ivec2 s, GLenum target, GLenum fi, unsigned capacity)
{
    auto t = Component::Create<TextureArray>(name, s, target, fi, capacity);
    glGenTextures(1, &t->_glid);
    glObjectLabel(GL_TEXTURE, t->_glid, -1, t->Name().c_str());
    glBindTexture(t->_target, t->_glid);
    glTexStorage3D(t->_target, 1, t->_internal_format, t->_size.x, t->_size.y, t->_capacity);
    glBindTexture(t->_target, 0);
    t->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    t->set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    t->set_parameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    t->set_parameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    t->_array.resize(capacity);
    Texture::Add(t);
    glCheckError();
    return (t);
}

void TextureArray::set(std::shared_ptr<Texture> texture, int index)
{
    _array.at(index) = texture;
}

void TextureArray::load()
{
    if (_loaded)
        return;
    glBindTexture(_target, _glid);
    for (auto index = 0u; index < _array.size(); index++) {
        auto t = _array.at(index);
        if (nullptr == t || nullptr == t->data())
            continue;
        GLint level = 0; // Which mipmap is to be used in texture ?
        GLint xoffset = 0;
        GLint yoffset = 0;
        GLint zoffset = index; // Which texture is to be set ?
        GLsizei depth = 1; // Set only one texture
        glTexSubImage3D(_target, level, xoffset, yoffset, zoffset, t->size().x, t->size().y, depth, t->format(), t->data_format(), t->data());
    }
    glBindTexture(_target, 0);
    _loaded = true;
    glCheckError();
}
*/