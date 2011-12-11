#include <string.h>
#include <stdio.h>

#include <webhdfs/webhdfs.h>

struct data {
    size_t nbytes;
};

static size_t __upload_func (void *ptr, size_t size, void *data) {
    struct data *p = (struct data *)data;

    if (p->nbytes < size)
        size = p->nbytes;

    if (size == 11) {
        printf("UPLOAD FUNC %lu\n", size);
        memcpy(ptr, "Hello HDFS\n", 11);
        p->nbytes -= size;
        return(11);
    }

    return(0);
}

static void __write_file (webhdfs_t *fs, const char *path) {
    struct data data;

    data.nbytes = 11;
    if (webhdfs_file_create(fs, path, 0, __upload_func, &data))
        printf("fail create file\n");
}

static void __append_file (webhdfs_t *fs, const char *path) {
    webhdfs_file_t *file;

    file = webhdfs_file_open(fs, path);
    webhdfs_file_append_buffer(file, "hello append\n", 13);
    webhdfs_file_append_buffer(file, "this is line 2\n", 15);
    webhdfs_file_close(file);
}

int main (int argc, char **argv) {
    webhdfs_conf_t *conf;
    webhdfs_t *fs;

    /* Setup webhdfs config */
    conf = webhdfs_conf_load("examples/server.conf");

    /* Connect to WebHDFS */
    fs = webhdfs_connect(conf);

    __write_file(fs, "/ftest3.txt");
    __append_file(fs, "/ftest3.txt");

    /* Disconnect from WebHDFS */
    webhdfs_disconnect(fs);
    webhdfs_conf_free(conf);

    return(0);
}

