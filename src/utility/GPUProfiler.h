#pragma once

#include <string>
#include <vector>


namespace engine {
    class GPUProfiler {
      public:
        void init();
        void shutdown();

        void beginFrame();
        int begin(std::string name);
        void end(int handle);
        void endFrame();
        void print();

      private:
        static constexpr int RING = 3;
        static constexpr int MAX = 128;

        unsigned int m_queries[RING][MAX];

        struct Scope {
            std::string name;
            int depth, begin, end;
        };
        std::vector<Scope> m_scopes[RING];
        int m_slot = 0;
        int m_depth = 0;
        int m_marker = 0;
    };

    struct GPUScope {
        GPUProfiler* profiler;
        int h;
        GPUScope(GPUProfiler* profiler, std::string name)
            : profiler(profiler),
              h(profiler->begin(name)) {}
        ~GPUScope() { profiler->end(h); }
    };

#define GPU_SCOPE(profiler, name) GPUScope _gpuscope_##__LINE__((profiler), name)
}  // namespace engine