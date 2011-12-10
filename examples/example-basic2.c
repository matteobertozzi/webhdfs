#include <stdio.h>

#include <webhdfs/webhdfs.h>

static void __read_dir (webhdfs_t *fs, const char *path) {
    const webhdfs_fstat_t *stat;
    webhdfs_dir_t *dir;

    dir = webhdfs_dir_open(fs, path);

    while ((stat = webhdfs_dir_read(dir)) != NULL) {
        printf("%s\n", stat->path);
        printf(" - group:  %s\n", stat->group);
        printf(" - owner:  %s\n", stat->owner);
        printf(" - type:   %s\n", stat->type);
        printf(" - length: %lu\n", stat->length);
        printf(" - block:  %lu\n", stat->block);
        printf(" - atime:  %lu\n", stat->atime);
        printf(" - mtime:  %lu\n", stat->mtime);
        printf(" - replication: %d\n", stat->replication);
        printf(" - permission:  %o\n", stat->permission);
    }

    webhdfs_dir_close(dir);
}

static void __read_file (webhdfs_t *fs, const char *path) {
    webhdfs_file_t *file;
    char buffer[32];
    size_t n;

    file = webhdfs_file_open(fs, path);

    n = webhdfs_file_read(file, buffer, sizeof(buffer), 10);
    buffer[n] = '\0';
    printf("READ 0: %s (%lu)\n", buffer, n);

    n = webhdfs_file_read(file, buffer, sizeof(buffer), 0);
    printf("READ 1: %s (%lu)\n", buffer, n);

    webhdfs_file_close(file);
}

int main (int argc, char **argv) {
    webhdfs_conf_t *conf;
    webhdfs_t *fs;

    /* Setup webhdfs config */
    conf = webhdfs_conf_load("examples/server.conf");

    /* Connect to WebHDFS */
    fs = webhdfs_connect(conf);

    __read_dir(fs, "/");
    __read_file(fs, "/test");

    /* Disconnect from WebHDFS */
    webhdfs_disconnect(fs);
    webhdfs_conf_free(conf);

    return(0);
}

