#include "Result.h"
#include "sceneManager.h"

void Result::initialize()
{
	Scene::initialize();
}

void Result::deinitialize()
{

}

void Result::update(float elapsedTime)
{
	switch (state_){

	case 0:
		state_++;

	case 1:

		break;
	}
}

void Result::render(ID3D11DeviceContext* dc) {

}
