#define DOCTEST_CONFIG_IMPLEMENT
#include <3rdParty/doctest.h>

#include "TestFixture.h"

int main(int argc, char** argv) {
	doctest::Context context;

	context.applyCommandLine(argc, argv);

	int res;
	{
		TestFixture fixture;
		res = context.run();
	}

	if(context.shouldExit()) return res;

	return res;
}
