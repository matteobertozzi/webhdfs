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

#ifndef _WEBHDFS_PRIVATE_H_
#define _WEBHDFS_PRIVATE_H_

#include <stdarg.h>

#include <yajl/yajl_tree.h>

#include "webhdfs.h"
#include "buffer.h"

typedef struct webhdfs_req webhdfs_req_t;

struct webhdfs {
    const webhdfs_conf_t *conf;
};

struct webhdfs_conf {
    char *token;            /* delegation token */
    char *doas;             /* proxy user */
    char *user;             /* hdfs user */
    char *host;
    int   use_ssl;
    int   port;
};

struct webhdfs_req {
    webhdfs_upload_t upload;    /* Upload function used by put */
    void *   upload_data;       /* Upload user data */
    buffer_t buffer;            /* Internal buffer used for url & data */
    int      rcode;             /* Response code */
};

enum webhdfs_req_type {
    WEBHDFS_REQ_GET,
    WEBHDFS_REQ_PUT,
    WEBHDFS_REQ_POST,
    WEBHDFS_REQ_DELETE,
};

struct webhdfs_dir {
    webhdfs_fstat_t stat;
    yajl_val statuses;
    yajl_val root;
    size_t   current;
};

struct webhdfs_file {
    webhdfs_t *fs;
    char *     path;
    size_t     offset;
};

int      webhdfs_req_open                 (webhdfs_req_t *req,
                                           webhdfs_t *fs,
                                           const char *path);
void     webhdfs_req_free                 (webhdfs_req_t *req);

int      webhdfs_req_set_args             (webhdfs_req_t *req,
                                           const char *frmt,
                                           ...);
int      webhdfs_req_set_upload           (webhdfs_req_t *req,
                                           webhdfs_upload_t func,
                                           void *user_data);
int      webhdfs_req_exec                 (webhdfs_req_t *req,
                                           int type);

yajl_val webhdfs_req_json_response        (webhdfs_req_t *req);

yajl_val webhdfs_response_exception       (yajl_val node);
yajl_val webhdfs_response_boolean         (yajl_val node);
yajl_val webhdfs_response_content_summary (yajl_val node);
yajl_val webhdfs_response_file_checksum   (yajl_val node);
yajl_val webhdfs_response_file_status     (yajl_val node);
yajl_val webhdfs_response_file_statuses   (yajl_val node);
yajl_val webhdfs_response_token           (yajl_val node);
yajl_val webhdfs_response_path            (yajl_val node);
yajl_val webhdfs_response_long            (yajl_val node);


#endif /* !_WEBHDFS_PRIVATE_H_ */

