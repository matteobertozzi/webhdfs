/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define _FILE_OFFSET_BITS   64
#define FUSE_USE_VERSION    28
#include <fuse.h>

#include <webhdfs/webhdfs.h>

#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

struct webhdfs_fuse {
    webhdfs_t *webhdfs;
    FILE *flog;
};

static struct webhdfs_fuse __webhdfs_fuse;

/* ============================================================================
 *  webhdfs Fuse
 */
static int webhdfs_fuse_connect (void) {
    if ((__webhdfs_fuse.flog = fopen("webhdfs-fuse.log", "a")) == NULL) {
        perror("fopen() log file:");
        return(-1);
    }

    return(0);
}

static void webhdfs_fuse_disconnect (void) {
    fclose(__webhdfs_fuse.flog);
}

#define webhdfs_fuse_log(format, ...)                           \
    do {                                                        \
        fprintf(__webhdfs_fuse.flog, format, ##__VA_ARGS__);    \
        fflush(__webhdfs_fuse.flog);                            \
    } while (0)

/* ============================================================================
 * Metadata related functions
 */
static int webhdfs_fuse_statfs (const char *path, struct statvfs *stat) {
    webhdfs_fuse_log("__fuse_statfs() %s\n", path);

    if (0) {
        stat->f_bsize   = 0x0000;       /* file system block size */
        stat->f_frsize  = 0x0000;       /* fragment size */
        stat->f_blocks  = 0x0000;       /* size of fs in f_frsize units */
        stat->f_bfree   = 0x0000;       /* # free blocks */
        stat->f_bavail  = 0x0000;       /* # free blocks for unprivileged users */
        stat->f_files   = 0x0000;       /* # inodes */
        stat->f_ffree   = 0x0000;       /* # free inodes */
        stat->f_favail  = 0x0000;       /* # free inodes for unprivileged users */
        stat->f_fsid    = 0x0000;       /* file system ID */
        stat->f_flag    = 0x0000;       /* mount flags */
        stat->f_namemax = 0x0000;       /* maximum filename length */
    }

    return(-1);
}

static int webhdfs_fuse_getattr (const char *path, struct stat *stat) {
    webhdfs_fuse_log("__fuse_getattr() %s\n", path);

    if (0) {
        stat->st_dev     = 0;           /* ID of device containing file */
        stat->st_ino     = 0;           /* inode number */
        stat->st_rdev    = 0;           /* device ID (if special file) */
        stat->st_uid     = 0x0000;      /* user ID of owner */
        stat->st_gid     = 0x0000;      /* group ID of owner */
        stat->st_mode    = 0x0000;      /* protection */
        stat->st_nlink   = 1;           /* number of hard links */
        stat->st_size    = 0;           /* total size, in bytes */
        stat->st_blksize = 0;           /* blocksize for file system I/O */
        stat->st_blocks  = 0x0000;      /* number of 512B blocks allocated */
        stat->st_atime   = 0x0000;      /* time of last access */
        stat->st_mtime   = 0x0000;      /* time of last modification */
        stat->st_ctime   = 0x0000;      /* time of last status change */
    }

    return(-1);
}

static int webhdfs_fuse_setxattr (const char *path,
                                  const char *name,
                                  const char *value,
                                  size_t size,
                                  int flags)
{
    return(-1);
}

static int webhdfs_fuse_getxattr (const char *path,
                                  const char *name,
                                  char *value,
                                  size_t size)
{
    return(-1);
}

static int webhdfs_fuse_listxattr (const char *path, char *list, size_t size) {
    return(-1);
}

static int webhdfs_fuse_removexattr (const char *path, const char *name) {
    return(-1);
}

static int webhdfs_fuse_chmod (const char *path, mode_t mode) {
    return(-1);
}

static int webhdfs_fuse_chown (const char *path, uid_t uid, gid_t gid) {
    return(-1);
}

/* ============================================================================
 * Namespace related functions
 */
static int webhdfs_fuse_create (const char *path,
                                mode_t mode,
                                struct fuse_file_info *ffi)
{
    return(-1);
}

static int webhdfs_fuse_open (const char *path, struct fuse_file_info *ffi) {
    return(-1);
}

static int webhdfs_fuse_mknod (const char *path, mode_t mode, dev_t dev) {
    return(-1);
}

static int webhdfs_fuse_mkdir (const char *path, mode_t mode) {
    return(-1);
}

static int webhdfs_fuse_rmdir (const char *path) {
    return(-1);
}

static int webhdfs_fuse_unlink (const char *path) {
    return(-1);
}

static int webhdfs_fuse_rename (const char *path, const char *newpath) {
    return(-1);
}

static int webhdfs_fuse_link (const char *path, const char *newpath) {
    return(-1);
}

static int webhdfs_fuse_symlink (const char *path, const char *newpath) {
    return(-1);
}

static int webhdfs_fuse_readlink (const char *path, char *buffer, size_t size) {
    return(-1);
}

/* ============================================================================
 * Object related functions
 */
static int webhdfs_fuse_truncate (const char *path, off_t size) {
    return(-1);
}

static int webhdfs_fuse_read (const char *path,
                              char *buffer,
                              size_t size,
                              off_t offset,
                              struct fuse_file_info *ffi)
{
    return(-1);
}

static int webhdfs_fuse_write (const char *path,
                               const char *buffer,
                               size_t size,
                               off_t offset,
                               struct fuse_file_info *ffi)
{
    return(-1);
}

static int webhdfs_fuse_fsync (const char *path,
                               int data_sync,
                               struct fuse_file_info *ffi)
{
    return(-1);
}

static void __signal_sigsegv (int signo) {
    void *buffer[10];
    char **symbols;
    size_t i, size;

    fprintf(stderr, "Segfault backtrace:\n");
    size = backtrace(buffer, 10);
    symbols = backtrace_symbols(buffer, size);

    for (i = 0; i < size; ++i)
        fprintf(stderr, " - %s\n", symbols[i]);

    abort();
}

static struct fuse_operations webhdfs_fuse_ops = {
    /* Metadata */
    .getattr        = webhdfs_fuse_getattr,
    .setxattr       = webhdfs_fuse_setxattr,
    .getxattr       = webhdfs_fuse_getxattr,
    .listxattr      = webhdfs_fuse_listxattr,
    .removexattr    = webhdfs_fuse_removexattr,
    .chmod          = webhdfs_fuse_chmod,
    .chown          = webhdfs_fuse_chown,

    /* Namespace */
    .create         = webhdfs_fuse_create,
    .open           = webhdfs_fuse_open,
    .mknod          = webhdfs_fuse_mknod,
    .mkdir          = webhdfs_fuse_mkdir,
    .rmdir          = webhdfs_fuse_rmdir,
    .unlink         = webhdfs_fuse_unlink,
    .rename         = webhdfs_fuse_rename,
    .link           = webhdfs_fuse_link,
    .symlink        = webhdfs_fuse_symlink,
    .readlink       = webhdfs_fuse_readlink,

    /* Object */
    .truncate       = webhdfs_fuse_truncate,
    .read           = webhdfs_fuse_read,
    .write          = webhdfs_fuse_write,
    .fsync          = webhdfs_fuse_fsync,
    .ioctl          = NULL,
    .lock           = NULL,
};

int main (int argc, char **argv) {
    int res;

    if (signal(SIGSEGV, __signal_sigsegv) == SIG_ERR) {
        fprintf(stderr, "Failed to initialize signals\n");
        return(EXIT_FAILURE);
    }

    if (webhdfs_fuse_connect() < 0)
        return(-1);

    res = fuse_main(argc, argv, &webhdfs_fuse_ops, NULL);
    webhdfs_fuse_disconnect();

    return(res);
}

