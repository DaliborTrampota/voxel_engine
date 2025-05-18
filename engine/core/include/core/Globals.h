#pragma once

#include <thread>

extern std::thread::id glContextID;

#define _NO_THREAD_LOG

#ifdef NO_THREAD_LOG
#define GL_GUARD
#else
#define GL_GUARD \
    if (std::this_thread::get_id() != glContextID) \
        printf( \
            "GL_WARNING: [%s l%d]: Called GL function from different thread!\n", \
            __FILE__, \
            __LINE__ \
        );
#endif


#define INIT_GL_THREAD glContextID = std::this_thread::get_id();