#pragma once

#undef ERROR

namespace tdrp
{

enum class ClientPackets
{
	NOTHING = 0,
	LOGIN,
	SCENEOBJECTCHANGE,

	WANTFILE,				// [INT4 mod time][filename]
	SERVERATTRIBUTES,		// {[INT2 id][value], ...}

	COUNT
};

enum class ServerPackets
{
	NOTHING = 0,
	ERROR,					// [STRING error]
	LOGINSTATUS,
	CLIENTSCRIPT,
	CLIENTSCRIPTDELETE,
	CLASS,
	CLASSDELETE,
	SCENEOBJECTNEW,
	SCENEOBJECTCHANGE,
	SCENEOBJECTDELETE,

	FILESTART,				// [INT2 id][INT1 type][file name]
	FILEDATA,				// [INT2 id][data]
	FILEEND,				// [INT2 id]
	SERVERTYPE,				// [type]
	CLASSADD,				// [name]
	CLASSATTRIBUTEADD,		// [INT1 class length][class][INT2 id][name]
	CLASSSCRIPT,			// [STRING class name][script]
	SERVERATTRIBUTEADD,		// [INT2 id][name]
	SERVERATTRIBUTES,		// {[INT2 id][value], ...}
	OBJECTADD,				// [INT4 id][INT1 type][class]
	OBJECTDELETE,			// [INT4 id]
	OBJECTATTRIBUTES,		// [INT4 id] {[INT2 id][value], ...}
	OBJECTPROPERTIES,		// [INT4 id] {[INT1 id][value], ...}
	OBJECTSCRIPT,			// [INT4 id][script]
	PLAYEROBJECT,			// [INT4 id]
	PHYSICS_ADD_POLYGON,	// [INT4 id] ([INT1 0][FLOAT width][FLOAT height] / [INT1 1]{[FLOAT x1][FLOAT y1], ...})

	COUNT
};

} // end namespace tdrp
