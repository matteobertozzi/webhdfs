#!/usr/bin/env python
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import BaseHTTPServer

class WebHDFS(BaseHTTPServer.BaseHTTPRequestHandler):
    GET_OPS = ('OPEN', 'GETFILESTATUS', 'LISTSTATUS', 'GETCONTENTSUMMARY',
               'GETFILECHECKSUM', 'GETHOMEDIRECTORY', 'GETDELEGATIONTOKEN')
    PUT_OPS = ('CREATE', 'CREATE_DN', 'MKDIRS', 'RENAME', 'SETREPLICATION',
               'SETOWNER', 'SETPERMISSION', 'SETTIMES', 'RENEWDELEGATIONTOKEN',
               'CANCELDELEGATIONTOKEN')
    POST_OPS = ('APPEND')
    DELETE_OPS = ('DELETE')

    def do_GET(self):
        return self._dispatch(self.GET_OPS)

    def do_PUT(self):
        return self._dispatch(self.PUT_OPS)

    def do_POST(self):
        return self._dispatch(self.POST_OPS)

    def do_DELETE(self):
        return self._dispatch(self.DELETE_OPS)

    def op_CREATE(self, path, query):
        query['op'] = 'CREATE_DN'
        rest = '%s?%s' % (path, '&'.join('%s=%s' % x for x in query.items()))
        self.send_response(307)
        self.send_header("Location", 'http://localhost:50007/%s' % rest)
        self.send_header("Content-Length", 0)
        self.end_headers()

    def op_CREATE_DN(self, path, query):
        self.send_default_head(200)
        self.wfile.write('{CREATE DN %s}' % self.rfile.read(12))

    def op_MKDIRS(self, path, query):
        self.send_default_head(200)
        self.wfile.write('{"boolean": true}')

    def _dispatch(self, allowed_ops):
        path, query = self._getQuery()
        op = query.get('op', '').upper()

        func = None
        if op in allowed_ops:
            func_name = 'op_%s' % op
            if hasattr(self, func_name):
                func = getattr(self, func_name)

        if func is None:
            func = self._doUnsupportedOperationException

        return func(path, query)

    def _doUnsupportedOperationException(self, path, query):
        self.send_default_head(400)
        self.wfile.write("""{
  "RemoteException": {
    "exception": "UnsupportedOperationException",
    "javaClassName": "java.lang.UnsupportedOperationException",
    "message": "operation not supported..."
  }
}
""")


    def _getQuery(self):
        path = self.path
        i = path.find('?')
        if i < 0:
            return path, {}

        query = dict(query.split('=', 1) for query in path[i+1:].split('&'))
        return path[1:i], query

    def send_default_head(self, code):
        self.send_response(code)
        self.send_header("Content-type", 'application/json')
        #self.send_header("Transfer-Encoding", 'chunked')
        self.end_headers()

if __name__ == '__main__':
    srv = BaseHTTPServer.HTTPServer(('', 50007), WebHDFS)
    try:
        srv.serve_forever()
    except KeyboardInterrupt:
        print 'KeyboardInterrupt: Server is quitting...'
    finally:
        srv.shutdown()

