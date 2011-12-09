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

static size_t __webhdfs_req_write (void *ptr,
                                   size_t size,
                                   size_t nitems,
                                   void *stream)
{
    webhdfs_req_t *req = (webhdfs_req_t *)stream;
    size_t n = size * nitems;

    if (buffer_append(&(req->buffer), ptr, n))
        return(0);

    return(n);
}

static size_t __webhdfs_req_read (void *ptr,
                                  size_t size,
                                  size_t nitems,
                                  void *stream)
{
    return(0);
}

int webhdfs_req_open (webhdfs_req_t *req,
                      webhdfs_t *fs,
                      const char *path)
{
    const webhdfs_conf_t *conf = fs->conf;
    int r;

    buffer_open(&(req->buffer));

    /* Fill URL */
    buffer_clear(&(req->buffer));
    r = buffer_append_format(&(req->buffer), "%s://%s:%d/%s?",
                             conf->use_ssl ? "https" : "http",
                             conf->host, conf->port,
                             (path != NULL) ? path + 1 : "");

    if (conf->user != NULL)
        r |= buffer_append_format(&(req->buffer), "user.name=%s&", conf->user);

    if (conf->token != NULL)
        r |= buffer_append_format(&(req->buffer), "delegation=%s&", conf->token);

    return(r);
}

void webhdfs_req_close (webhdfs_req_t *req) {
    buffer_close(&(req->buffer));
}

int webhdfs_req_set_args (webhdfs_req_t *req,
                          const char *frmt,
                          ...)
{
    va_list ap;
    int r;

    va_start(ap, frmt);
    buffer_append_vformat(&(req->buffer), frmt, ap);
    va_end(ap);

    return(r);
}

int webhdfs_req_exec (webhdfs_req_t *req, int type) {
    CURLcode err;
    CURL *curl;

    if ((curl = curl_easy_init()) == NULL)
        return(1);

    curl_easy_setopt(curl, CURLOPT_URL, req->buffer.blob);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __webhdfs_req_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, req);

    switch (type) {
      case WEBHDFS_REQ_GET:
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        break;
      case WEBHDFS_REQ_PUT:
        curl_easy_setopt(curl, CURLOPT_PUT, 1);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1);     /* TODO */
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, __webhdfs_req_read);
        curl_easy_setopt(curl, CURLOPT_READDATA, req);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, 0);
        break;
      case WEBHDFS_REQ_POST:
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        break;
      case WEBHDFS_REQ_DELETE:
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        break;
    }

    buffer_clear(&(req->buffer));
    if ((err = curl_easy_perform(curl)))
        fprintf(stderr, "%s\n", curl_easy_strerror(err));

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &(req->rcode));
    curl_easy_cleanup(curl);

    return(err != 0);
}

yajl_val webhdfs_req_json_response (webhdfs_req_t *req) {
    char err[1024];
    yajl_val node;

    if (req->buffer.size == 0)
        return(NULL);

    if ((node = yajl_tree_parse(req->buffer.blob, err, sizeof(err))) == NULL)
        fprintf(stderr, "response-parse: %s\n", err);

    return(node);
}


