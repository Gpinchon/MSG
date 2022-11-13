/*
* @Author: gpinchon
* @Date:   2021-03-25 11:08:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-25 11:08:56
*/

#include <Driver/OpenGL/VertexArray.hpp>
#include <Driver/OpenGL/Buffer.hpp>
#include <Buffer/Accessor.hpp>

#include <GL/glew.h>
#include <array>

namespace OpenGL {
VertexArray::VertexArray()
{
	glCreateVertexArrays(1, &_handle);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &_handle);
}

VertexArray& VertexArray::Bind()
{
	glBindVertexArray(GetHandle());
	return *this;
}

void VertexArray::Done()
{
    BindNone();
}

void VertexArray::BindNone()
{
    glBindVertexArray(0);
}

}
