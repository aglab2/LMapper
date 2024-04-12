#include "InputFactory.h"

namespace Mapping
{
	void InputFactory::PollInput()
	{
		XINPUT_STATE state;
		XInputGetState(currentId_, &state);
		polledId_ = currentId_;
		controller_ = state.Gamepad;
	}
}