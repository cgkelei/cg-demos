#include "AssimpProcesser.h"
#include <unordered_map>

int main(int argc, char** argv)
{
	AssimpProcesser processer;
	processer.Process("media/Dwarves/dwarf-lod0_rotating_hand.X");
	return 0;
}