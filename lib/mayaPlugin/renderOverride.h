//
// Copyright 2019 Luma Pictures
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright 2023 Autodesk
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#ifndef MAYAUE_PIXELSTREAMING_RENDER_OVERRIDE_H
#define MAYAUE_PIXELSTREAMING_RENDER_OVERRIDE_H

//#include "renderGlobals.h"
//#include "pluginUtils.h"

#include <maya/MCallbackIdArray.h>
#include <maya/MMessage.h>
#include <maya/MObjectHandle.h>
#include <maya/MString.h>
#include <maya/MViewport2Renderer.h>

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

#include <ufe/ufe.h>

UFE_NS_DEF {
class SelectionChanged;
class Selection;
}

/*! \brief MayaPixelStreamingUeRenderOverride is a rendering override class for the viewport to use Unreal Pixel Streaming
 */
class MayaUePixelStreamingRenderOverride : public MHWRender::MRenderOverride
{
public:
    // Picking support.
    class PickHandlerBase;
    friend PickHandlerBase;

    MayaUePixelStreamingRenderOverride(const MayaUePixelStreamingRenderOverride& desc);
    ~MayaUePixelStreamingRenderOverride() override;

    /// The names of all render delegates that are being used by at least
    /// one modelEditor panel.
    static std::vector<MString> AllActiveRendererNames();

    MStatus Render(
        const MHWRender::MDrawContext&                         drawContext,
        const MHWRender::MDataServerOperation::MViewportScene& scene);
    
    MString uiName() const override { return MString("MayaPixelStreamingUe"); }

    MHWRender::DrawAPI supportedDrawAPIs() const override;

    MStatus setup(const MString& destination) override;
    MStatus cleanup() override;

    bool                         startOperationIterator() override;
    MHWRender::MRenderOperation* renderOperation() override;
    bool                         nextRenderOperation() override;

    bool select(
        const MHWRender::MFrameContext&  frameContext,
        const MHWRender::MSelectionInfo& selectInfo,
        bool                             useDepth,
        MSelectionList&                  selectionList,
        MPointArray&                     worldSpaceHitPts) override;

private:

};

#endif // MAYAUE_PIXELSTREAMING_RENDER_OVERRIDE_H