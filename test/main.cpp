#include "../src/dwcore.h"

bool TestElevationCompression();

int main(int argc, const char* argv[])
{
	int numFailedTests = 0;

	if(!TestElevationCompression()) numFailedTests++;
	
	return numFailedTests;
}
