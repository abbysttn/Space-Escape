#include "itemmanager.h"

void ItemManager::RegisterItem(ShipPart* item)
{
	if (!item) return;

	if (m_itemStates.find(item) == m_itemStates.end()) {
		m_itemStates[item] = false;
		m_allItems.push_back(item);
	}
}

void ItemManager::UnregisterItem(ShipPart* item)
{
	if (!item) return;

	m_itemStates.erase(item);

	m_allItems.erase(remove(m_allItems.begin(), m_allItems.end(), item), m_allItems.end());
}

void ItemManager::MarkCollected(ShipPart* item)
{
	if (!item) return;

	auto it = m_itemStates.find(item);
	if (it != m_itemStates.end()) {
		it->second = true;
		item->SetCollected(true);
	}
}

bool ItemManager::IsCollected(ShipPart* item) const
{
	if (!item) return false;

	auto it = m_itemStates.find(item);
	if (it != m_itemStates.end()) {
		return it->second;
	}
	return false;
}

void ItemManager::ResetAllItems()
{
	for (auto& pair : m_itemStates) {
		ShipPart* item = pair.first;
		pair.second = false;
		item->SetCollected(false);
		item->SetActive(true);
	}
}

const std::vector<ShipPart*> ItemManager::GetAllItems() const
{
	return m_allItems;
}
