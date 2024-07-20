set(TARGET_NAME ueMayaPixelStreaming)

add_library(${TARGET_NAME} SHARED)

# -----------------------------------------------------------------------------
# sources
# -----------------------------------------------------------------------------
target_sources(${TARGET_NAME} 
    PRIVATE
        renderOverride.cpp
)

# -----------------------------------------------------------------------------
# compiler configuration
# -----------------------------------------------------------------------------
target_compile_definitions(${TARGET_NAME}
    PRIVATE 
        # M3dView needs this (technically, MNativeWindowHdl.h)
        # private atm because M3dView is only used in .cpp right now
        # gcc will NOT set this automatically
        $<$<BOOL:${IS_LINUX}>:LINUX>
        # Not sure if msvcc sets this automatically, but won't hurt to redefine
        $<$<BOOL:${IS_WINDOWS}>:_WIN32>
)

# -----------------------------------------------------------------------------
# include directories
# -----------------------------------------------------------------------------
target_include_directories(${TARGET_NAME} 
    PRIVATE
        $<$<BOOL:${UFE_FOUND}>:${UFE_INCLUDE_DIR}>
)


if(DEFINED MAYAPixelStreamingUe_VERSION)
    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            MAYAPixelStreamingUe_VERSION=${MAYAPixelStreamingUe_VERSION}
    )
endif()

ueMayaPixelStreaming_compile_config(${TARGET_NAME})

# -----------------------------------------------------------------------------
# link libraries
# -----------------------------------------------------------------------------
target_link_libraries(${TARGET_NAME} 
    PRIVATE 
        # TODO my own libraries
        flowViewport
        $<$<BOOL:${UFE_FOUND}>:${UFE_LIBRARY}>
)

# -----------------------------------------------------------------------------
# properties
# -----------------------------------------------------------------------------
maya_set_plugin_properties(${TARGET_NAME})

# -----------------------------------------------------------------------------
# run-time search paths
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# install
# -----------------------------------------------------------------------------
install(TARGETS ${TARGET_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/maya)
