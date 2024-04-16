#include "ButtonBehavior.h"

#include "./Library/Library.h"

#include "./Component.h"
#include "./game.h"


void ButtonManagerBehavior::execute(GameObject* obj, float elapsedTime)
{
	auto* button = obj->getComponent<ButtonComponent>();

	switch (obj->state_)
	{
	case 0:

		obj->state_++;
		[[fallthrough]];

	case 1:

		// --- ���͂ŃC���f�b�N�X��ύX ---
		if (input::trigger(input::UP))
			button->index_--;

		if (input::trigger(input::DOWN))
			button->index_++;

		// --- �C���f�b�N�X��͈͓��Ɏ��߂� ---
		button->index_ = (std::max)(button->index_, 0);
		button->index_ = (std::min)(button->index_, 2);

		break;
	}
}


// --- ���V�[���̐ݒ� ---
void SceneTransitionBehavior::setScene(Scene& scene)
{
	SceneManager::instance().nextScene = &scene;
}



// --- �^�C�g����ʂ���Q�[����ʂւ̃{�^�� ---
void TitleStartButtonBehavior::execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:

		obj->state_++;
		[[fallthrough]];

	case 1:

	{
		// --- �e�̎擾 ---
		auto* parent = obj->parent_->getComponent<ButtonComponent>();

		// --- �|�C���^�̊m�F ---
		if (!parent)
			return;

		// --- �J�ڏ��� ---
		if (parent->index_ == 0/*�X�^�[�g*/ && input::trigger(input::ENTER))
			setScene(Game::instance());
	}

		break;
	}
}


// --- �^�C�g����ʂ���I������{�^�� ---
void TitleEndButtonBehavior::execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:

		obj->state_++;
		[[fallthrough]];

	case 1:

	{
		// --- �e�̎擾 ---
		auto* parent = obj->parent_->getComponent<ButtonComponent>();

		// --- �|�C���^�̎擾 ---
		if (!parent)
			break;

		// --- �I������ ---
		if (parent->index_ == 2/*�I��*/ && input::trigger(input::ENTER))
			window::close();
	}

	break;
	}
}
