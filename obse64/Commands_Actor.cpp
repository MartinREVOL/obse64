#include "GameScript.h"
#include "GameObjects.h"
#include "GameRTTI.h"
#include "GameConsole.h"
#include "ParamInfos.h"
#include "ProtectedRefRegistry.h"

static bool IsActorRef(TESObjectREFR * ref)
{
	if(!ref)
		return false;

	return DYNAMIC_CAST(ref, TESObjectREFR, Actor) != nullptr;
}

static bool Cmd_IsRefProtected_Execute(COMMAND_ARGS)
{
	*result = 0;

	if(IsActorRef(thisObj))
		*result = ProtectedRefRegistry::Get().IsProtected(thisObj->refID) ? 1 : 0;

	if(IsConsoleMode())
		Console_Print("IsRefProtected >> %.0f", *result);

	return true;
}

static bool Cmd_SetRefProtected_Execute(COMMAND_ARGS)
{
	u32 setProtected = 0;
	if(!ExtractArgs(EXTRACT_ARGS, &setProtected))
		return true;

	if(IsActorRef(thisObj))
	{
		ProtectedRefRegistry::Get().SetProtected(thisObj->refID, setProtected != 0);

		if(IsConsoleMode())
			Console_Print("SetRefProtected >> %d", setProtected != 0);
	}

	return true;
}

CommandInfo kCommandInfo_IsRefProtected =
{
	"IsRefProtected", "",
	0,
	"returns 1 if the calling actor reference is protected",
	1,
	0, nullptr,

	Cmd_IsRefProtected_Execute,
};

CommandInfo kCommandInfo_SetRefProtected =
{
	"SetRefProtected", "",
	0,
	"sets or clears per-reference actor protection",
	1,
	1, kParams_OneInt,

	Cmd_SetRefProtected_Execute,
};
