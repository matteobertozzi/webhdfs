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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <yajl/yajl_tree.h>

#include "webhdfs_p.h"
#include "webhdfs.h"

webhdfs_t *webhdfs_connect (const webhdfs_conf_t *conf) {
    webhdfs_t *fs;

    if ((fs = (webhdfs_t *) malloc(sizeof(webhdfs_t))) == NULL)
        return(NULL);

    fs->conf = conf;

    return(fs);
}

void webhdfs_disconnect (webhdfs_t *fs) {
    free(fs);
}

static int __webhdfs_delete (webhdfs_t *fs, const char *path, int recursive) {
    webhdfs_req_t req;
    yajl_val node, v;

    webhdfs_req_open(&req, fs, path);
    webhdfs_req_set_args(&req, "op=DELETE&recursive=%s",
                               recursive ? "true" : "false");
    webhdfs_req_exec(&req, WEBHDFS_REQ_DELETE);
    node = webhdfs_req_json_response(&req);
    webhdfs_req_close(&req);

    if ((v = webhdfs_response_exception(node)) != NULL) {
        yajl_tree_free(node);
        return(1);
    }

    if ((v = webhdfs_response_boolean(node)) != NULL) {
        int failure = YAJL_IS_FALSE(v);
        yajl_tree_free(node);
        return(failure);
    }

    yajl_tree_free(node);
    return(2);
}

int webhdfs_mkdir (webhdfs_t *fs, const char *path, int permission) {
    webhdfs_req_t req;
    yajl_val node, v;

    webhdfs_req_open(&req, fs, path);
    webhdfs_req_set_args(&req, "op=MKDIRS&permission=%o", permission);
    webhdfs_req_exec(&req, WEBHDFS_REQ_PUT);
    node = webhdfs_req_json_response(&req);
    webhdfs_req_close(&req);

    if ((v = webhdfs_response_exception(node)) != NULL) {
        yajl_tree_free(node);
        return(1);
    }

    if ((v = webhdfs_response_boolean(node)) != NULL) {
        int failure = YAJL_IS_FALSE(v);
        yajl_tree_free(node);
        return(failure);
    }

    yajl_tree_free(node);
    return(2);
}

int webhdfs_rmdir (webhdfs_t *fs, const char *path, int recursive) {
    return(__webhdfs_delete(fs, path, recursive));
}

int webhdfs_unlink (webhdfs_t *fs, const char *path) {
    return(__webhdfs_delete(fs, path, 0));
}

int webhdfs_rename (webhdfs_t *fs, const char *oldname, const char *newname) {
    webhdfs_req_t req;
    yajl_val node, v;

    webhdfs_req_open(&req, fs, oldname);
    webhdfs_req_set_args(&req, "op=RENAME&destination=%s", newname);
    webhdfs_req_exec(&req, WEBHDFS_REQ_PUT);
    node = webhdfs_req_json_response(&req);
    webhdfs_req_close(&req);

    if ((v = webhdfs_response_exception(node)) != NULL) {
        yajl_tree_free(node);
        return(1);
    }

    if ((v = webhdfs_response_boolean(node)) != NULL) {
        int failure = YAJL_IS_FALSE(v);
        yajl_tree_free(node);
        return(failure);
    }

    yajl_tree_free(node);
    return(2);
}

int webhdfs_chown (webhdfs_t *fs,
                   const char *path,
                   const char *user,
                   const char *group)
{
    webhdfs_req_t req;
    yajl_val node, v;

    webhdfs_req_open(&req, fs, path);
    webhdfs_req_set_args(&req, "op=SETOWNER&user=%s&group=%s", user, group);
    webhdfs_req_exec(&req, WEBHDFS_REQ_PUT);
    node = webhdfs_req_json_response(&req);
    webhdfs_req_close(&req);

    if ((v = webhdfs_response_exception(node)) != NULL) {
        yajl_tree_free(node);
        return(1);
    }

    yajl_tree_free(node);
    return(0);
}

int webhdfs_chmod (webhdfs_t *fs, const char *path, int permission) {
    webhdfs_req_t req;
    yajl_val node, v;

    webhdfs_req_open(&req, fs, path);
    webhdfs_req_set_args(&req, "op=SETPERMISSION&permission=%o", permission);
    webhdfs_req_exec(&req, WEBHDFS_REQ_PUT);
    node = webhdfs_req_json_response(&req);
    webhdfs_req_close(&req);

    if ((v = webhdfs_response_exception(node)) != NULL) {
        yajl_tree_free(node);
        return(1);
    }

    yajl_tree_free(node);
    return(0);
}

int webhdfs_set_replication (webhdfs_t *fs, const char *path, int replication) {
    webhdfs_req_t req;
    yajl_val node, v;

    webhdfs_req_open(&req, fs, path);
    webhdfs_req_set_args(&req, "op=SETREPLICATION&replication=%d", replication);
    webhdfs_req_exec(&req, WEBHDFS_REQ_PUT);
    node = webhdfs_req_json_response(&req);
    webhdfs_req_close(&req);

    if ((v = webhdfs_response_exception(node)) != NULL) {
        yajl_tree_free(node);
        return(1);
    }

    if ((v = webhdfs_response_boolean(node)) != NULL) {
        int failure = YAJL_IS_FALSE(v);
        yajl_tree_free(node);
        return(failure);
    }

    yajl_tree_free(node);
    return(2);
}

int webhdfs_set_times (webhdfs_t *fs, const char *path, int mtime, int atime) {
    webhdfs_req_t req;
    yajl_val node, v;

    webhdfs_req_open(&req, fs, path);
    webhdfs_req_set_args(&req, "op=SETTIMES&modificationtime=%d&accesstime=%d",
                               mtime, atime);
    webhdfs_req_exec(&req, WEBHDFS_REQ_PUT);
    node = webhdfs_req_json_response(&req);
    webhdfs_req_close(&req);

    if ((v = webhdfs_response_exception(node)) != NULL) {
        yajl_tree_free(node);
        return(1);
    }

    yajl_tree_free(node);
    return(0);
}

char *webhdfs_home_dir (webhdfs_t *fs) {
    webhdfs_req_t req;
    yajl_val node, v;

    webhdfs_req_open(&req, fs, NULL);
    webhdfs_req_set_args(&req, "op=GETHOMEDIRECTORY");
    webhdfs_req_exec(&req, WEBHDFS_REQ_GET);
    node = webhdfs_req_json_response(&req);
    webhdfs_req_close(&req);

    if ((v = webhdfs_response_exception(node)) != NULL) {
        yajl_tree_free(node);
        return(NULL);
    }

    if ((v = webhdfs_response_path(node)) != NULL) {
        char *path = strdup(YAJL_GET_STRING(v));
        yajl_tree_free(node);
        return(path);
    }

    yajl_tree_free(node);
    return(NULL);
}

int webhdfs_bad (webhdfs_t *fs) {
    webhdfs_req_t req;

    webhdfs_req_open(&req, fs, NULL);
    webhdfs_req_set_args(&req, "op=BAD");
    webhdfs_req_exec(&req, WEBHDFS_REQ_GET);
    printf("TODO: bad: %s\n", req.buffer.blob);
    webhdfs_req_close(&req);

    return(0);
}


