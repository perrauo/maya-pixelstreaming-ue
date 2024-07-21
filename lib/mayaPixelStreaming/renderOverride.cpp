//
// Copyright 2024 Perrauo
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

#include <ufe/hierarchy.h>
#include <ufe/selection.h>
#include <ufe/namedSelection.h>
#include <ufe/path.h>
#include <ufe/pathString.h>
#include <ufe/observableSelection.h>
#include <ufe/globalSelection.h>
#include <ufe/selectionNotification.h>
#include <ufe/observer.h>

#include <maya/M3dView.h>
#include <maya/MConditionMessage.h>
#include <maya/MDGMessage.h>
#include <maya/MDrawContext.h>
#include <maya/MEventMessage.h>
#include <maya/MGlobal.h>
#include <maya/MNodeMessage.h>
#include <maya/MObjectHandle.h>
#include <maya/MProfiler.h>
#include <maya/MSceneMessage.h>
#include <maya/MSelectionList.h>
#include <maya/MTimerMessage.h>
#include <maya/MUiMessage.h>
#include <maya/MFnCamera.h>
#include <maya/MFileIO.h>
#include <maya/MFrameContext.h>
#include <maya/MQtUtil.h>

#include <atomic>
#include <chrono>
#include <exception>
#include <limits>


MayaPixelStreamingRenderOverride::MayaPixelStreamingRenderOverride(const MayaPixelStreamingRendererDesc& desc)
    : MHWRender::MRenderOverride(desc.overrideName)
    , _desc(desc)
{
}

MayaPixelStreamingRenderOverride::~MayaPixelStreamingRenderOverride()
{
}


std::vector<MString> MayaPixelStreamingRenderOverride::AllActiveRendererNames()
{
    std::vector<MString> renderers;
    return renderers;
}

MStatus MayaPixelStreamingRenderOverride::Render(
    const MHWRender::MDrawContext&  drawContext,
    const MHWRender::MDataServerOperation::MViewportScene& scene)
{
   
    MString panelName;
    auto framecontext = getFrameContext();
    if (framecontext){
        framecontext->renderingDestination(panelName);
        
    }

    int width = 0;
    int height = 0;
    drawContext.getRenderTargetSize(width, height);


    return MStatus::kSuccess;
}


MHWRender::DrawAPI MayaPixelStreamingRenderOverride::supportedDrawAPIs() const
{
    return MHWRender::kOpenGLCoreProfile | MHWRender::kOpenGL;
}

MStatus MayaPixelStreamingRenderOverride::setup(const MString& destination)
{
    auto* renderer = MHWRender::MRenderer::theRenderer();
    if (renderer == nullptr) {
        return MStatus::kFailure;
    }

    if (_operations.empty()) {
        
        _operations.push_back(new MayaPixelStreamingPreRender("MayaPixelStreamingRenderOverride_PreScene"));
        _operations.push_back(new MayaPixelStreamingRender("MayaPixelStreamingRenderOverride_DataServer", this));
        _operations.push_back(new MayaPixelStreamingPostRender("MayaPixelStreamingRenderOverride_PostScene"));
        _operations.push_back(new MHWRender::MHUDRender());
        auto* presentTarget = new MHWRender::MPresentTarget("MayaPixelStreamingRenderOverride_Present");
        presentTarget->setPresentDepth(true);
        presentTarget->setTargetBackBuffer(MHWRender::MPresentTarget::kCenterBuffer);
        _operations.push_back(presentTarget);
    }

    return MS::kSuccess;
}

MStatus MayaPixelStreamingRenderOverride::cleanup()
{
    _currentOperation = -1;
    return MS::kSuccess;
}

bool MayaPixelStreamingRenderOverride::startOperationIterator()
{
    _currentOperation = 0;
    return true;
}

MHWRender::MRenderOperation* MayaPixelStreamingRenderOverride::renderOperation()
{
    if (_currentOperation >= 0 && _currentOperation < static_cast<int>(_operations.size())) {
        return _operations[_currentOperation];
    }
    return nullptr;
}

bool MayaPixelStreamingRenderOverride::nextRenderOperation()
{
    return ++_currentOperation < static_cast<int>(_operations.size());
}



bool MayaPixelStreamingRenderOverride::select(
const MHWRender::MFrameContext& frameContext,
const MHWRender::MSelectionInfo& selectInfo,
bool useDepth,
MSelectionList& selectionList,
MPointArray& worldSpaceHitPts)
{
    return false;
}


