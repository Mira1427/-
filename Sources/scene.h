#pragma once

struct ID3D11DeviceContext;
class GameObject;

class Scene {
public:
	virtual void initialize(){ state_ = 0; }
	virtual void deinitialize() = 0;
	virtual void update(float elapsedTime) = 0;
	virtual void render(ID3D11DeviceContext* dc) = 0;
	virtual void ImGui() {};

	GameObject* camera_;

protected:
	int state_;
};