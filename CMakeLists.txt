cmake_minimum_required(VERSION 2.8.9)
set(PROJECT_NAME "MutexSample")
project(${PROJECT_NAME})

# set build mode
set(USE_STD11 ON)


if (USE_STD11)
    add_definitions(-DUSE_STD11)
    set(CMAKE_CXX_STANDARD 11)
    set(INCLUDE_DIRECTORIES
    )
    set(LINK_DIRECTORIES
    )
    if (MSVC)
        set(TARGET_LINK_LIBRARIES_RELEASE
        )
        set(TARGET_LINK_LIBRARIES_DEBUG
        )
    elseif (UNIX)
        set(TARGET_LINK_LIBRARIES_RELEASE
            -lstdc++ -std=c++11
            -lpthread
        )
        set(TARGET_LINK_LIBRARIES_DEBUG
            -lstdc++ -std=c++11
            -lpthread
        )
    else ()
        set(TARGET_LINK_LIBRARIES_RELEASE
        )
        set(TARGET_LINK_LIBRARIES_DEBUG
        )
    endif ()
elseif (MSVC)
    add_definitions(-DUSE_WIN32)
    add_definitions(-DUNICODE)
    set(INCLUDE_DIRECTORIES
    )
    set(LINK_DIRECTORIES
    )
    set(TARGET_LINK_LIBRARIES_RELEASE
    )
    set(TARGET_LINK_LIBRARIES_DEBUG
    )
elseif (UNIX)
    add_definitions(-DUSE_POSIX)
    set(INCLUDE_DIRECTORIES
    )
    set(LINK_DIRECTORIES
    )
    set(TARGET_LINK_LIBRARIES_RELEASE
        -lpthread
    )
    set(TARGET_LINK_LIBRARIES_DEBUG
        -lpthread
    )
else ()
endif ()

include_directories(
    ${INCLUDE_DIRECTORIES}
)

link_directories(
    ${LINK_DIRECTORIES}
)

add_executable(${PROJECT_NAME}
    Source.cpp
)

target_link_libraries(${PROJECT_NAME}
    ${TARGET_LINK_LIBRARIES_RELEASE}
    ${TARGET_LINK_LIBRARIES_DEBUG}
)

