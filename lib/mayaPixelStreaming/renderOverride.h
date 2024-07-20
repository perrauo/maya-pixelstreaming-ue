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


struct MayaPixelStreamingRendererDesc
{
    MString rendererName;
    MString overrideName;
    MString displayName;
    
    MayaPixelStreamingRendererDesc(
    const MString& rendererName
    , const MString& overrideName
    , const MString& displayName)
    : rendererName(rendererName)
    , overrideName(overrideName)
    , displayName(displayName)
    {    
    }
};

/*! \brief MayaPixelStreamingRenderOverride is a rendering override class for the viewport to use Unreal Pixel Streaming
 */
class MayaPixelStreamingRenderOverride : public MHWRender::MRenderOverride
{
public:
    // Picking support.
    class PickHandlerBase;
    friend PickHandlerBase;

    MayaPixelStreamingRenderOverride(const MayaPixelStreamingRendererDesc& desc);
    ~MayaPixelStreamingRenderOverride() override;

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
    int _currentOperation = 0;
    MayaPixelStreamingRendererDesc _desc;
    std::vector<MHWRender::MRenderOperation*> _operations;

};

class MayaPixelStreamingRenderOverride;

class MayaPixelStreamingPreRender : public MHWRender::MSceneRender
{
public:
    explicit MayaPixelStreamingPreRender(const MString& name)
        : MHWRender::MSceneRender(name)
    {
        /// To keep the colors always sync'ed, reuse same clear colors as global ones instead of set
        /// the same colors explicitly.
        mClearOperation.setOverridesColors(false);
    }

    MUint64 getObjectTypeExclusions() override
    {
        /// To skip the generation of some unwanted render lists even the kRenderPreSceneUIItems
        /// filter is specified.
        return MFrameContext::kExcludeManipulators | MFrameContext::kExcludeHUD;
    }

    MSceneFilterOption renderFilterOverride() override { return kRenderPreSceneUIItems; }

    MHWRender::MClearOperation& clearOperation() override { return mClearOperation; }
};

class MayaPixelStreamingPostRender : public MHWRender::MSceneRender
{
public:
    explicit MayaPixelStreamingPostRender(const MString& name)
        : MHWRender::MSceneRender(name)
    {
        mClearOperation.setMask(MHWRender::MClearOperation::kClearNone);
    }

    MUint64 getObjectTypeExclusions() override
    {
        static MUint64 sObjectTypeExclusions = []() -> MUint64 {
            /// Exclude everything as a baseline. Restore grease pencils, hud, and grid.
            MUint64 flags = MFrameContext::kExcludeAll;
            flags &= ~MFrameContext::kExcludeGreasePencils;
            flags &= ~MFrameContext::kExcludeHUD;
            flags &= ~MFrameContext::kExcludeDimensions;
            flags &= ~MFrameContext::kExcludeManipulators;
            return flags;
            }();
        return sObjectTypeExclusions;
    }

    MSceneFilterOption renderFilterOverride() override
    {
        return MSceneFilterOption(kRenderPostSceneUIItems);
    }

    MHWRender::MClearOperation& clearOperation() override { return mClearOperation; }
};

/**
 * \brief Serves to synchronize maya viewport data with the scene delegate before scene update is
 *  called
 *   when requiresSceneUpdate=false, subtype=kDataServerRemovals and after scene update is called
 *   when requiresSceneUpdate=true, subtype=kDataServer
 */

class MayaPixelStreamingRender : public MHWRender::MDataServerOperation
{
public:
    MayaPixelStreamingRender(const MString& name, MayaPixelStreamingRenderOverride* override)
        : MDataServerOperation(name)
        , _override(override)
    {
    }

    MStatus execute(
        const MDrawContext& drawContext,
        const MHWRender::MDataServerOperation::MViewportScene& scene) override
    {
        return _override->Render(drawContext, scene);
    }

    bool requiresResetDeviceStates() const override
    {
        /// Reset maya graphics device states (MAYA-126735)
        return true;
    }

private:
    MayaPixelStreamingRenderOverride* _override;
};

#endif // MAYA_PIXELSTREAMING_RENDER_OVERRIDE_H