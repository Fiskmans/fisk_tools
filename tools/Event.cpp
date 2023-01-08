#include "tools/Event.h"

namespace fisk::tools
{
	EventRegistration::~EventRegistration()
	{
		if (myDeregistraion)
			myDeregistraion();
	}

} // namespace fisk::tools