#pragma once

// Get rid of a horribly annoying warning in protobuf.
#pragma warning (push)
#pragma warning (disable : 4146)

#include "engine/network/packets/CLogin.pb.h"
#include "engine/network/packets/CSceneObjectChange.pb.h"
#include "engine/network/packets/CSendEvent.pb.h"

#pragma warning (pop)
