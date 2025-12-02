cmake_minimum_required(VERSION 3.12)

project(soloud LANGUAGES C CXX)
set(SOLOUD_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/soloud)

file(GLOB_RECURSE SOLOUD_SOURCES
    ${SOLOUD_ROOT}/src/audiosource/*.cpp
    ${SOLOUD_ROOT}/src/backend/miniaudio/*.cpp
    ${SOLOUD_ROOT}/src/core/*.cpp
    ${SOLOUD_ROOT}/src/filter/*.cpp
    ${SOLOUD_ROOT}/src/soloud.cpp
)


add_library(soloud STATIC ${SOLOUD_SOURCES})

target_include_directories(soloud PUBLIC ${SOLOUD_ROOT}/include)

target_compile_definitions(soloud PUBLIC
    WITH_MINIAUDIO
    MINIAUDIO_IMPLEMENTATION
)

target_compile_features(soloud PUBLIC cxx_std_17)