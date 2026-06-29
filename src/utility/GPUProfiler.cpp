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
    // each scope needs two markers (begin + end); reserve both up front so a scope
    // is all-or-nothing and end() can never overflow the query budget
    assert(m_marker + 2 <= MAX && "GPU profiler scope budget exceeded (2 markers/scope)");
    if (m_marker + 2 > MAX) {
        return -1;
    }
    int b = m_marker;
    m_marker += 2;  // b = begin slot, b + 1 = reserved end slot
    glQueryCounter(m_queries[m_slot][b], GL_TIMESTAMP);
    m_scopes[m_slot].push_back({std::move(name), m_depth++, b, -1});
    return m_scopes[m_slot].size() - 1;
}

void GPUProfiler::end(int handle) {
    if (handle < 0 || handle >= m_scopes[m_slot].size()) {
        return;
    }
    m_depth--;
    Scope& s = m_scopes[m_slot][handle];
    int e = s.begin + 1;  // pre-reserved in begin() — always in bounds
    glQueryCounter(m_queries[m_slot][e], GL_TIMESTAMP);
    s.end = e;
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