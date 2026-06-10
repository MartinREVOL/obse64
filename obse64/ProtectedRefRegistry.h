#pragma once

#include "obse64_common/Types.h"
#include <mutex>
#include <unordered_set>

class ProtectedRefRegistry
{
public:
	static ProtectedRefRegistry & Get();

	void SetProtected(u32 formID, bool protect);
	bool IsProtected(u32 formID) const;
	void Clear();

private:
	ProtectedRefRegistry() = default;

	mutable std::mutex m_mutex;
	std::unordered_set<u32> m_refs;
};
