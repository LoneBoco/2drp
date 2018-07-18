#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "client/render/PixelFormat.h"

namespace tdrp {
  namespace render {
    class Image {
    public:
      Image(uint32_t width, uint32_t height, PixelFormat format);
      ~Image();

      Image(const Image&) = delete;
      Image(Image&&) = delete;
      Image& operator=(const Image&) = delete;

      void Resize(uint32_t width, uint32_t height);
      void SetPixels(const unsigned char* buffer);
      void SetPixels(
          uint32_t srcX, uint32_t srcY, uint32_t srcW, uint32_t srcH, uint32_t stride,
          uint32_t dstX, uint32_t dstY,
          const unsigned char* buffer
      );

      const unsigned char* GetPixels() const { return m_data; };
      PixelFormat GetFormat() const { return m_format; };
      uint32_t GetWidth() const { return m_width; };
      uint32_t GetHeight() const { return m_height; };

    private:
      void Free();

      uint32_t m_width;
      uint32_t m_stride;
      uint32_t m_height;

      PixelFormat m_format;
      unsigned char* m_data;
    };
  }
}
