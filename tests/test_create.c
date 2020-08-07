#include "archive.h"

int main(int argc, const char **argv)
{
  const char *outname;
  argv++;
  outname = *argv++;
  create_package(outname, argv);
  return 0;
}
