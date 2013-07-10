libalgol - a collection of plug-ins for developing back-end C++ web tools.

## Components

**Algol Admin**

*missing*

**Messaging & analytics (using RabbitMQ and BSON)**

*missing*

**HTTP using boost::ASIO**

The HTTP module abstracts away several aspects of getting an HTTP server up using boost::asio and lets you focus on handling the requests and dispatching replies:

* connection accepting, pooling, and time-outs
* the ability to gracefully restart, pause, and resume the kernel
* parsing of request headers and body
* accepting requests with no Content-Length specified (a streaming parser)

**Logging with log4cpp**

*missing*

**Lua and Python bindings (via SWIG)**

`libalgol` can be used natively in Lua and Python scripts with the following components exposed:

* HTTP (parse requests and dispatch responses)
* messaging (receive, consume, reply, broadcast)
* logging
* analytics tracking

**Configurables via CLI and JSON (using yajl)**

*missing*

**File Manager (cURL and binreloc)**

*missing*

**Dynamic .so plugin system**

*missing*

**Utility**

* a wrapper for regex engines (PCREpp)
* a timing facility for benchmarking routines while debugging and developing

## License

libalgol - a collection of plug-ins for developing back-end C++ web tools
Copyright (c) 2013 Algol Labs, LLC.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.