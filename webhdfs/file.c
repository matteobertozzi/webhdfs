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

int webhdfs_file_create (webhdfs_t *fs,
                         const char *path,
                         int override)
{
    webhdfs_req_t req;
    yajl_val node, v;

    webhdfs_req_open(&req, fs, path);
    webhdfs_req_set_args(&req, "op=CREATE&override=%s",
                               override ? "true" : "false");
    webhdfs_req_exec(&req, WEBHDFS_REQ_PUT);
    node = webhdfs_req_json_response(&req);
    webhdfs_req_close(&req);

    /* Exception */
    if ((v = webhdfs_response_exception(node)) != NULL) {
        yajl_tree_free(node);
        return(1);
    }

    yajl_tree_free(node);
    return(0);
}

webhdfs_file_t *webhdfs_file_open (webhdfs_t *fs,
                                   const char *path)
{
    webhdfs_file_t *file;
    yajl_val node, v;
    
    if ((file = (webhdfs_file_t *) malloc(sizeof(webhdfs_file_t))) == NULL)
        return(NULL);

    file->fs = fs;
    if ((file->path = strdup(path)) == NULL) {
        free(file);
        return(NULL);
    }

    return(file);
}

int webhdfs_file_append (webhdfs_file_t *file,
                         const void *buffer,
                         size_t nbyte)
{
    webhdfs_req_t req;
    yajl_val node, v;

    webhdfs_req_open(&req, file->fs, file->path);
    webhdfs_req_set_args(&req, "op=APPEND");
    webhdfs_req_exec(&req, WEBHDFS_REQ_POST);
    node = webhdfs_req_json_response(&req);
    webhdfs_req_close(&req);

    /* Exception */
    if ((v = webhdfs_response_exception(node)) != NULL) {
        yajl_tree_free(node);
        return(1);
    }

    yajl_tree_free(node);
    return(0);
}

size_t webhdfs_file_read (webhdfs_file_t *file,
                          void *buffer,
                          size_t nbyte,
                          size_t offset)
{
    webhdfs_req_t req;
    yajl_val node, v;
    size_t size = 0;

    webhdfs_req_open(&req, file->fs, file->path);
    webhdfs_req_set_args(&req, "op=OPEN&offset=%ld&length=%ld", offset, nbyte);
    webhdfs_req_exec(&req, WEBHDFS_REQ_GET);

    if (req.rcode == 200) {
        size = req.buffer.size;
        memcpy(buffer, req.buffer.blob, size);
    } else {
        /* Exception */
        yajl_val node;
        node = webhdfs_req_json_response(&req);
        if ((v = webhdfs_response_exception(node)) != NULL) {
            yajl_tree_free(node);
            return(0);
        }
        yajl_tree_free(node);
    }

    webhdfs_req_close(&req);
    return(size);
}

void webhdfs_file_close (webhdfs_file_t *file) {
    free(file->path);
    free(file);
}

