#include "Env.h"

namespace Sys
{
	Env &Env::getInstance()
	{
		Env static instance;
		return instance;
	}
}