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

#include "viewCommand.h"

#include "renderOverride.h"

#include <maya/MArgDatabase.h>
#include <maya/MGlobal.h>
#include <maya/MSyntax.h>

const MString MayaPixelStreamingViewCmd::name("mayaPixelStreaming");

namespace {

constexpr auto _helpText = R"HELP(
Maya to Hydra utility function.
Usage: mayaPixelStreaming [flags]
)HELP";

constexpr auto _helpNonVerboseText = R"HELP(
Use -verbose/-v to see advanced / debugging flags

)HELP";

constexpr auto _helpVerboseText = R"HELP(
Debug flags:

)HELP";

} // namespace

MSyntax MayaPixelStreamingViewCmd::createSyntax()
{
    MSyntax syntax;
    return syntax;
}

