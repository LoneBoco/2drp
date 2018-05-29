#pragma once

namespace tdrp {
  namespace render {
    enum PixelFormat {
      RGBA_8888, // 4-component, 8-bits per-pixel (32-bit)
      RGB_888,   // 3-component, 8-bits per-pixel (24-bit)
      R_8        // 1-component, 8-bits per-pixel (8-bit)
    };
  }
}
