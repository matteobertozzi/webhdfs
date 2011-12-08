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

#ifndef _WEBHDFS_H_
#define _WEBHDFS_H_

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct webhdfs webhdfs_t;
typedef struct webhdfs_conf webhdfs_conf_t;
typedef struct webhdfs_file webhdfs_file_t;

/* WebHDFS Configuration - host:port, user, token, ... */
webhdfs_conf_t *webhdfs_conf_alloc      (void);
void            webhdfs_conf_free       (webhdfs_conf_t *conf);

int             webhdfs_conf_set_server (webhdfs_conf_t *conf,
                                         const char *host,
                                         int port,
                                         int use_ssl);
int             webhdfs_conf_set_user   (webhdfs_conf_t *conf,
                                         const char *user);
int             webhdfs_conf_set_token  (webhdfs_conf_t *conf,
                                         const char *token);

/* WebHDFS File-System */
webhdfs_t *     webhdfs_connect         (const webhdfs_conf_t *conf);
void            webhdfs_disconnect      (webhdfs_t *fs);

int             webhdfs_mkdir           (webhdfs_t *fs,
                                         const char *path,
                                         int permission);
int             webhdfs_bad             (webhdfs_t *fs);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _WEBHDFS_H_ */

