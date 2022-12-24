#include "tools/Event.h"

namespace fisk::tools
{
	EventRegistration::~EventRegistration()
	{
		myDeregistraion();
	}
}