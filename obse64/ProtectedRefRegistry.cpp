#include "ProtectedRefRegistry.h"

ProtectedRefRegistry & ProtectedRefRegistry::Get()
{
	static ProtectedRefRegistry s_registry;
	return s_registry;
}

void ProtectedRefRegistry::SetProtected(u32 formID, bool protect)
{
	if(!formID)
		return;

	std::lock_guard<std::mutex> lock(m_mutex);

	if(protect)
		m_refs.insert(formID);
	else
		m_refs.erase(formID);
}

bool ProtectedRefRegistry::IsProtected(u32 formID) const
{
	if(!formID)
		return false;

	std::lock_guard<std::mutex> lock(m_mutex);
	return m_refs.find(formID) != m_refs.end();
}

void ProtectedRefRegistry::Clear()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_refs.clear();
}
