#pragma once

#include <LWGL/GLTypes.h>
#include <memory>

namespace gl {
    class Texture2D;
    class FBO;
}  // namespace gl

namespace engine {

    class Material;

    // extern std::unique_ptr<Material> s_gaussianBlurMat;

    enum class BlurType {
        Gaussian,
        LinearSampling
    };

    void initUtilityShaders();

    void applyGaussianBlur(
        const gl::FBO& source,  //TODO ptr as temp?
        gl::FBOAttachment::Attachment sourceAttachment,
        gl::FBOAttachment::Attachment tempAttachment,
        int width,
        int height,
        const gl::FBO* temp = nullptr
    );

    void applyLinearSamplingBlur(
        const gl::FBO& source,
        gl::FBOAttachment::Attachment sourceAttachment,
        gl::FBOAttachment::Attachment tempAttachment,
        int width,
        int height,
        const gl::FBO* temp = nullptr
    );

    void applyBlur(
        std::unique_ptr<Material>& blurMat,
        const gl::FBO& source,
        gl::FBOAttachment::Attachment sourceAttachment,
        gl::FBOAttachment::Attachment tempAttachment,
        int width,
        int height,
        const gl::FBO* temp = nullptr
    );
}  // namespace engine