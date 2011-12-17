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

struct append_buffer {
    const char *buffer;
    size_t offset;
    size_t nbytes;
};

static size_t __append_buffer_upload (void *ptr, size_t length, void *data) {
    struct append_buffer *abuf = (struct append_buffer *)data;

    if (abuf->nbytes < length)
        length = abuf->nbytes;

    if (length > 0) {
        memcpy(ptr, abuf->buffer + abuf->offset, length);
        abuf->offset += length;
        abuf->nbytes -= length;
    }

    return(length);
}

int webhdfs_file_create (webhdfs_t *fs,
                         const char *path,
                         int override,
                         webhdfs_upload_t upload_func,
                         void *upload_data)
{
    webhdfs_req_t req;
    yajl_val node, v;

    webhdfs_req_open(&req, fs, path);
    webhdfs_req_set_args(&req, "op=CREATE&override=%s",
                               override ? "true" : "false");
    webhdfs_req_set_upload(&req, upload_func, upload_data);
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
    file->offset = 0U;
    if ((file->path = strdup(path)) == NULL) {
        free(file);
        return(NULL);
    }

    return(file);
}
int webhdfs_file_append (webhdfs_file_t *file,
                         webhdfs_upload_t upload_func,
                         void *upload_data)
{
    webhdfs_req_t req;
    yajl_val node, v;

    webhdfs_req_open(&req, file->fs, file->path);
    webhdfs_req_set_args(&req, "op=APPEND");
    webhdfs_req_set_upload(&req, upload_func, upload_data);
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

int webhdfs_file_append_buffer (webhdfs_file_t *file,
                                const void *buffer,
                                size_t nbytes)
{
    struct append_buffer data;
    data.buffer = (const char *)buffer;
    data.offset = 0;
    data.nbytes = nbytes;
    return(webhdfs_file_append(file, __append_buffer_upload, &data));
}

size_t webhdfs_file_pread (webhdfs_file_t *file,
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

size_t webhdfs_file_read (webhdfs_file_t *file,
                          void *buffer,
                          size_t nbyte)
{
    size_t rd;

    if ((rd = webhdfs_file_pread(file, buffer, nbyte, file->offset)) > 0)
        file->offset += rd;

    return(rd);
}

int webhdfs_file_seek (webhdfs_file_t *file, size_t offset) {
    /* TODO: Check file length? */
    file->offset = offset;
    return(0);
}

void webhdfs_file_close (webhdfs_file_t *file) {
    free(file->path);
    free(file);
}

