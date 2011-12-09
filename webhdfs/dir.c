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

webhdfs_dir_t *webhdfs_dir_open (webhdfs_t *fs,
                                 const char *path)
{
    const char *file_status[] = {"FileStatus", NULL};
    webhdfs_dir_t *dir;
    webhdfs_req_t req;
    yajl_val node, v;

    webhdfs_req_open(&req, fs, path);
    webhdfs_req_set_args(&req, "op=LISTSTATUS");
    webhdfs_req_exec(&req, WEBHDFS_REQ_GET);
    node = webhdfs_req_json_response(&req);
    webhdfs_req_close(&req);

    if ((v = webhdfs_response_exception(node)) != NULL) {
        yajl_tree_free(node);
        return(NULL);
    }

    if ((v = webhdfs_response_file_statuses(node)) == NULL) {
        yajl_tree_free(node);
        return(NULL);
    }

    if ((v = yajl_tree_get(v, file_status, yajl_t_array)) == NULL) {
        yajl_tree_free(node);
        return(NULL);
    }

    if ((dir = (webhdfs_dir_t *) malloc(sizeof(webhdfs_dir_t))) == NULL) {
        yajl_tree_free(node);
        return(NULL);
    }

    dir->root = node;
    dir->statuses = v;
    dir->current = 0;

    return(dir);
}

const webhdfs_fstat_t *webhdfs_dir_read (webhdfs_dir_t *dir) {
    const char *path[] = {"pathSuffix", NULL};
    const char *replication[] = {"replication", NULL};
    const char *permission[] = {"permission", NULL};
    const char *length[] = {"length", NULL};
    const char *group[] = {"group", NULL};
    const char *owner[] = {"owner", NULL};
    const char *type[] = {"type", NULL};
    const char *mtime[] = {"modificationTime", NULL};
    const char *block[] = {"block", NULL};
    const char *atime[] = {"accessTime", NULL};
    webhdfs_fstat_t stat;
    yajl_val node, v;

    if (dir->current >= YAJL_GET_ARRAY(dir->statuses)->len)
        return(NULL);

    node = YAJL_GET_ARRAY(dir->statuses)->values[dir->current];
    dir->current++;

    if ((v = yajl_tree_get(node, atime, yajl_t_number)))
        dir->stat.atime = YAJL_GET_INTEGER(v);

    if ((v = yajl_tree_get(node, atime, yajl_t_number)))
        dir->stat.atime = YAJL_GET_INTEGER(v);

    if ((v = yajl_tree_get(node, length, yajl_t_number)))
        dir->stat.length = YAJL_GET_INTEGER(v);

    if ((v = yajl_tree_get(node, block, yajl_t_number)))
        dir->stat.block = YAJL_GET_INTEGER(v);

    if ((v = yajl_tree_get(node, replication, yajl_t_number)))
        dir->stat.replication = YAJL_GET_INTEGER(v);

    if ((v = yajl_tree_get(node, permission, yajl_t_number)))
        dir->stat.permission = strtol(YAJL_GET_STRING(v), NULL, 8);

    if ((v = yajl_tree_get(node, path, yajl_t_string)))
        dir->stat.path = YAJL_GET_STRING(v);

    if ((v = yajl_tree_get(node, group, yajl_t_string)))
        dir->stat.group = YAJL_GET_STRING(v);

    if ((v = yajl_tree_get(node, owner, yajl_t_string)))
        dir->stat.owner = YAJL_GET_STRING(v);

    if ((v = yajl_tree_get(node, type, yajl_t_string)))
        dir->stat.type = YAJL_GET_STRING(v);

    return(&(dir->stat));
}

void webhdfs_dir_close (webhdfs_dir_t *dir) {
    if (dir->root != NULL)
        yajl_tree_free(dir->root);
    free(dir);
}

