#pragma once

#include "X360Controller.h"

class InputFactory
{
public:
	X360::Controller GetInput()
	{
		if (currentId_ != polledId_)
		{
			PollInput();
		}

		return controller_;
	}
	void SetId(int id)
	{
		currentId_ = id;
	}

private:
	void PollInput();

	int currentId_ = 0;
	int polledId_ = -1;
	X360::Controller controller_;
};
