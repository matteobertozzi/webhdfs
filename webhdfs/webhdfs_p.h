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

#include <stdint.h>

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
    buffer_t buffer;
};

enum webhdfs_req_type {
    WEBHDFS_REQ_GET,
    WEBHDFS_REQ_PUT,
    WEBHDFS_REQ_POST,
    WEBHDFS_REQ_DELETE,
};

struct webhdfs_file {
};

int     webhdfs_req_open       (webhdfs_req_t *req,
                                webhdfs_t *fs,
                                const char *path);
void    webhdfs_req_free       (webhdfs_req_t *req);

int     webhdfs_req_set_args   (webhdfs_req_t *req,
                                const char *frmt,
                                ...);
int     webhdfs_req_exec       (webhdfs_req_t *req,
                                int type);

#endif /* !_WEBHDFS_PRIVATE_H_ */

