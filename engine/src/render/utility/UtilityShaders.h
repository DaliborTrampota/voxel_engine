#pragma once

#include <LWGL/GLTypes.h>

namespace gl {
    class Texture2D;
    class FBO;
}  // namespace gl

namespace engine {

    // extern std::unique_ptr<Material> s_gaussianBlurMat;

    void initUtilityShaders();

    void applyGaussianBlur(
        gl::FBO& source,  //TODO const, ptr as temp?
        gl::FBOAttachment::Attachment sourceAttachment,
        gl::FBOAttachment::Attachment tempAttachment,
        int width,
        int height,
        gl::FBO* temp = nullptr  //TODO const
    );
}  // namespace engine