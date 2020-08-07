#include "archive.h"

int main(int argc, char *argv[]) {
	char *archive = argv[1];

	//extract_package(archive, "/home/yuyu/test2");
	create_package(archive, "/home/yuyu/seiba.zst");

	return 0;
}
