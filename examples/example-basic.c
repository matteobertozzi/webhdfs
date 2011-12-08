#include <webhdfs/webhdfs.h>

int main (int argc, char **argv) {
    webhdfs_conf_t *conf;
    webhdfs_t *fs;

    /* Setup webhdfs config */
    conf = webhdfs_conf_alloc();
    webhdfs_conf_set_server(conf, "localhost", 50007, 0);
    webhdfs_conf_set_user(conf, "th30z");

    /* Connect to WebHDFS */
    fs = webhdfs_connect(conf);

    webhdfs_bad(fs);
    webhdfs_mkdir(fs, "/test", 0644);

    /* Disconnect from WebHDFS */
    webhdfs_disconnect(fs);

    return(0);
}

