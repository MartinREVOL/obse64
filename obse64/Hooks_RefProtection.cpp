#include "Hooks_RefProtection.h"
#include "ProtectedRefRegistry.h"
#include "RE_Offsets.h"
#include "obse64_common/BranchTrampoline.h"
#include "obse64_common/Log.h"
#include "obse64_common/Relocation.h"
#include "obse64_common/SafeWrite.h"
#include "xbyak/xbyak.h"

namespace
{
	bool IsProtectedActor(void * actor)
	{
		if(!actor)
			return false;

		auto formID = *reinterpret_cast<u32 *>(reinterpret_cast<uintptr_t>(actor) + RE::RefProtection::kActorFormIDOffset);
		return ProtectedRefRegistry::Get().IsProtected(formID);
	}

	class DeathGuardHookCode : public Xbyak::CodeGenerator
	{
	public:
		DeathGuardHookCode(void * buf, uintptr_t isProtectedActor, uintptr_t nativeIsEssential, uintptr_t deathPath, uintptr_t essentialPath)
			: Xbyak::CodeGenerator(256, buf)
		{
			// RSI is the victim Actor* at this guard.
			mov(rcx, rsi);
			mov(rax, isProtectedActor);
			call(rax);

			test(al, al);
			jnz("essentialPath");

			mov(rcx, rsi);
			mov(rax, nativeIsEssential);
			call(rax);

			test(al, al);
			jz("deathPath");

			L("essentialPath");
			mov(rax, essentialPath);
			jmp(rax);

			L("deathPath");
			mov(rax, deathPath);
			jmp(rax);
		}
	};

	class CrosshairIconHookCode : public Xbyak::CodeGenerator
	{
	public:
		CrosshairIconHookCode(void * buf, uintptr_t isProtectedActor, uintptr_t nativeIsEssential, uintptr_t normalReturn, uintptr_t essentialReturn)
			: Xbyak::CodeGenerator(256, buf)
		{
			// RBX is the target Actor* for this crosshair icon query.
			mov(rcx, rbx);
			mov(rax, isProtectedActor);
			call(rax);

			test(al, al);
			jnz("essentialReturn");

			test(rbx, rbx);
			jz("normalReturn");

			mov(rcx, rbx);
			mov(rax, nativeIsEssential);
			call(rax);

			test(al, al);
			jz("normalReturn");

			L("essentialReturn");
			mov(rax, essentialReturn);
			jmp(rax);

			L("normalReturn");
			mov(rax, normalReturn);
			jmp(rax);
		}
	};

	void InstallDeathGuardHook()
	{
		uintptr_t hookSite = RelocationManager::s_baseAddr + RE::RefProtection::kDeathGuardHookRVA;
		uintptr_t nativeIsEssential = RelocationManager::s_baseAddr + RE::RefProtection::kNativeIsEssentialRVA;
		uintptr_t deathPath = RelocationManager::s_baseAddr + RE::RefProtection::kDeathGuardDeathPathRVA;
		uintptr_t essentialPath = RelocationManager::s_baseAddr + RE::RefProtection::kDeathGuardEssentialFallthroughRVA;

		void * codeBuf = g_localTrampoline.startAlloc();
		DeathGuardHookCode code(codeBuf, uintptr_t(&IsProtectedActor), nativeIsEssential, deathPath, essentialPath);
		code.ready();
		g_localTrampoline.endAlloc(code.getCurr());

		g_branchTrampoline.write5Branch(hookSite, uintptr_t(codeBuf));

		for(u32 i = 5; i < RE::RefProtection::kDeathGuardPatchSize; i++)
			safeWrite8(hookSite + i, 0x90);

		_MESSAGE("ref protection death guard hook installed at %016I64X", hookSite);
	}

	void InstallCrosshairIconHook()
	{
		uintptr_t hookSite = RelocationManager::s_baseAddr + RE::RefProtection::kCrosshairIconHookRVA;
		uintptr_t nativeIsEssential = RelocationManager::s_baseAddr + RE::RefProtection::kNativeIsEssentialRVA;
		uintptr_t normalReturn = RelocationManager::s_baseAddr + RE::RefProtection::kCrosshairIconNormalReturnRVA;
		uintptr_t essentialReturn = RelocationManager::s_baseAddr + RE::RefProtection::kCrosshairIconEssentialReturnRVA;

		void * codeBuf = g_localTrampoline.startAlloc();
		CrosshairIconHookCode code(codeBuf, uintptr_t(&IsProtectedActor), nativeIsEssential, normalReturn, essentialReturn);
		code.ready();
		g_localTrampoline.endAlloc(code.getCurr());

		g_branchTrampoline.write5Branch(hookSite, uintptr_t(codeBuf));

		for(u32 i = 5; i < RE::RefProtection::kCrosshairIconPatchSize; i++)
			safeWrite8(hookSite + i, 0x90);

		_MESSAGE("ref protection crosshair icon hook installed at %016I64X", hookSite);
	}
}

void Hooks_RefProtection_Apply()
{
	InstallDeathGuardHook();
	InstallCrosshairIconHook();
}
