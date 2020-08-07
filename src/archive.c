/*
 * libusagi - A package management library
 * Package archive reading library
 */

#include <sys/stat.h> 
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "archive.h"
#include "utils.h"

/* FROM https://github.com/libarchive/libarchive/wiki/Examples#A_Complete_Extractor */
int copy_data(struct archive *ar, struct archive *aw) {
	int r;
	const void *buff;
	size_t size;
	la_int64_t offset;

	for (;;) {
		r = archive_read_data_block(ar, &buff, &size, &offset);
		if (r == ARCHIVE_EOF)
			return (ARCHIVE_OK);
		if (r < ARCHIVE_OK)
			return (r);
		r = archive_write_data_block(aw, buff, size, offset);
		if (r < ARCHIVE_OK) {
			fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(aw));
			return (r);
		}
	}
}
/* FROM END */

void extract_package(const char *file, const char *path) {
	int r, flags;
	ssize_t buf = 8192;
	struct archive *a = archive_read_new();
	struct archive *ext = archive_write_disk_new();
	struct archive_entry *entry;

	flags =  ARCHIVE_EXTRACT_OWNER | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_TIME;
	flags += ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS | ARCHIVE_EXTRACT_XATTR;
	flags += ARCHIVE_EXTRACT_SECURE_SYMLINKS | ARCHIVE_EXTRACT_SECURE_NODOTDOT;

	archive_read_support_filter_zstd(a);
	archive_read_support_format_cpio(a);
	archive_write_disk_set_options(ext, flags);
	archive_write_disk_set_standard_lookup(ext);

	if (archive_read_open_filename(a, file, buf) < 0) {
		fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(a));
		exit(1);
	}

	chdir(path);
	for (;;) {
		r = archive_read_next_header(a, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r != ARCHIVE_OK) {
			fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(a));
			exit(1);
		}

		r = archive_write_header(ext, entry);
		if (r != ARCHIVE_OK) {
			fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(a));
			exit(1);
		} else if (archive_entry_size(entry) > 0) {
			r = copy_data(a, ext);
			if (r != ARCHIVE_OK) {
				fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(a));
				exit(1);
			}
		}
		r = archive_write_finish_entry(ext);
		if (r != ARCHIVE_OK) {
			fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(a));
			exit(1);
		}
	}

	archive_write_close(ext);
	archive_write_free(ext);
	archive_read_close(a);
	archive_read_free(a);
	exit(0);
}

void create_package(const char *file, const char *out) {
	exit(0);
}
