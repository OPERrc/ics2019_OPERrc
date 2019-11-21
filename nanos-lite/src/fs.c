#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *, size_t, size_t);
size_t ramdisk_write(const void *, size_t, size_t);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

int fs_open(const char *pathname, int flags, int mode) {
  int i;
  for (i = 0; i < NR_FILES; i++)
    if (strcmp(pathname, file_table[i].name) == 0)
      break;
  if (i == NR_FILES)
    assert(0);
  return i;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  assert(offset <= file_table[fd].size);
  switch(whence) {
    case SEEK_SET: file_table[fd].open_offset = offset; break;
    case SEEK_CUR: file_table[fd].open_offset += offset; break;
    case SEEK_END: file_table[fd].open_offset = offset + file_table[fd].size; break;
    default: assert(0); return -1;
  }
  return file_table[fd].open_offset;
}

size_t fs_read(int fd, void *buf, size_t len) {
  assert(file_table[fd].open_offset <= file_table[fd].size);
  if (fd >= NR_FILES)
    return -1;
  
  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  fs_lseek(fd, len, SEEK_CUR);
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  assert(file_table[fd].open_offset <= file_table[fd].size);
  if (fd != 1 && fd != 2) {
    ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
    fs_lseek(fd, len, SEEK_CUR);
    return len;
  }
  else {
    // Log("%s\n", buf);
    uintptr_t i = 0;
    while(i < len && buf) {
      _putc(*(char *)buf++);
      i++;
    }
    return i;
  }
}

int fs_close(int fd) {
  return 0;
}

void init_fs() {
  for (int i = 3; i < NR_FILES; i++) {
    file_table[i].read = &ramdisk_read;
    file_table[i].write = &ramdisk_write;
    file_table[i].open_offset = 0;
  }
  // TODO: initialize the size of /dev/fb
}
