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

#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct webhdfs webhdfs_t;
typedef struct webhdfs_dir webhdfs_dir_t;
typedef struct webhdfs_conf webhdfs_conf_t;
typedef struct webhdfs_file webhdfs_file_t;

typedef size_t (*webhdfs_upload_t)  (void *ptr,
                                     size_t size,
                                     void *user_data);

typedef struct webhdfs_fstat {
    char *group;
    char *owner;
    char *path;
    char *type;
    size_t length;
    size_t block;
    size_t atime;
    size_t mtime;
    int replication;
    int permission;
} webhdfs_fstat_t;

/* WebHDFS Configuration - host:port, user, token, ... */
webhdfs_conf_t *        webhdfs_conf_alloc        (void);
webhdfs_conf_t *        webhdfs_conf_load         (const char *filename);
void                    webhdfs_conf_free         (webhdfs_conf_t *conf);

int                     webhdfs_conf_set_server   (webhdfs_conf_t *conf,
                                                   const char *host,
                                                   int port,
                                                   int use_ssl);
int                     webhdfs_conf_set_user     (webhdfs_conf_t *conf,
                                                   const char *user);
int                     webhdfs_conf_set_token    (webhdfs_conf_t *conf,
                                                   const char *token);

/* WebHDFS File-System */
webhdfs_t *             webhdfs_connect           (const webhdfs_conf_t *conf);
void                    webhdfs_disconnect        (webhdfs_t *fs);

int                     webhdfs_file_create       (webhdfs_t *fs,
                                                   const char *path,
                                                   int override,
                                                   webhdfs_upload_t upload_f,
                                                   void *upload_data);
webhdfs_file_t *        webhdfs_file_open         (webhdfs_t *fs,
                                                   const char *path);
int                     webhdfs_file_append       (webhdfs_file_t *file,
                                                   webhdfs_upload_t upload_f,
                                                   void *upload_data);
int                    webhdfs_file_append_buffer (webhdfs_file_t *file,
                                                   const void *buffer,
                                                   size_t nbytes);
size_t                 webhdfs_file_pread         (webhdfs_file_t *fs,
                                                   void *buffer,
                                                   size_t nbytes,
                                                   size_t offset);
size_t                 webhdfs_file_read          (webhdfs_file_t *fs,
                                                   void *buffer,
                                                   size_t nbyte);
int                    webhdfs_file_seek          (webhdfs_file_t *fs,
                                                   size_t offset);
void                   webhdfs_file_close         (webhdfs_file_t *file);

webhdfs_dir_t *        webhdfs_dir_open           (webhdfs_t *fs,
                                                   const char *path);
const webhdfs_fstat_t *webhdfs_dir_read           (webhdfs_dir_t *dir);
void                   webhdfs_dir_close          (webhdfs_dir_t *dir);

webhdfs_fstat_t *      webhdfs_stat               (webhdfs_t *fs,
                                                   const char *path);
void                   webhdfs_fstat_free         (webhdfs_fstat_t *fstat);

int                    webhdfs_mkdir              (webhdfs_t *fs,
                                                   const char *path,
                                                   int permission);
int                    webhdfs_rmdir              (webhdfs_t *fs,
                                                   const char *path,
                                                   int recursive);

int                    webhdfs_unlink             (webhdfs_t *fs,
                                                   const char *path);
int                    webhdfs_rename             (webhdfs_t *fs,
                                                   const char *oldname,
                                                   const char *newname);

int                    webhdfs_chown              (webhdfs_t *fs,
                                                   const char *path,
                                                   const char *user,
                                                   const char *group);
int                    webhdfs_chmod              (webhdfs_t *fs,
                                                   const char *path,
                                                   int permission);

int                    webhdfs_set_replication    (webhdfs_t *fs,
                                                   const char *path,
                                                   int replication);

int                    webhdfs_set_times          (webhdfs_t *fs,
                                                   const char *path,
                                                   int mtime,
                                                   int atime);

char *                 webhdfs_home_dir           (webhdfs_t *fs);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _WEBHDFS_H_ */

