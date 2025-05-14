#ifndef _WEAPONSTATE_H__
#define _WEAPONSTATE_H__

#include <unordered_map>

class WeaponState {
public:
	static WeaponState& GetInstance();
	static void DestroyInstance();
	void DebugDraw();

	void CreateWeaponMap();
	void SetCurrentWeapon(int weapon);

	int GetNextWeapon();
	int GetCurrentWeapon();

	void AddWeapon(int weapon);

	int GetWeaponUpgrade();

	void Reset();

protected:

private:
	WeaponState();
	~WeaponState();
	WeaponState(const WeaponState& weaponState);
	WeaponState& operator=(const WeaponState& weaponState);

public:

protected:
	static WeaponState* sm_wInstance;

	std::unordered_map<int, bool> m_weaponsObtained;

	int m_currentWeapon;
	int m_nextWeapon;

	bool m_playerAlive = true;

private:
};

#endif // !_WEAPONSTATE_H__