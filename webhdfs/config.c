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

