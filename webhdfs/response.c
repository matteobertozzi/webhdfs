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

#include <yajl/yajl_tree.h>

yajl_val webhdfs_response_exception (yajl_val node) {
    const char *node_path[] = {"RemoteException", NULL};
    return(yajl_tree_get(node, node_path, yajl_t_any));
}

yajl_val webhdfs_response_boolean (yajl_val node) {
    const char *node_path[] = {"boolean", NULL};
    return(yajl_tree_get(node, node_path, yajl_t_any));
}

yajl_val webhdfs_response_content_summary (yajl_val node) {
    const char *node_path[] = {"ContentSummary", NULL};
    return(yajl_tree_get(node, node_path, yajl_t_any));
}

yajl_val webhdfs_response_file_checksum (yajl_val node) {
    const char *node_path[] = {"FileChecksum", NULL};
    return(yajl_tree_get(node, node_path, yajl_t_any));
}

yajl_val webhdfs_response_file_status (yajl_val node) {
    const char *node_path[] = {"FileStatus", NULL};
    return(yajl_tree_get(node, node_path, yajl_t_any));
}

yajl_val webhdfs_response_file_statuses (yajl_val node) {
    const char *node_path[] = {"FileStatuses", NULL};
    return(yajl_tree_get(node, node_path, yajl_t_any));
}

yajl_val webhdfs_response_token (yajl_val node) {
    const char *node_path[] = {"Path", NULL};
    return(yajl_tree_get(node, node_path, yajl_t_any));
}

yajl_val webhdfs_response_path (yajl_val node) {
    const char *node_path[] = {"Path", NULL};
    return(yajl_tree_get(node, node_path, yajl_t_any));
}

yajl_val webhdfs_response_long (yajl_val node) {
    const char *node_path[] = {"long", NULL};
    return(yajl_tree_get(node, node_path, yajl_t_any));
}

