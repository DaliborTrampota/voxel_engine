#include "GPUProfiler.h"

#include <glad/glad.h>
#include <cassert>
#include <cstdio>


using namespace engine;

void GPUProfiler::init() {
    for (auto& f : m_queries) {
        glGenQueries(MAX, f);
    }
}

void GPUProfiler::shutdown() {
    for (auto& f : m_queries) {
        glDeleteQueries(MAX, f);
    }
}

void GPUProfiler::beginFrame() {
    m_slot = (m_slot + 1) % RING;
    m_marker = 0;
    m_depth = 0;
    m_scopes[m_slot].clear();
}

int GPUProfiler::begin(std::string name) {
    assert(m_marker < MAX && "GPU profiler scope budget exceeded (each scope uses 2 markers)");
    if (m_marker >= MAX) {
        return -1;
    }
    int b = m_marker++;
    glQueryCounter(m_queries[m_slot][b], GL_TIMESTAMP);
    m_scopes[m_slot].push_back({name, m_depth++, b, -1});
    return m_scopes[m_slot].size() - 1;
}

void GPUProfiler::end(int handle) {
    if (handle < 0 || handle >= m_scopes[m_slot].size()) {
        return;
    }
    m_depth--;
    assert(m_marker < MAX && "GPU profiler scope budget exceeded (each scope uses 2 markers)");
    if (m_marker >= MAX) {
        return;
    }
    int e = m_marker++;
    glQueryCounter(m_queries[m_slot][e], GL_TIMESTAMP);
    m_scopes[m_slot][handle].end = e;
}

void GPUProfiler::endFrame() {
    int read = (m_slot + 1) % RING;
    for (auto& s : m_scopes[read]) {
        if (s.end < 0)
            continue;
        GLuint64 t0, t1;
        glGetQueryObjectui64v(m_queries[read][s.begin], GL_QUERY_RESULT, &t0);
        glGetQueryObjectui64v(m_queries[read][s.end], GL_QUERY_RESULT, &t1);
        double time = (t1 - t0) / 1'000'000.0;
        //todo move to print
        printf("%s: %fms\n", s.name.c_str(), time);
    }
}