#ifndef _ITEMMANAGER_H__
#define _ITEMMANAGER_H__

#include <unordered_map>
#include <vector>
#include "shippart.h"

class GameObjectPool;
class Renderer;

using namespace std;

class ItemManager {
public:
	ItemManager() = default;
	~ItemManager() = default;

	static ItemManager& GetInstance() {
		static ItemManager instance;
		return instance;
	}

	void CreatePool(Renderer& renderer);
	GameObjectPool* GetPool();

	void MarkCollected(ShipPart* item);
	bool IsCollected(ShipPart* item) const;

	void ResetAllItems();

protected:

private:
	ItemManager(const ItemManager& itemManager);
	ItemManager& operator=(const ItemManager& itemManager);

public:

protected:
	unordered_map<ShipPart*, bool> m_itemStates;
	GameObjectPool* m_allItems;

private:
};

#endif // !_ITEMMANAGER_H__
