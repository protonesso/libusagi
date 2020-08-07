#include <archive.h>
#include <archive_entry.h>

int copy_data(struct archive *ar, struct archive *aw);
void extract_package(const char *file, const char *path);
void create_package(const char *out, const char **file);
