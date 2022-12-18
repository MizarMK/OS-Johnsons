/*
  Hoofs (pronounce Hooves) Filesystem

  gcc -Wall `pkg-config fuse --cflags --libs` hoofs.c -o hoofs
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>

static char *fileSystemRoot;

/*
 * Write given log message to console
 */
static void logMessage(const char *format, ...) {
  va_list argList;
  va_start(argList, format);
  vfprintf(stdout, format, argList);
}

/*
 * Compute the byte length of the full path string 
 * from the root (fileSystemRoot + path)
 */
static size_t getFullPathLength(const char *path) {
  return strlen(fileSystemRoot) + strlen(path) + 1;
}

/*
 * Compute the full path from the root (fileSystemRoot + path).
 * We assume the user only wants up to n-1 bytes of the full path.
 */
static char *getFullPath(const char *path, char *fullPath, size_t n) {
  strncpy(fullPath, fileSystemRoot, n);
  strncat(fullPath, path, n);

  return fullPath;
}

/*
 * Get file attributes
 */
static int hoofs_getattr(const char *path, struct stat *stbuf) {
  // Compute the full path name
  size_t pathLen = getFullPathLength(path);
  char fullPath[pathLen];
  getFullPath(path, fullPath, pathLen);
  logMessage("Getting attributes for %s\n", fullPath);

  // Fetch file stats from local file system
  if (lstat(fullPath, stbuf) == -1) {
    logMessage("lstat() failed: %s\n", strerror(errno));
    return -errno;
  }
  return 0;
}

static int hoofs_setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
  printf("setxattr\n");
  return 0;
}

static int hoofs_chmod(const char *path, mode_t mode) {
  printf("chmod\n");
  return 0;
}

static int hoofs_chown(const char *path, uid_t uid, gid_t gid) {
  printf("chown\n");
  return 0;
}

static int hoofs_utime(const char *path, struct utimbuf *ubuf) {
  printf("utime\n");
  return 0;
}

static int hoofs_truncate(const char *path, off_t newsize) {
  printf("truncate\n");
  return 0;
}

static int hoofs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
  // Compute the full path name
  size_t pathLen = getFullPathLength(path);
  char fullPath[pathLen];
  getFullPath(path, fullPath, pathLen);
  logMessage("Reading directory for %s\n", fullPath);

  // Fetch the directory information
  DIR *dirPtr = opendir(fullPath);
  if (dirPtr == NULL) {
    logMessage("opendir() failed: %s\n", strerror(errno));
    return -errno;
  }

  struct dirent *dirEntry;
  while ((dirEntry = readdir(dirPtr)) != NULL) {
    if (filler(buf, dirEntry->d_name, NULL, 0) != 0) {
      return -ENOMEM;
    }
  }

  closedir(dirPtr);

  return 0;
}

static int hoofs_open(const char *path, struct fuse_file_info *fi) {
  // Compute the full path name
  size_t pathLen = getFullPathLength(path);
  char fullPath[pathLen];
  getFullPath(path, fullPath, pathLen);
  logMessage("Opening file %s\n", fullPath);

  int fd = open(fullPath, fi->flags);
  if (fd <= 0) {
    logMessage("open() failed: %s\n", strerror(errno));
    return -errno;
  }
  fi->fh = (uint64_t) fd;

  return 0;
}

static int hoofs_release(const char *path, struct fuse_file_info *fi) {
    if (close((int)fi->fh) < 0) {
    logMessage("closeOB() failed: %s\n", strerror(errno));
    return -errno;
  }

  return 0;
}

static int hoofs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
  // Compute the full path name
  size_t pathLen = getFullPathLength(path);
  char fullPath[pathLen];
  getFullPath(path, fullPath, pathLen);
  logMessage("Creating file %s\n", fullPath);

  int fd = creat(fullPath, mode);
  if (fd < 0) {
    logMessage("creat() failed: %s\n", strerror(errno));
    return -errno;
  }
  fi->fh = (uint64_t) fd;

  return 0;
}

static int hoofs_unlink(const char *path) {
  // Compute the full path name
  size_t pathLen = getFullPathLength(path);
  char fullPath[pathLen];
  getFullPath(path, fullPath, pathLen);
  logMessage("Deleting file/directory %s\n", fullPath);

  int retVal = remove(fullPath);
  if (retVal < 0) {
    logMessage("remove() failed: %s\n", strerror(errno));
    return -errno;
  }

  return 0;
}

static int hoofs_rmdir(const char *path) {
  return hoofs_unlink(path);
}

static int hoofs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
  logMessage("Reading from open file\n");

  // Go to file offset
  if (lseek((int) fi->fh, offset, SEEK_SET) < 0) {
    logMessage("lseek() failed: %s\n", strerror(errno));
    return -errno;
  }

  // Read bytes
  ssize_t readBytes = read((int) fi->fh, buf, size);
  if (readBytes < 0) {
    logMessage("read() failed: %s\n", strerror(errno));
    return -errno;
  }

  return (int) readBytes;
}

static int hoofs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
  logMessage("Writing to file\n");

  // Go to file offset
  if (lseek((int) fi->fh, offset, SEEK_SET) < 0) {
    logMessage("lseek() failed: %s\n", strerror(errno));
    return -errno;
  }

  // Write bytes
  ssize_t writtenBytes = write((int) fi->fh, buf, size);
  if (writtenBytes < 0) {
    logMessage("write() failed: %s\n", strerror(errno));
    return -errno;
  }

  return (int) writtenBytes;
}

static struct fuse_operations hoofs_oper = {
  .getattr = hoofs_getattr,
  .setxattr = hoofs_setxattr,
  .chmod = hoofs_chmod,
  .chown = hoofs_chown,
  .utime = hoofs_utime,
  .truncate = hoofs_truncate,
  .readdir = hoofs_readdir,
  .open = hoofs_open,
  .release = hoofs_release,
  .create = hoofs_create,
  .unlink = hoofs_unlink,
  .rmdir = hoofs_rmdir,
  .read = hoofs_read,
  .write = hoofs_write,
};

static int myfs_opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs) {
  if (key == FUSE_OPT_KEY_NONOPT && fileSystemRoot == NULL) {
    fileSystemRoot = strdup(arg);
    return 0;
  }
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <filesystem root> <fuse params>...\n", argv[0]);
    exit(1);
  }
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

  fuse_opt_parse(&args, NULL, NULL, myfs_opt_proc);

  return fuse_main(args.argc, args.argv, &hoofs_oper, NULL);
}
