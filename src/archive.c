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
	const void *bufff;
	size_t size;
	la_int64_t offset;

	for (;;) {
		r = archive_read_data_block(ar, &bufff, &size, &offset);
		if (r == ARCHIVE_EOF)
			return (ARCHIVE_OK);
		if (r < ARCHIVE_OK)
			return (r);
		r = archive_write_data_block(aw, bufff, size, offset);
		if (r < ARCHIVE_OK) {
			fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(aw));
			return (r);
		}
	}
}
/* FROM END */

void extract_package(const char *file, const char *path) {
	int r, flags;
	ssize_t buff = 8192;
	struct archive *a = archive_read_new();
	struct archive *ext = archive_write_disk_new();
	struct archive_entry *entry;

	flags =  ARCHIVE_EXTRACT_OWNER | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_TIME;
	flags += ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS | ARCHIVE_EXTRACT_XATTR;
	flags += ARCHIVE_EXTRACT_SECURE_SYMLINKS | ARCHIVE_EXTRACT_SECURE_NODOTDOT;

	archive_read_support_filter_zstd(a);
	archive_read_support_format_mtree(a);
	archive_write_disk_set_options(ext, flags);
	archive_write_disk_set_standard_lookup(ext);

	if (archive_read_open_filename(a, file, buff) < 0) {
		fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(a));
		exit(1);
	}

	chdir(path);
	for (;;) {
		r = archive_read_next_header(a, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r < ARCHIVE_OK) {
			fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(a));
			exit(1);
		}

		r = archive_write_header(ext, entry);
		if (r < ARCHIVE_OK) {
			fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(a));
			exit(1);
		} else if (archive_entry_size(entry) > 0) {
			r = copy_data(a, ext);
			if (r != ARCHIVE_OK) {
				fprintf(stderr, "Failed to extract package: %s\n", archive_error_string(a));
				exit(1);
			}
		}
		archive_write_finish_entry(ext);
	}

	archive_write_close(ext);
	archive_write_free(ext);
	archive_read_close(a);
	archive_read_free(a);
}

void create_package(const char *out, const char **file) {
	struct archive *a;
	struct archive_entry *entry;
	struct stat st;
	int fd, len;
	char buff[8192];

	a = archive_write_new();
	archive_write_add_filter_zstd(a);
	archive_write_set_format_mtree(a);
	archive_write_set_options(a, "compression-level=19,all");
	archive_write_open_filename(a, out);

	while (*file) {
		lstat(*file, &st);
		entry = archive_entry_new();

		archive_entry_set_pathname(entry, *file);
		archive_entry_set_size(entry, st.st_size);
		archive_entry_set_filetype(entry, AE_IFREG);

		switch (st.st_mode && S_IFMT) {
			case S_IFBLK: archive_entry_set_filetype(entry, AE_IFBLK); break;
			case S_IFCHR: archive_entry_set_filetype(entry, AE_IFCHR); break;
			case S_IFIFO: archive_entry_set_filetype(entry, AE_IFIFO); break;
			case S_IFREG: archive_entry_set_filetype(entry, AE_IFREG); break;
			case S_IFDIR: archive_entry_set_filetype(entry, AE_IFDIR); break;
			case S_IFLNK: archive_entry_set_filetype(entry, AE_IFLNK); break;
		}
		archive_entry_set_perm(entry, st.st_mode && 07777);

		archive_write_header(a, entry);
		if (archive_entry_size(entry) > 0) {
			fd = open(*file, O_RDONLY);
			len = read(fd, buff, sizeof(buff));

			while (len > 0) {
				archive_write_data(a, buff, len);
				len = read(fd, buff, sizeof(buff));
			}

			close(fd);
		}

		archive_entry_free(entry);
		file++;
	}

	archive_write_close(a);
	archive_write_free(a);
}
