set(TARGET_NAME mayaPixelStreaming)

add_library(${TARGET_NAME} SHARED)

# -----------------------------------------------------------------------------
# sources
# -----------------------------------------------------------------------------
target_sources(${TARGET_NAME} 
    PRIVATE
        renderOverride.cpp
        plugin.cpp
        viewCommand.cpp
)

set(HEADERS    
    renderOverride.h
    viewCommand.h
)

# -----------------------------------------------------------------------------
# promoted headers
# -----------------------------------------------------------------------------
mayaPixelStreaming_promoteHeaderList(
    HEADERS 
        ${HEADERS}    
    BASEDIR
        ${TARGET_NAME}
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
    PUBLIC
        ${MAYA_INCLUDE_DIRS}
        ${PYTHON_INCLUDE_DIR}
        ${PXR_INCLUDE_DIRS}
        ${BOOST_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}/include
    PRIVATE
        $<$<BOOL:${UFE_FOUND}>:${UFE_INCLUDE_DIR}>
)

if(DEFINED MAYA_PIXELSTREAMING_VERSION)
    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            MAYA_PIXELSTREAMING_VERSION=${MAYA_PIXELSTREAMING_VERSION}
    )
endif()

mayaPixelStreaming_compile_config(${TARGET_NAME})

# -----------------------------------------------------------------------------
# link libraries
# -----------------------------------------------------------------------------
target_link_libraries(${TARGET_NAME} 
    PUBLIC
        ${MAYA_LIBRARIES}

    PRIVATE 
        WebEngineCore WebEngine
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
