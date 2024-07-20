//
// Copyright 2019 Perrauo
//
// Licensed under the Proprietary License, Version 1.0 (the "License");
// you may not use this file except in compliance with the License.
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright 2024 Perrauo, Inc. All rights reserved.
//

#include "renderOverride.h"
#include "viewCommand.h"

#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MSceneMessage.h>

#include <memory>
#include <vector>

#include <stdio.h>
#include <stdlib.h>

#if MAYA_API_VERSION < 20240000
#error Maya API version 2024+ required
#endif

#if defined(MAYA_PIXELSTREAMING_VERSION)
#define STRINGIFY(x)   #x
#define TOSTRING(x)    STRINGIFY(x)
#define PLUGIN_VERSION TOSTRING(MAYA_PIXELSTREAMING_VERSION)
#else
#pragma message("MAYA_PIXELSTREAMING_VERSION is not defined")
#define PLUGIN_VERSION "Maya-PixelStreaming experimental"
#endif

namespace {
    const std::string kMayaPixelStreamingPluginName = "mayaPixelStreaming";
    std::vector<MayaPixelStreamingRenderOverride*> _renderOverrides;
    std::vector<MCallbackId> _pluginLoadingCallbackIds;
}

void initialize()
{
}

void finalize()
{
}

void afterPluginLoadCallback(const MStringArray& strs, void* clientData)
{
}

void beforePluginUnloadCallback(const MStringArray& strs, void* clientData)
{
}

PLUGIN_EXPORT MStatus initializePlugin(MObject obj)
{
    MString experimental("mayaPixelStreaming is experimental.");
    MGlobal::displayWarning(experimental);

    MStatus ret = MS::kSuccess;

    MFnPlugin plugin(obj, "Perrauo", PLUGIN_VERSION, "Any");

    if (!plugin.registerCommand(
        MayaPixelStreamingViewCmd::name, MayaPixelStreamingViewCmd::creator, MayaPixelStreamingViewCmd::createSyntax)) {
        ret = MS::kFailure;
        ret.perror("Error registering mayaPixelStreaming command!");
        return ret;
    }

    if (auto* renderer = MHWRender::MRenderer::theRenderer()) {
        MayaPixelStreamingRendererDesc desc("Unreal Engine", "Pixel Streaming", "Pixel Streaming");
        auto renderOverride = std::make_unique<MayaPixelStreamingRenderOverride>(desc);
        MStatus status = renderer->registerOverride(renderOverride.get());
        if (status == MS::kSuccess) {
            _renderOverrides.push_back(renderOverride.release());
        }
    }

    if (!plugin.registerUIStrings(nullptr, "mayaPixelStreaming_registerUIStrings")) {
        ret = MS::kFailure;
        ret.perror("Error registering mayaPixelStreaming UI string resources.");
        return ret;
    }

    if (!plugin.registerUI(
        "mayaPixelStreaming_registerUI_load",
        "mayaPixelStreaming_registerUI_unload",
        "mayaPixelStreaming_registerUI_batch_load",
        "mayaPixelStreaming_registerUI_batch_unload"))
    {
        ret = MS::kFailure;
        ret.perror("Error registering mayaPixelStreaming UI procedures.");
        return ret;
    }

    auto registerPluginLoadingCallback = [&](MSceneMessage::Message pluginLoadingMessage, MMessage::MStringArrayFunction callback) {
        MStatus callbackStatus;
        MCallbackId callbackId = MSceneMessage::addStringArrayCallback(
            pluginLoadingMessage,
            callback,
            nullptr,
            &callbackStatus);
        if (callbackStatus) {
            _pluginLoadingCallbackIds.push_back(callbackId);
        }
        else {
            ret = MS::kFailure;
            ret.perror("Error registering mayaPixelStreaming plugin loading callback.");
        }
        };

    std::vector<std::pair<MSceneMessage::Message, MMessage::MStringArrayFunction>> pluginLoadingCallbacks = {
        {MSceneMessage::Message::kAfterPluginLoad, afterPluginLoadCallback},
        {MSceneMessage::Message::kBeforePluginUnload, beforePluginUnloadCallback}
    };
    for (const auto& pluginLoadingCallback : pluginLoadingCallbacks) {
        registerPluginLoadingCallback(pluginLoadingCallback.first, pluginLoadingCallback.second);
        if (!ret) {
            return ret;
        }
    }

    initialize();

    return ret;
}

PLUGIN_EXPORT MStatus uninitializePlugin(MObject obj)
{
    finalize();

    for (const auto& callbackId : _pluginLoadingCallbackIds) {
        MSceneMessage::removeCallback(callbackId);
    }

    MFnPlugin plugin(obj, "Autodesk", PLUGIN_VERSION, "Any");
    MStatus   ret = MS::kSuccess;
    if (auto* renderer = MHWRender::MRenderer::theRenderer()) {
        for (unsigned int i = 0; i < _renderOverrides.size(); i++) {
            renderer->deregisterOverride(_renderOverrides[i]);
            delete _renderOverrides[i];
        }
    }

    _renderOverrides.clear();

    // Clear any registered callbacks
    //MGlobal::executeCommand("callbacks -cc -owner mayaHydra;");

    if (!plugin.deregisterCommand("pixelstreaming")) {
        ret = MS::kFailure;
        ret.perror("Error deregistering mayaPixelStreaming command!");
    }

    return ret;
}
