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
#include <stdio.h>

#include <yajl/yajl_tree.h>
#include <curl/curl.h>

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

int webhdfs_mkdir (webhdfs_t *fs, const char *path, int permission) {
    webhdfs_req_t req;

    webhdfs_req_open(&req, fs, path);
    webhdfs_req_set_args(&req, "op=MKDIRS&permission=%o", permission);
    webhdfs_req_exec(&req, WEBHDFS_REQ_PUT);
    printf("TODO: mkdirs: %s\n", req.buffer.blob);
    webhdfs_req_close(&req);

    return(0);
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


