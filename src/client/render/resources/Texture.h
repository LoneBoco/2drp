#pragma once

#include "client/render/resources/Image.h"
#include "client/render/PixelFormat.h"

namespace tdrp {
  namespace render {
    class Texture {
    public:
      Texture(const ResourceHandle<Image>& image);
      ~Texture();

      Texture(const Texture&) = delete;
      Texture(Texture&&) = delete;
      Texture& operator=(const Texture&) = delete;
      Texture& operator=(Texture&) = delete;

      void SetImage(const ResourceHandle<Image>& image);
      const ResourceHandle<Image>& GetImage() const { return m_image; };

      // TODO: Texture flags
      void SetTextureFlags() {};

    private:
      ResourceHandle<Image> m_image;
    };
  }
}
