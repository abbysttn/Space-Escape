#include "itemmanager.h"

#include "gameobjectpool.h"
#include "shippart.h"
#include "Renderer.h"

#include "logmanager.h"

void ItemManager::CreatePool(Renderer& renderer)
{
	m_itemStates.clear();

	m_allItems = new GameObjectPool(ShipPart(), 7);

	for (size_t i = 0; i < m_allItems->totalCount(); i++) {
		if (GameObject* obj = m_allItems->getObjectAtIndex(i)) {
			ShipPart* part = static_cast<ShipPart*>(obj);
			if (!part->Initialise(renderer)) {
				return;
			}

			if (m_itemStates.find(part) == m_itemStates.end()) {
				m_itemStates.try_emplace(part, false);
			}
		}
	}
}

GameObjectPool* ItemManager::GetPool()
{
	return m_allItems;
}

void ItemManager::MarkCollected(ShipPart* item)
{
	if (!item) return;

	auto part = m_itemStates.find(item);

	if (part != m_itemStates.end()) {
		part->second = true;
		item->SetCollected(true);
	}
}

bool ItemManager::IsCollected(ShipPart* item) const
{
	if (!item) return false;

	auto part = m_itemStates.find(item);

	if (part != m_itemStates.end()) {
		return part->second;
	}

	return false;
}

void ItemManager::ResetAllItems()
{
	for (size_t i = 0; i < m_allItems->totalCount(); i++) {
		if (GameObject* obj = m_allItems->getObjectAtIndex(i)) {
			ShipPart* part = static_cast<ShipPart*>(obj);
			m_allItems->release(part);
		}
	}

	delete m_allItems;
	m_allItems = 0;

	m_itemStates.clear();
}

int ItemManager::GetItemCollectedCount()
{
	int count = 0;

	for (size_t i = 0; i < m_allItems->totalCount(); i++) {
		if (GameObject* obj = m_allItems->getObjectAtIndex(i)) {
			ShipPart* part = static_cast<ShipPart*>(obj);
			if (m_itemStates.find(part)->second == true) count++;
		}
	}

	return count;
}

