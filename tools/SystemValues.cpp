#include "SystemValues.h"

bool fisk::tools::SystemValues::Process(DataProcessor& aProcessor)
{
	return aProcessor.Process(myFloatValues)
		&& aProcessor.Process(myDoubleValues)
		&& aProcessor.Process(myLongDoubleValues)
		&& aProcessor.Process(myCharValues)
		&& aProcessor.Process(myShortValues)
		&& aProcessor.Process(myIntValues)
		&& aProcessor.Process(myLongValues)
		&& aProcessor.Process(myLongLongValues)
		&& aProcessor.Process(mySizeTValues);
}
