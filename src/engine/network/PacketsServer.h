#pragma once

// Get rid of a horribly annoying warning in protobuf.
#pragma warning (push)
#pragma warning (disable : 4146)

#include "engine/network/packets/SClass.pb.h"
#include "engine/network/packets/SClassDelete.pb.h"
#include "engine/network/packets/SClientScript.pb.h"
#include "engine/network/packets/SClientScriptDelete.pb.h"
#include "engine/network/packets/SError.pb.h"
#include "engine/network/packets/SLoginStatus.pb.h"
#include "engine/network/packets/SPackageFiles.pb.h"
#include "engine/network/packets/SSceneObjectChange.pb.h"
#include "engine/network/packets/SSceneObjectDelete.pb.h"
#include "engine/network/packets/SSceneObjectNew.pb.h"
#include "engine/network/packets/SSendEvent.pb.h"
#include "engine/network/packets/SServerInfo.pb.h"
#include "engine/network/packets/STransferFile.pb.h"

#pragma warning (pop)
