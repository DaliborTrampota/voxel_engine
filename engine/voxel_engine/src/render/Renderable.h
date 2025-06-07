#pragma once


namespace engine {

    struct RenderContext;
    class Engine;
    class Camera;

    class Renderable {
      public:
        virtual ~Renderable() = default;

        /// @brief Engine will call this method before rendering the object.
        ///        You have to set at the very least the gl::Attributes instance and material.
        ///        Model matrix is optional, but recommended.
        /// @param ctx The render context to be filled with the necessary data for rendering.
        /// @param pass Number indicating the pass, if you need multiple passes for rendering.
        /// @note The render pass has to be set by the caller of this method.
        //virtual void render(RenderContext& ctx, int pass = 0) = 0;


        /// @brief Engine will call this method before rendering the object.
        ///        You have to set at the very least the gl::Attributes instance and material.
        ///        Model matrix is optional, but recommended.
        /// @param Engine The render engine to which you can push RenderContext to schedule rendering.
        /// @param pass Number indicating the pass, if you need multiple passes for rendering.
        /// @note The render pass has to be set by the caller of this method.
        /// @note The render context has to be filled with the necessary data for rendering.
        virtual void render(Engine& engine, const Camera* camera, int pass = 0) = 0;
    };
}  // namespace engine