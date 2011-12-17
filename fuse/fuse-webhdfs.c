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

#include <sys/types.h>
#include <execinfo.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>

struct webhdfs_fuse {
    webhdfs_t *webhdfs;
    FILE *flog;
};

static struct webhdfs_fuse __webhdfs_fuse;

/* ============================================================================
 *  helper/utils
 */
#define webhdfs_fuse_log(format, ...)                           \
    do {                                                        \
        fprintf(__webhdfs_fuse.flog, format, ##__VA_ARGS__);    \
        fflush(__webhdfs_fuse.flog);                            \
    } while (0)

#define __WEBHDFS                   (__webhdfs_fuse.webhdfs)

#define __ceil_div(a, b)            (((a) + (b) - 1) / (b))

/* ============================================================================
 *  user/group utils
 */
#define GROUP_SIZE_MAX      (1024)      /* sysconf(_SC_GETGR_R_SIZE_MAX) */
#define USER_SIZE_MAX       (1024)      /* sysconf(_SC_GETPW_R_SIZE_MAX) */

static int __hdfs_user_uid (const char *name, uid_t *uid) {
    char buffer[USER_SIZE_MAX];
    struct passwd *tmp;
    struct passwd pwd;

    if (getpwnam_r(name, &pwd, buffer, USER_SIZE_MAX, &tmp))
        return(1);

    *uid = pwd.pw_uid;
    return(0);
}

static char *__hdfs_user_name (uid_t uid) {
    char buffer[USER_SIZE_MAX];
    struct passwd *tmp;
    struct passwd pwd;

    if (getpwuid_r(uid, &pwd, buffer, USER_SIZE_MAX, &tmp))
        return(NULL);

    return(pwd.pw_name ? strdup(pwd.pw_name) : NULL);
}

static int __hdfs_group_gid (const char *name, gid_t *gid) {
    char buffer[GROUP_SIZE_MAX];
    struct group *tmp;
    struct group grp;

    if (getgrnam_r(name, &grp, buffer, GROUP_SIZE_MAX, &tmp))
        return(1);

    *gid = grp.gr_gid;
    return(0);
}

static char *__hdfs_group_name (gid_t gid) {
    char buffer[GROUP_SIZE_MAX];
    struct group *tmp;
    struct group grp;

    if (getgrgid_r(gid, &grp, buffer, GROUP_SIZE_MAX, &tmp))
        return(NULL);

    return(grp.gr_name ? strdup(grp.gr_name) : NULL);
}

/* ============================================================================
 *  stat/utils utils
 */
#define HDFS_DEFAULT_GID            99
#define HDFS_DEFAULT_UID            99

#define __hdfs_is_dir(stat)         (!strcmp((stat)->type, "DIRECTORY"))
#define __hdfs_is_file(stat)        (!strcmp((stat)->type, "FILE"))

static void __hdfs_stat (const webhdfs_fstat_t *hdfs_stat, struct stat *stat) {
    memset(stat, 0, sizeof(struct stat));

    if (__hdfs_user_uid(hdfs_stat->owner, &(stat->st_uid)))
        stat->st_uid = HDFS_DEFAULT_UID;
    if (__hdfs_group_gid(hdfs_stat->group, &(stat->st_gid)))
        stat->st_gid = HDFS_DEFAULT_GID;

    if (__hdfs_is_dir(hdfs_stat)) {
        stat->st_mode    = S_IFDIR | hdfs_stat->permission;
        stat->st_nlink   = 2;
        stat->st_size    = 4096;
        stat->st_blksize = 4096;
    } else /* if (__hdfs_is_file(hdfs_stat)) */ {
        stat->st_mode    = S_IFREG | hdfs_stat->permission;
        stat->st_nlink   = 1;
        stat->st_size    = hdfs_stat->length;
        stat->st_blksize = hdfs_stat->block;
    }

    stat->st_blocks  = __ceil_div(stat->st_size, stat->st_blksize);
    stat->st_atime   = hdfs_stat->atime;
    stat->st_mtime   = hdfs_stat->mtime;
    stat->st_ctime   = hdfs_stat->mtime;
}

/* ============================================================================
 *  webhdfs Fuse
 */
static int webhdfs_fuse_connect (const webhdfs_conf_t *config) {
    if ((__webhdfs_fuse.flog = fopen("webhdfs-fuse.log", "a")) == NULL) {
        perror("fopen() log file:");
        return(-1);
    }

    if ((__webhdfs_fuse.webhdfs = webhdfs_connect(config)) == NULL) {
        fclose(__webhdfs_fuse.flog);
        return(-2);
    }

    return(0);
}

static void webhdfs_fuse_disconnect (void) {
    webhdfs_disconnect(__webhdfs_fuse.webhdfs);
    fclose(__webhdfs_fuse.flog);
}

/* ============================================================================
 * Metadata related functions
 */
static int webhdfs_fuse_statfs (const char *path, struct statvfs *stat) {
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
    webhdfs_fstat_t *hdfs_stat;

    if ((hdfs_stat = webhdfs_stat(__WEBHDFS, path)) != NULL) {
        __hdfs_stat(hdfs_stat, stat);
        webhdfs_fstat_free(hdfs_stat);
        return(0);
    }

    return(-ENOENT);
}

static int webhdfs_fuse_chmod (const char *path, mode_t mode) {
    if (webhdfs_chmod(__WEBHDFS, path, mode))
        return(-ENOENT);
    return(0);
}

static int webhdfs_fuse_chown (const char *path, uid_t uid, gid_t gid) {
    char *group;
    char *user;
    int ret;

    if ((user = __hdfs_user_name(uid)) == NULL) {
        return(-EIO);
    }

    if ((group = __hdfs_group_name(gid)) == NULL) {
        free(user);
        return(-EIO);
    }

    ret = 0;
    if (webhdfs_chown(__WEBHDFS, path, user, group))
        ret = -EIO;

    free(group);
    free(user);
    return(ret);
}

/* ============================================================================
 * Namespace related functions
 */
static int webhdfs_fuse_create (const char *path,
                                mode_t mode,
                                struct fuse_file_info *ffi)
{
    webhdfs_file_t *file;

    if (webhdfs_file_create(__WEBHDFS, path, 0, NULL, NULL))
        return(-EIO);

    if ((file = webhdfs_file_open(__WEBHDFS, path)) == NULL)
        return(-EIO);

    ffi->fh = (uint64_t)file;
    webhdfs_chmod(__WEBHDFS, path, mode);

    return(0);
}

static int webhdfs_fuse_open (const char *path, struct fuse_file_info *ffi) {
    webhdfs_file_t *file;

    if ((file = webhdfs_file_open(__WEBHDFS, path)) == NULL)
        return(-EIO);

    ffi->fh = (uint64_t)file;

    return(0);
}

static int webhdfs_fuse_close (const char *path, struct fuse_file_info *ffi) {
    webhdfs_file_t *file = (webhdfs_file_t *)ffi->fh;
    webhdfs_file_close(file);
    return(0);
}

static int webhdfs_fuse_mkdir (const char *path, mode_t mode) {
    if (webhdfs_mkdir(__WEBHDFS, path, mode))
        return(-EIO);
    return(0);
}

static int webhdfs_fuse_rmdir (const char *path) {
    if (webhdfs_rmdir(__WEBHDFS, path, 1))
        return(-EIO);
    return(0);
}

static int webhdfs_fuse_unlink (const char *path) {
    if (webhdfs_unlink(__WEBHDFS, path))
        return(-EIO);
    return(0);
}

static int webhdfs_fuse_rename (const char *path, const char *newpath) {
    if (webhdfs_rename(__WEBHDFS, path, newpath))
        return(-EIO);
    return(0);
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
static int webhdfs_fuse_read (const char *path,
                              char *buffer,
                              size_t size,
                              off_t offset,
                              struct fuse_file_info *ffi)
{
    webhdfs_file_t *file = (webhdfs_file_t *)ffi->fh;
    size_t rd;

    if (!(rd = webhdfs_file_pread(file, buffer, size, offset)))
        return(-EIO);

    return(rd);
}

static int webhdfs_fuse_write (const char *path,
                               const char *buffer,
                               size_t size,
                               off_t offset,
                               struct fuse_file_info *ffi)
{
    webhdfs_file_t *file = (webhdfs_file_t *)ffi->fh;
    size_t wr;

    /* check is append only */
    if (!(wr = webhdfs_file_append_buffer(file, buffer, size)))
        return(-EIO);

    return(wr);
}

static int webhdfs_fuse_fsync (const char *path,
                               int data_sync,
                               struct fuse_file_info *ffi)
{
    return(0);
}

static int webhdfs_fuse_readdir (const char *path,
                                 void *buf,
                                 fuse_fill_dir_t filler,
                                 off_t offset,
                                 struct fuse_file_info *fi)
{
    const webhdfs_fstat_t *stat;
    webhdfs_dir_t *dir;

    if ((dir = webhdfs_dir_open(__WEBHDFS, path)) == NULL)
        return(-EIO);

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    while ((stat = webhdfs_dir_read(dir)) != NULL)
        filler(buf, stat->path, NULL, 0);

    webhdfs_dir_close(dir);
    return(0);
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
    .statfs         = webhdfs_fuse_statfs,
    .getattr        = webhdfs_fuse_getattr,
    .chmod          = webhdfs_fuse_chmod,
    .chown          = webhdfs_fuse_chown,

    /* Namespace */
    .create         = webhdfs_fuse_create,
    .open           = webhdfs_fuse_open,
    .release        = webhdfs_fuse_close,
    .mkdir          = webhdfs_fuse_mkdir,
    .rmdir          = webhdfs_fuse_rmdir,
    .unlink         = webhdfs_fuse_unlink,
    .rename         = webhdfs_fuse_rename,
    .link           = webhdfs_fuse_link,
    .symlink        = webhdfs_fuse_symlink,
    .readlink       = webhdfs_fuse_readlink,

    /* Object */
    .read           = webhdfs_fuse_read,
    .write          = webhdfs_fuse_write,
    .fsync          = webhdfs_fuse_fsync,
    .readdir        = webhdfs_fuse_readdir,
    .ioctl          = NULL,
    .lock           = NULL,
};

int main (int argc, char **argv) {
    webhdfs_conf_t *conf;
    int res;

    if (signal(SIGSEGV, __signal_sigsegv) == SIG_ERR) {
        fprintf(stderr, "Failed to initialize signals\n");
        return(EXIT_FAILURE);
    }

    if ((conf = webhdfs_conf_load("server.conf")) == NULL)
        return(EXIT_FAILURE);

    if (webhdfs_fuse_connect(conf) < 0)
        return(EXIT_FAILURE);

    res = fuse_main(argc, argv, &webhdfs_fuse_ops, NULL);

    webhdfs_fuse_disconnect();
    webhdfs_conf_free(conf);

    return(res);
}

