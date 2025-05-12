#ifndef _ITEMMANAGER_H__
#define _ITEMMANAGER_H__

#include <unordered_map>
#include <vector>
#include "shippart.h"

using namespace std;

class ItemManager {
public:
	ItemManager() = default;
	~ItemManager() = default;

	static ItemManager& GetInstance() {
		static ItemManager instance;
		return instance;
	}

	void RegisterItem(ShipPart* item);
	void UnregisterItem(ShipPart* item);

	void MarkCollected(ShipPart* item);
	bool IsCollected(ShipPart* item) const;

	void ResetAllItems();

	const std::vector<ShipPart*> GetAllItems() const;

protected:

private:
	ItemManager(const ItemManager& itemManager);
	ItemManager& operator=(const ItemManager& itemManager);

public:

protected:
	unordered_map<ShipPart*, bool> m_itemStates;
	std::vector<ShipPart*> m_allItems;

private:
};

#endif // !_ITEMMANAGER_H__
