#include "AssimpProcesser.h"
#include <unordered_map>

int main(int argc, char** argv)
{
	AssimpProcesser processer;
	processer.Process("media/bat.x");
	return 0;
}