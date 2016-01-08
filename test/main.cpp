#include "../src/dwcore.h"

bool TestElevationCompression();
bool TestSDFRasterizer();

int main(int argc, const char* argv[])
{
	int numFailedTests = 0;

	//if (!TestElevationCompression()) numFailedTests++;
	if (!TestSDFRasterizer()) numFailedTests++;

	return numFailedTests;
}
