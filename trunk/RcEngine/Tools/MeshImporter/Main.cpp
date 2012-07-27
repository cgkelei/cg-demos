#include "AssimpProcesser.h"
#include <unordered_map>

int main(int argc, char** argv)
{
	AssimpProcesser processer;
	processer.Process("media/tiny.x");
	return 0;
}