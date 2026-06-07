#include "GameScript.h"
#include "GameForms.h"
#include "GameObjects.h"
#include "GameRTTI.h"
#include "GameConsole.h"
#include "ParamInfos.h"

static TESActorBaseData * GetActorBaseData(TESObjectREFR * ref)
{
	if(!ref)
		return nullptr;

	TESForm * baseForm = ref->GetBaseForm();
	if(!baseForm)
		return nullptr;

	return DYNAMIC_CAST(baseForm, TESForm, TESActorBaseData);
}

static bool Cmd_IsRefEssential_Execute(COMMAND_ARGS)
{
	*result = 0;

	TESActorBaseData * actorBaseData = GetActorBaseData(thisObj);
	if(actorBaseData)
		*result = actorBaseData->IsEssential() ? 1 : 0;

	if(IsConsoleMode())
		Console_Print("IsRefEssential >> %.0f", *result);

	return true;
}

static bool Cmd_SetRefEssential_Execute(COMMAND_ARGS)
{
	u32 setEssential = 0;
	if(!ExtractArgs(EXTRACT_ARGS, &setEssential))
		return true;

	TESActorBaseData * actorBaseData = GetActorBaseData(thisObj);
	if(actorBaseData)
	{
		actorBaseData->SetEssential(setEssential != 0);

		if(IsConsoleMode())
			Console_Print("SetRefEssential >> %d", setEssential != 0);
	}

	return true;
}

CommandInfo kCommandInfo_IsRefEssential =
{
	"IsRefEssential", "",
	0,
	"returns 1 if the calling actor reference's base is essential",
	1,
	0, nullptr,

	Cmd_IsRefEssential_Execute,
};

CommandInfo kCommandInfo_SetRefEssential =
{
	"SetRefEssential", "",
	0,
	"sets or clears the essential flag on the calling actor reference's base",
	1,
	1, kParams_OneInt,

	Cmd_SetRefEssential_Execute,
};
