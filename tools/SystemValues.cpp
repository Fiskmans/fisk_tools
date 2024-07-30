#include "SystemValues.h"

bool fisk::tools::SystemValues::Process(DataProcessor& aProcessor)
{
	bool success = true;

	success &= aProcessor.Process(myFloatValues);
	success &= aProcessor.Process(myDoubleValues);
	success &= aProcessor.Process(myLongDoubleValues);

	success &= aProcessor.Process(myCharValues);
	success &= aProcessor.Process(myShortValues);
	success &= aProcessor.Process(myIntValues);
	success &= aProcessor.Process(myLongValues);
	success &= aProcessor.Process(myLongLongValues);
	success &= aProcessor.Process(mySizeTValues);

	return success;
}
