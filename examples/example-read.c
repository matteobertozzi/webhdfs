#include <stdio.h>

#include <webhdfs/webhdfs.h>

static void __read_file (webhdfs_t *fs, const char *path) {
    webhdfs_file_t *file;
    char buffer[80];
    size_t n;

    file = webhdfs_file_open(fs, path);

    while ((n = webhdfs_file_read(file, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        printf("READ: %s (%lu)\n", buffer, n);
    }

    webhdfs_file_close(file);
}

int main (int argc, char **argv) {
    webhdfs_conf_t *conf;
    webhdfs_t *fs;

    /* Setup webhdfs config */
    conf = webhdfs_conf_load("examples/server.conf");

    /* Connect to WebHDFS */
    fs = webhdfs_connect(conf);

    __read_file(fs, "/ftest.txt");

    /* Disconnect from WebHDFS */
    webhdfs_disconnect(fs);
    webhdfs_conf_free(conf);

    return(0);
}

