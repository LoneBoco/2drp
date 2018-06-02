#include <algorithm>
#include <cstring>
#include "client/render/resources/Image.h"

using namespace tdrp::render;

Image::Image(uint32_t width, uint32_t height, PixelFormat format) :
  m_format(format) {

  Resize(width, height);
}

Image::~Image() {
  Free();
}

void Image::Resize(uint32_t width, uint32_t height) {
  // Free the existing bitmap
  Free();

  m_width = width;
  m_stride = width * m_format.GetWidth();
  m_height = height;

  // Allocate the new buffer
  m_data = (unsigned char*)malloc(m_width * m_height * m_format.GetWidth());
}

void Image::SetPixels(const unsigned char* buffer) {
  memcpy(m_data, buffer, m_width * m_height * m_format.GetWidth());
}

void Image::SetPixels(
    uint32_t srcX, uint32_t srcY, uint32_t srcW, uint32_t srcH, uint32_t stride,
    uint32_t dstX, uint32_t dstY,
    const unsigned char* buffer
) {
  // TODO: unsafe xD overruns!!
  
  // Copy over a partial rect from the buffer passed in
  // Pixel formats must match :)
  const unsigned char* readPtr  = &buffer[srcX * m_format.GetWidth() + stride * srcY];
  unsigned char* writePtr = &m_data[dstX * m_format.GetWidth() + m_stride * dstY];

  // Naive prevent overrun
  const uint32_t byteWidth  = std::min(srcW, m_width  - srcX) * m_format.GetWidth();
  const uint32_t height     = std::min(srcH, m_height - srcY);

  for (int y=0;y<height;++y) {
    memcpy(writePtr, readPtr, byteWidth);

    writePtr += m_width * m_format.GetWidth();
    readPtr  += stride  * m_format.GetWidth();
  }
}

void Image::Free() {
  if (m_data) {
    free(m_data);

    m_data = nullptr;
  }
}
