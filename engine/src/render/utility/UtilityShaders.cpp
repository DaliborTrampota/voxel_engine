#include "UtilityShaders.h"

#include <LWGL/GLTypes.h>
#include <LWGL/buffer/FBO.h>
#include <LWGL/texture/Texture2D.h>
#include <LWGL/texture/TextureBase.h>
#include <render/Material.h>


namespace engine {

    std::unique_ptr<Material> s_gaussianBlurMat = nullptr;
    static GLuint s_dummyVAO = 0;

    void initUtilityShaders() {
        s_gaussianBlurMat = std::make_unique<Material>(
            "resources/shaders/utility/GaussianBlur.vert",
            "resources/shaders/utility/GaussianBlur.frag",
            "GaussianBlur"
        );

        // Create dummy VAO for fullscreen triangle (vertex ID trick)
        if (s_dummyVAO == 0) {
            glGenVertexArrays(1, &s_dummyVAO);
        }
    }


    void applyGaussianBlur(
        gl::FBO& source,
        gl::FBOAttachment::Attachment sourceAttachment,
        gl::FBOAttachment::Attachment tempAttachment,
        int width,
        int height,
        gl::FBO* temp
    ) {
        int prevViewport[4];
        glGetIntegerv(GL_VIEWPORT, prevViewport);
        glViewport(0, 0, width, height);
        glDisable(GL_DEPTH_TEST);

        s_gaussianBlurMat->use();
        glBindVertexArray(s_dummyVAO);  // Bind dummy VAO for vertex ID trick

        source.bind();
        source.setDrawBuffers({tempAttachment});  // draw to temp

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, source.texture(sourceAttachment));  // read from source

        s_gaussianBlurMat->setInt("inputTex", 0);
        s_gaussianBlurMat->setFloat("texelSize", 1.0f / width);
        s_gaussianBlurMat->setBool("horizontal", true);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        const gl::FBO* tempFBO = temp ? temp : &source;
        tempFBO->bind();
        tempFBO->setDrawBuffers({sourceAttachment});  // draw back to source

        glBindTexture(GL_TEXTURE_2D, tempFBO->texture(tempAttachment));  // read from temp

        s_gaussianBlurMat->setFloat("texelSize", 1.0f / height);
        s_gaussianBlurMat->setBool("horizontal", false);
        glDrawArrays(GL_TRIANGLES, 0, 3);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);  // Unbind VAO
        source.unbind();
        glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
        glEnable(GL_DEPTH_TEST);
    }
}  // namespace engine