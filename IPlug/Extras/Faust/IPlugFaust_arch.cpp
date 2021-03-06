<<includeIntrinsic>>

#define FAUSTFLOAT sample

#include "IPlugFaust.h"

/* BEGIN AUTO GENERATED BY THE FAUST COMPILER ... */
//#define max std::max
//#define min std::min
<<includeclass>>
//#undef max
//#undef min
/* ... END AUTO GENERATED BY THE FAUST COMPILER  */

class Faust_mydsp : public IPlugFaust
{
public:
	Faust_mydsp(const char* name, const char* inputDSPFile = 0, int nVoices = 1, int rate = 1,
						const char* outputCPPFile = 0, const char* drawPath = 0, const char* libraryPath = DEFAULT_FAUST_LIBRARY_PATH)
	: IPlugFaust(name, nVoices)
	{
	}

	void Init() override
	{
		mDSP = std::make_unique<FAUSTCLASS>();
		mDSP->buildUserInterface(this);
		BuildParameterMap();
		mInitialized = true;
	}
};

#undef FAUSTCLASS
