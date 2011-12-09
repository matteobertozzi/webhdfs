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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include <yajl/yajl_tree.h>
#include <curl/curl.h>

#include "webhdfs_p.h"
#include "webhdfs.h"


webhdfs_conf_t *webhdfs_conf_alloc (void) {
    webhdfs_conf_t *conf;

    if ((conf = (webhdfs_conf_t *) malloc(sizeof(webhdfs_conf_t))) != NULL)
        memset(conf, 0, sizeof(webhdfs_conf_t));

    return(conf);
}

void webhdfs_conf_free (webhdfs_conf_t *conf) {
    if (conf->host != NULL)
        free(conf->host);

    if (conf->user != NULL)
        free(conf->user);

    if (conf->token != NULL)
        free(conf->token);

    free(conf);
}

static char *__load_file (const char *filename) {
    char *pblob;
    char *blob;
    off_t size;
    ssize_t rd;
    int fd;

    if ((fd = open(filename, O_RDONLY)) < 0) {
        perror("open()");
        return(NULL);
    }

    if (!(size = lseek(fd, 0U, SEEK_END))) {
        perror("lseek()");
        return(NULL);
    }

    if ((blob = (char *) malloc(size + 1)) == NULL) {
        perror("malloc()");
        close(fd);
        return(NULL);
    }

    pblob = blob;
    blob[size] = '\0';
    lseek(fd, 0U, SEEK_SET);
    while (size > 0) {
        if ((rd = read(fd, pblob, size)) <= 0) {
            free(blob);
            close(fd);
            return(NULL);
        }
        pblob += rd;
        size -= rd;
    }

    close(fd);
    return(blob);
}

webhdfs_conf_t *webhdfs_conf_load (const char *filename) {
    const char *jsonUseSsl[] = {"use-ssl", NULL};
    const char *jsonToken[] = {"token", NULL};
    const char *jsonDoAs[] = {"doas", NULL};
    const char *jsonUser[] = {"user", NULL};
    const char *jsonHost[] = {"host", NULL};
    const char *jsonPort[] = {"port", NULL};
    webhdfs_conf_t *conf;
    char buffer[1024];
    yajl_val node, v;
    void *cbuf;

    /* Load conf file */
    if ((cbuf = __load_file(filename)) == NULL)
        return(NULL);

    /* Parse json */
    if ((node = yajl_tree_parse(cbuf, buffer, sizeof(buffer))) == NULL) {
        fprintf(stderr, "conf-parse: %s\n", buffer);
        free(cbuf);
        return(NULL);
    }

    /* Unload conf file (we've json tree) */
    free(cbuf);

    if ((conf = webhdfs_conf_alloc()) == NULL) {
        yajl_tree_free(node);
        return(NULL);
    }

    if ((v = yajl_tree_get(node, jsonToken, yajl_t_string)) != NULL)
        conf->token = strdup(YAJL_GET_STRING(v));

    if ((v = yajl_tree_get(node, jsonHost, yajl_t_string)) != NULL)
        conf->host = strdup(YAJL_GET_STRING(v));

    if ((v = yajl_tree_get(node, jsonDoAs, yajl_t_string)) != NULL)
        conf->doas = strdup(YAJL_GET_STRING(v));

    if ((v = yajl_tree_get(node, jsonUser, yajl_t_string)) != NULL)
        conf->user = strdup(YAJL_GET_STRING(v));

    if ((v = yajl_tree_get(node, jsonUseSsl, yajl_t_number)) != NULL)
        conf->use_ssl = YAJL_IS_TRUE(v);

    if ((v = yajl_tree_get(node, jsonPort, yajl_t_number)) != NULL)
        conf->port = YAJL_GET_INTEGER(v);

    yajl_tree_free(node);
    return(conf);
}

int webhdfs_conf_set_server (webhdfs_conf_t *conf,
                             const char *host,
                             int port,
                             int use_ssl)
{
    if (conf->host != NULL)
        free(conf->host);

    if ((conf->host = strdup(host)) == NULL)
        return(1);

    conf->port = port;
    conf->use_ssl = use_ssl;

    return(0);
}

int webhdfs_conf_set_user (webhdfs_conf_t *conf,
                           const char *user)
{
    if (conf->user != NULL)
        free(conf->user);

    if ((conf->user = strdup(user)) == NULL)
        return(1);

    return(0);
}

int webhdfs_conf_set_token (webhdfs_conf_t *conf,
                            const char *token)
{
    if (conf->token != NULL)
        free(conf->token);

    if ((conf->token = strdup(token)) == NULL)
        return(1);

    return(0);
}

