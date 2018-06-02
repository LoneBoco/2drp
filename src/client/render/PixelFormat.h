#pragma once

#include "engine/Enum.h"

namespace tdrp {
  namespace render {
    // Psuedo-enum
    struct PixelFormat {
      ENUM_ENABLE(PixelFormat)

      ENUM(RGBA_8888) // 4-component, 8-bpp (32-bit)
      ENUM(RGB_888)   // 3-component, 8-bpp (24-bit)
      ENUM(R_8)       // 1-component, 8-bpp (8-bit)

      uint32_t GetWidth() {
        // Switch on the underlying value to get the byte width
        switch (value) {
          case PixelFormat::RGBA_8888:
            return 4;

          case PixelFormat::RGB_888:
            return 3;

          case PixelFormat::R_8:
            return 1;

          default:
            return 0;
        };
      }
    };
  }
}
