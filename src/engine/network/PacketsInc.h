#pragma once

// Get rid of a horribly annoying warning in protobuf.
#pragma warning (push)
#pragma warning (disable : 4146)
#pragma warning (disable : 4267)

#include "engine/network/packets/ClassAdd.pb.h"
#include "engine/network/packets/ClassDelete.pb.h"
#include "engine/network/packets/ClientScriptAdd.pb.h"
#include "engine/network/packets/ClientScriptDelete.pb.h"
#include "engine/network/packets/Error.pb.h"
#include "engine/network/packets/FileRequest.pb.h"
#include "engine/network/packets/FileTransfer.pb.h"
#include "engine/network/packets/FlagSet.pb.h"
#include "engine/network/packets/ItemAdd.pb.h"
#include "engine/network/packets/ItemDefinition.pb.h"
#include "engine/network/packets/ItemCount.pb.h"
#include "engine/network/packets/Login.pb.h"
#include "engine/network/packets/LoginStatus.pb.h"
#include "engine/network/packets/SceneObjectChange.pb.h"
#include "engine/network/packets/SceneObjectChunkData.pb.h"
#include "engine/network/packets/SceneObjectControl.pb.h"
#include "engine/network/packets/SceneObjectDelete.pb.h"
#include "engine/network/packets/SceneObjectNew.pb.h"
#include "engine/network/packets/SceneObjectOwnership.pb.h"
#include "engine/network/packets/SceneObjectRequestChunkData.pb.h"
#include "engine/network/packets/SceneObjectShapes.pb.h"
#include "engine/network/packets/SceneObjectUnfollow.pb.h"
#include "engine/network/packets/SceneTilesetAdd.pb.h"
#include "engine/network/packets/SendEvent.pb.h"
#include "engine/network/packets/ServerInfo.pb.h"
#include "engine/network/packets/SwitchScene.pb.h"

#pragma warning (pop)
