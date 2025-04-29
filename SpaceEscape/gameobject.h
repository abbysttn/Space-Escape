#ifndef _GAMEOBJECT_H__
#define _GAMEOBJECT_H__

class GameObject {
public:
	GameObject() : x(0), y(0), active(false) {}
	virtual ~GameObject() {}

	virtual void reset() = 0;
	virtual bool isActive() const = 0;

	virtual GameObject* Create() const = 0;

	float x, y;
	bool active;

protected:
private:
	GameObject(const GameObject& gameobject);
	GameObject& operator=(const GameObject& gameobject);
};

#endif // !_GAMEOBJECT_H__
