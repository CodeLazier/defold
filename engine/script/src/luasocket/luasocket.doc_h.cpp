/*# LuaSocket API documentation
 *
 * [LuaSocket](https://github.com/diegonehab/luasocket) is a Lua extension library that provides
 * support for the TCP and UDP transport layers. Defold provides the "socket" namespace in
 * runtime, which contain the core C functionality. Additional LuaSocket support modules for
 * SMTP, HTTP, FTP etc are not part of the core included, but can be easily added to a project
 * and used.
 *
 * Note the included helper module "socket.lua" in "builtins/scripts/socket.lua". Require this
 * module to add some additional functions and shortcuts to the namespace:
 *
 * ```lua
 * require "builtins.scripts.socket"
 * ```
 *
 * LuaSocket is Copyright &copy; 2004-2007 Diego Nehab. All rights reserved.
 * LuaSocket is free software, released under the MIT license (same license as the Lua core).
 *
 * @document
 * @name LuaSocket
 * @namespace socket
 */


/*# waits for a number of sockets to change status
 *
 * The function returns a list with the sockets ready for reading, a list with the sockets ready for writing and an error message. The error message is "timeout" if a timeout condition was met and nil otherwise. The returned tables are doubly keyed both by integers and also by the sockets themselves, to simplify the test if a specific socket has changed status.
 *
 * `Recvt` and `sendt` parameters can be empty tables or `nil`. Non-socket values (or values with non-numeric indices) in these arrays will be silently ignored.
 *
 * The returned tables are doubly keyed both by integers and also by the sockets themselves, to simplify the test if a specific socket has changed status.
 *
 * [icon:attention] This function can monitor a limited number of sockets, as defined by the constant socket._SETSIZE. This number may be as high as 1024 or as low as 64 by default, depending on the system. It is usually possible to change this at compile time. Invoking select with a larger number of sockets will raise an error.
 *
 * [icon:attention] A known bug in WinSock causes select to fail on non-blocking TCP sockets. The function may return a socket as writable even though the socket is not ready for sending.
 *
 * [icon:attention] Calling select with a server socket in the receive parameter before a call to accept does not guarantee accept will return immediately. Use the settimeout method or accept might block forever.
 *
 * [icon:attention] If you close a socket and pass it to select, it will be ignored.
 *
 * (Using select with non-socket objects: Any object that implements `getfd` and `dirty` can be used with select, allowing objects from other libraries to be used within a socket.select driven loop.)
 *
 * @name socket.select
 * @param recvt [type:table] array with the sockets to test for characters available for reading.
 * @param sendt [type:table] array with sockets that are watched to see if it is OK to immediately write on them.
 * @param [timeout] [type:number] the maximum amount of time (in seconds) to wait for a change in status. Nil, negative or omitted timeout value allows the function to block indefinitely.
 *
 * @return sockets_r [type:table] a list with the sockets ready for reading.
 * @return sockets_w [type:table] a list with the sockets ready for writing.
 * @return error [type:string] an error message. "timeout" if a timeout condition was met, otherwise `nil`.
 */


/*# the current LuaSocket version
 *
 * This constant has a string describing the current LuaSocket version.
 *
 * @variable
 * @name socket._VERSION
 *
 */

/*# creates a new IPv6 TCP master object
 *
 * Creates and returns an IPv6 TCP master object. A master object can be transformed into a server object with the method `listen` (after a call to `bind`) or into a client object with the method connect. The only other method supported by a master object is the close method.
 *
 * Note: The TCP object returned will have the option "ipv6-v6only" set to true.
 *
 * @name socket.tcp6
 * @return tcp_master [type:master] a new IPv6 TCP master object, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# max numbers of sockets the select function can handle
 *
 * This constant contains the maximum number of sockets that the select function can handle.
 *
 * @variable
 * @name socket._SETSIZE
 *
 */

/*# creates a new IPv6 UDP object
 *
 * Creates and returns an unconnected IPv6 UDP object. Unconnected objects support the `sendto`, `receive`, `receivefrom`, `getoption`, `getsockname`, `setoption`, `settimeout`, `setpeername`, `setsockname`, and `close` methods. The `setpeername` method is used to connect the object.
 *
 * Note: The UDP object returned will have the option "ipv6-v6only" set to true.
 *
 * @name socket.udp6
 * @return udp_unconnected [type:unconnected] a new unconnected IPv6 UDP object, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# resolve to host name (IPv4)
 *
 * This function converts from an IPv4 address to host name.
 * The address can be an IPv4 address or a host name.
 *
 * @name socket.dns.tohostname
 * @param address [type:string] an IPv4 address or host name.
 * @return hostname [type:string] the canonic host name of the given address, or `nil` in case of an error.
 * @return resolved [type:table|string] a table with all information returned by the resolver, or if an error occurs, the error message string.
 */

/*# resolve to IPv4 address
 *
 * This function converts a host name to IPv4 address.
 * The address can be an IP address or a host name.
 *
 * @name socket.dns.toip
 * @param address [type:string] a hostname or an IP address.
 * @return ip_address [type:string] the first IP address found for the hostname, or `nil` in case of an error.
 * @return resolved [type:table|string] a table with all information returned by the resolver, or if an error occurs, the error message string.
 *
 */

/*# gets the machine host name
 *
 * Returns the standard host name for the machine as a string.
 *
 * @name socket.dns.gethostname
 * @return hostname [type:string] the host name for the machine.
 *
 */

/*# resolve to IPv4 or IPv6 address
 *
 * This function converts a host name to IPv4 or IPv6 address.
 * The supplied address can be an IPv4 or IPv6 address or host name.
 *
 * The function returns a table with all information returned by the resolver:
 *
 * ```lua
 * {
 *  [1] = {
 *     family = family-name-1,
 *     addr = address-1
 *   },
 *   ...
 *   [n] = {
 *     family = family-name-n,
 *     addr = address-n
 *   }
 * }
 * ```
 *
 * Here, family contains the string `"inet"` for IPv4 addresses, and `"inet6"` for IPv6 addresses.
 * In case of error, the function returns nil followed by an error message.
 *
 * @name socket.dns.getaddrinfo
 * @param address [type:string] a hostname or an IPv4 or IPv6 address.
 * @return resolved [type:table] a table with all information returned by the resolver, or if an error occurs, `nil`.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# resolve to hostname (IPv4 or IPv6)
 *
 * This function converts an address to host name.
 * The supplied address can be an IPv4 or IPv6 address or host name.
 *
 * The function returns a table with all information returned by the resolver:
 *
 * ```lua
 * {
 *   [1] = host-name-1,
 *   ...
 *   [n] = host-name-n,
 * }
 * ```
 *
 * @name socket.dns.getnameinfo
 * @param address [type:string] a hostname or an IPv4 or IPv6 address.
 * @return resolved [type:table] a table with all information returned by the resolver, or if an error occurs, `nil`.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# creates a new connected TCP client object
 *
 * This function is a shortcut that creates and returns a TCP client object connected to a remote
 * address at a given port. Optionally, the user can also specify the local address and port to
 * bind (`locaddr` and `locport`), or restrict the socket family to `"inet"` or `"inet6"`.
 * Without specifying family to connect, whether a tcp or tcp6 connection is created depends on
 * your system configuration.
 *
 * @name socket.connect
 * @param address [type:string] the address to connect to.
 * @param port [type:number] the port to connect to.
 * @param [locaddr] [type:string] optional local address to bind to.
 * @param [locport] [type:number] optional local port to bind to.
 * @param [family] [type:string] optional socket family to use, `"inet"` or `"inet6"`.
 * @return tcp_client [type:client] a new IPv6 TCP client object, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# creates a new IPv4 UDP object
 *
 * Creates and returns an unconnected IPv4 UDP object. Unconnected objects support the `sendto`, `receive`, `receivefrom`, `getoption`, `getsockname`, `setoption`, `settimeout`, `setpeername`, `setsockname`, and `close` methods. The `setpeername` method is used to connect the object.
 *
 * @name socket.udp
 * @return upd_unconnected [type:unconnected] a new unconnected IPv4 UDP object, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# drops a number of arguments and returns the remaining
 *
 * This function drops a number of arguments and returns the remaining.
 * It is useful to avoid creation of dummy variables:
 * `D` is the number of arguments to drop. <code>Ret<sub>1</sub></code> to <code>ret<sub>N</sub></code> are the arguments.
 * The function returns <code>ret<sub>D+1</sub></code> to <code>ret<sub>N</sub></code>.
 *
 * @name socket.skip
 * @param d [type:number] the number of arguments to drop.
 * @param [ret1] [type:any] argument 1.
 * @param [ret2] [type:any] argument 2.
 * @param [retN] [type:any] argument N.
 * @return [retD+1] [type:any] argument D+1.
 * @return [retD+2] [type:any] argument D+2.
 * @return [retN] [type:any] argument N.
 * @examples
 *
 * Instead of doing the following with dummy variables:
 *
 * ```lua
 * -- get the status code and separator from SMTP server reply
 * local dummy1, dummy2, code, sep = string.find(line, "^(%d%d%d)(.?)")
 * ```
 *
 * You can skip a number of variables:
 *
 * ```lua
 * -- get the status code and separator from SMTP server reply
 * local code, sep = socket.skip(2, string.find(line, "^(%d%d%d)(.?)"))
 * ```
 */

/*#
 * Creates and returns an IPv4 TCP master object. A master object can be transformed into a server object with the method `listen` (after a call to `bind`) or into a client object with the method `connect`. The only other method supported by a master object is the `close` method.
 *
 * @name socket.tcp
 * @return tcp_master [type:master] a new IPv4 TCP master object, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# creates a new try function
 *
 * This function creates and returns a clean try function that allows for cleanup before the exception is raised.
 * The `finalizer` function will be called in protected mode (see [ref:protect]).
 *
 * @name socket.newtry
 * @param finalizer [type:function()] a function that will be called before the try throws the exception.
 * @return try [type:function] the customized try function.
 * @examples
 *
 * Perform operations on an open socket `c`:
 *
 * ```lua
 * -- create a try function that closes 'c' on error
 * local try = socket.newtry(function() c:close() end)
 * -- do everything reassured c will be closed
 * try(c:send("hello there?\r\n"))
 * local answer = try(c:receive())
 * ...
 * try(c:send("good bye\r\n"))
 * c:close()
 * ```
 */

/*# converts a function that throws exceptions into a safe function
 *
 * Converts a function that throws exceptions into a safe function. This function only catches exceptions thrown by try functions. It does not catch normal Lua errors.
 *
 * [icon:attention] Beware that if your function performs some illegal operation that raises an error, the protected function will catch the error and return it as a string. This is because try functions uses errors as the mechanism to throw exceptions.
 *
 * @name socket.protect
 * @param func [type:function] a function that calls a try function (or assert, or error) to throw exceptions.
 * @return safe_func [type:function(function())] an equivalent function that instead of throwing exceptions, returns `nil` followed by an error message.
 * @examples
 *
 * ```lua
 * local dostuff = socket.protect(function()
 *     local try = socket.newtry()
 *     local c = try(socket.connect("myserver.com", 80))
 *     try = socket.newtry(function() c:close() end)
 *     try(c:send("hello?\r\n"))
 *     local answer = try(c:receive())
 *     c:close()
 * end)
 *
 * local n, error = dostuff()
 * ```
 */

/*# gets seconds since system epoch
 *
 * Returns the time in seconds, relative to the system epoch (Unix epoch time since January 1, 1970 (UTC) or Windows file time since January 1, 1601 (UTC)).
 * You should use the values returned by this function for relative measurements only.
 *
 * @name socket.gettime
 * @return seconds [type:number] the number of seconds elapsed.
 * @examples
 *
 * How to use the gettime() function to measure running time:
 *
 * ```lua
 * t = socket.gettime()
 * -- do stuff
 * print(socket.gettime() - t .. " seconds elapsed")
 * ```
 */

/*# sleeps for a number of seconds
 *
 * Freezes the program execution during a given amount of time.
 *
 * @name socket.sleep
 * @param time [type:number] the number of seconds to sleep for.
 *
 */

/****************************************/
/*                                      */
/*                  TCP                 */
/*                                      */
/****************************************/

/*# waits for a remote connection on the server object
 *
 * Waits for a remote connection on the server object and returns a client object representing that connection.
 *
 * [icon:attention] Calling `socket.select` with a server object in the `recvt` parameter before a call to accept does not guarantee accept will return immediately. Use the `settimeout` method or accept might block until another client shows up.
 *
 * @name server:accept
 * @return tcp_client [type:client] if a connection is successfully initiated, a client object is returned, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred. The error is `"timeout"` if a timeout condition is met.
 *
 */

/*# binds a master object to address and port on the local host
 *
 * Binds a master object to address and port on the local host.
 *
 * @name master:bind
 * @param address [type:string] an IP address or a host name. If address is `"*"`, the system binds to all local interfaces using the `INADDR_ANY` constant.
 * @param port [type:number] the port to commect to, in the range [0..64K). If port is 0, the system automatically chooses an ephemeral port.
 * @return status [type:number] the value `1`, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# closes a master TCP object
 *
 * Closes the TCP object. The internal socket used by the object is closed and the local address to which the object was bound is made available to other applications. No further operations (except for further calls to the close method) are allowed on a closed socket.
 *
 * [icon:attention] It is important to close all used sockets once they are not needed, since, in many systems, each socket uses a file descriptor, which are limited system resources. Garbage-collected objects are automatically closed before destruction, though.
 *
 * @name master:close
 *
 */

/*# closes a client TCP object
 *
 * Closes the TCP object. The internal socket used by the object is closed and the local address to which the object was bound is made available to other applications. No further operations (except for further calls to the close method) are allowed on a closed socket.
 *
 * [icon:attention] It is important to close all used sockets once they are not needed, since, in many systems, each socket uses a file descriptor, which are limited system resources. Garbage-collected objects are automatically closed before destruction, though.
 *
 * @name client:close
 *
 */

/*# closes a server TCP object
 *
 * Closes the TCP object. The internal socket used by the object is closed and the local address to which the object was bound is made available to other applications. No further operations (except for further calls to the close method) are allowed on a closed socket.
 *
 * [icon:attention] It is important to close all used sockets once they are not needed, since, in many systems, each socket uses a file descriptor, which are limited system resources. Garbage-collected objects are automatically closed before destruction, though.
 *
 * @name server:close
 *
 */

/*# connects a master object to a remote host
 *
 * Attempts to connect a master object to a remote host, transforming it into a client object. Client objects support methods send, receive, getsockname, getpeername, settimeout, and close.
 *
 * Note that the function `socket.connect` is available and is a shortcut for the creation of client sockets.
 *
 * @name master:connect
 * @param address [type:string] an IP address or a host name. If address is `"*"`, the system binds to all local interfaces using the `INADDR_ANY` constant.
 * @param port [type:number] the port to commect to, in the range [0..64K). If port is 0, the system automatically chooses an ephemeral port.
 * @return status [type:number] the value `1`, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# gets information about a client's peer
 *
 * Returns information about the remote side of a connected client object.
 *
 * [icon:attention] It makes no sense to call this method on server objects.
 *
 * @name client:getpeername
 * @return info [type:string] a string with the IP address of the peer, the port number that peer is using for the connection, and the family ("inet" or "inet6"). In case of error, the method returns `nil`.
 *
 */

/*# gets the local address information from master
 *
 * Returns the local address information associated to the object.
 *
 * @name master:getsockname
 * @return info [type:string] a string with local IP address, the local port number, and the family ("inet" or "inet6"). In case of error, the method returns `nil`.
 *
 */

/*# gets the local address information from client
 *
 * Returns the local address information associated to the object.
 *
 * @name client:getsockname
 * @return info [type:string] a string with local IP address, the local port number, and the family ("inet" or "inet6"). In case of error, the method returns `nil`.
 *
 */

/*# gets the local address information from server
 *
 * Returns the local address information associated to the object.
 *
 * @name server:getsockname
 * @return info [type:string] a string with local IP address, the local port number, and the family ("inet" or "inet6"). In case of error, the method returns `nil`.
 *
 */

/*# gets accounting information on the socket
 *
 * Returns accounting information on the socket, useful for throttling of bandwidth.
 *
 * @name master:getstats
 * @return stats [type:string] a string with the number of bytes received, the number of bytes sent, and the age of the socket object in seconds.
 *
 */

/*# gets accounting information on the socket
 *
 * Returns accounting information on the socket, useful for throttling of bandwidth.
 *
 * @name client:getstats
 * @return stats [type:string] a string with the number of bytes received, the number of bytes sent, and the age of the socket object in seconds.
 *
 */

/*# gets accounting information on the socket
 *
 * Returns accounting information on the socket, useful for throttling of bandwidth.
 *
 * @name server:getstats
 * @return stats [type:string] a string with the number of bytes received, the number of bytes sent, and the age of the socket object in seconds.
 *
 */

/*# makes the master socket listen for connections
 *
 * Specifies the socket is willing to receive connections, transforming the object into a server object. Server objects support the `accept`, `getsockname`, `setoption`, `settimeout`, and `close` methods.
 *
 * @name master:listen
 * @param backlog [type:number] the number of client connections that can be queued waiting for service. If the queue is full and another client attempts connection, the connection is refused.
 * @return status [type:number] the value `1`, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# receives data from a client socket
 *
 * Reads data from a client object, according to the specified `read pattern`. Patterns follow the Lua file I/O format, and the difference in performance between patterns is negligible.
 *
 * @name client:receive
 * @param [pattern] [type:string|number] the read pattern that can be any of the following:
 *
 * `"*a"`
 * : reads from the socket until the connection is closed. No end-of-line translation is performed;
 *
 * `"*l"`
 * : reads a line of text from the socket. The line is terminated by a LF character (ASCII 10), optionally preceded by a CR character (ASCII 13). The CR and LF characters are not included in the returned line. In fact, all CR characters are ignored by the pattern. This is the default pattern;
 *
 * `number`
 * : causes the method to read a specified number of bytes from the socket.
 *
 * @param [prefix] [type:string] an optional string to be concatenated to the beginning of any received data before return.
 * @return data [type:string] the received pattern, or `nil` in case of error.
 * @return partial [type:string] a (possibly empty) string containing the partial that was received, or `nil` if no error occurred.
 * @return error [type:string] the error message, or `nil` if no error occurred. The error message can be the string `"closed"` in case the connection was closed before the transmission was completed or the string `"timeout"` in case there was a timeout during the operation.
 *
 */

/*# sends data through client socket
 *
 * Sends data through client object.
 * The optional arguments i and j work exactly like the standard [ref:string.sub] Lua function to allow the selection of a substring to be sent.
 *
 * [icon:attention] Output is not buffered. For small strings, it is always better to concatenate them in Lua (with the `..` operator) and send the result in one call instead of calling the method several times.
 *
 * @name client:send
 * @param data [type:string] the string to be sent.
 * @param [i] [type:number] optional starting index of the string.
 * @param [j] [type:number] optional end index of string.
 * @return index [type:number] the index of the last byte within [i, j] that has been sent, or `nil` in case of error. Notice that, if `i` is 1 or absent, this is effectively the total number of bytes sent.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# sets options for the socket
 *
 * Sets options for the TCP object. Options are only needed by low-level or time-critical applications. You should only modify an option if you are sure you need it.
 *
 * @name client:setoption
 * @param option [type:string] the name of the option to set. The value is provided in the `value` parameter:
 *
 * `"keepalive"`
 * : Setting this option to `true` enables the periodic transmission of messages on a connected socket. Should the connected party fail to respond to these messages, the connection is considered broken and processes using the socket are notified;
 *
 * `"linger"`
 * : Controls the action taken when unsent data are queued on a socket and a close is performed. The value is a table with the following keys:
 *
 * - [type:boolean] `on`
 * - [type:number] `timeout` (seconds)
 *
 * If the 'on' field is set to true, the system will block the process on the close attempt until it is able to transmit the data or until `timeout` has passed. If 'on' is false and a close is issued, the system will process the close in a manner that allows the process to continue as quickly as possible. It is not advised to set this to anything other than zero;
 *
 * `"reuseaddr"`
 * : Setting this option indicates that the rules used in validating addresses supplied in a call to `bind` should allow reuse of local addresses;
 *
 * `"tcp-nodelay"`
 * : Setting this option to `true` disables the Nagle's algorithm for the connection;
 *
 * `"ipv6-v6only"`
 * : Setting this option to `true` restricts an inet6 socket to sending and receiving only IPv6 packets.
 *
 * @param [value] [type:any] the value to set for the specified option.
 * @return status [type:number] the value `1`, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# sets options for the socket
 *
 * Sets options for the TCP object. Options are only needed by low-level or time-critical applications. You should only modify an option if you are sure you need it.
 *
 * @name server:setoption
 * @param option [type:string] the name of the option to set. The value is provided in the `value` parameter:
 *
 * `"keepalive"`
 * : Setting this option to `true` enables the periodic transmission of messages on a connected socket. Should the connected party fail to respond to these messages, the connection is considered broken and processes using the socket are notified;
 *
 * `"linger"`
 * : Controls the action taken when unsent data are queued on a socket and a close is performed. The value is a table with the following keys:
 *
 * - [type:boolean] `on`
 * - [type:number] `timeout` (seconds)
 *
 * If the 'on' field is set to true, the system will block the process on the close attempt until it is able to transmit the data or until `timeout` has passed. If 'on' is false and a close is issued, the system will process the close in a manner that allows the process to continue as quickly as possible. It is not advised to set this to anything other than zero;
 *
 * `"reuseaddr"`
 * : Setting this option indicates that the rules used in validating addresses supplied in a call to `bind` should allow reuse of local addresses;
 *
 * `"tcp-nodelay"`
 * : Setting this option to `true` disables the Nagle's algorithm for the connection;
 *
 * `"ipv6-v6only"`
 * : Setting this option to `true` restricts an inet6 socket to sending and receiving only IPv6 packets.
 *
 * @param [value] [type:any] the value to set for the specified option.
 * @return status [type:number] the value `1`, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# gets options for the socket
 *
 * Gets options for the TCP object. See [ref:client:setoption] for description of the option names and values.
 *
 * @name client:getoption
 * @param option [type:string] the name of the option to get:
 *
 * - `"keepalive"`
 * - `"linger"`
 * - `"reuseaddr"`
 * - `"tcp-nodelay"`
 *
 * @return value [type:any] the option value, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# gets options for the socket
 *
 * Gets options for the TCP object. See [ref:server:setoption] for description of the option names and values.
 *
 * @name server:getoption
 * @param option [type:string] the name of the option to get:
 *
 * - `"keepalive"`
 * - `"linger"`
 * - `"reuseaddr"`
 * - `"tcp-nodelay"`
 *
 * @return value [type:any] the option value, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# resets accounting information on the socket
 *
 * Resets accounting information on the socket, useful for throttling of bandwidth.
 *
 * @name master:setstats
 * @param received [type:number] the new number of bytes received.
 * @param sent [type:number] the new number of bytes sent.
 * @param age [type:number] the new age in seconds.
 * @return success [type:number] the value `1` in case of success, or `nil` in case of error.
 *
 */

/*# resets accounting information on the socket
 *
 * Resets accounting information on the socket, useful for throttling of bandwidth.
 *
 * @name client:setstats
 * @param received [type:number] the new number of bytes received.
 * @param sent [type:number] the new number of bytes sent.
 * @param age [type:number] the new age in seconds.
 * @return success [type:number] the value `1` in case of success, or `nil` in case of error.
 *
 */

/*# resets accounting information on the socket
 *
 * Resets accounting information on the socket, useful for throttling of bandwidth.
 *
 * @name server:setstats
 * @param received [type:number] the new number of bytes received.
 * @param sent [type:number] the new number of bytes sent.
 * @param age [type:number] the new age in seconds.
 * @return success [type:number] the value `1` in case of success, or `nil` in case of error.
 *
 */

/*# set the timeout values for the socket
 *
 * Changes the timeout values for the object. By default, all I/O operations are blocking. That is, any call to the methods `send`, `receive`, and `accept` will block indefinitely, until the operation completes. The `settimeout` method defines a limit on the amount of time the I/O methods can block. When a timeout is set and the specified amount of time has elapsed, the affected methods give up and fail with an error code.
 *
 * There are two timeout modes and both can be used together for fine tuning.
 *
 * [icon:attention] Although timeout values have millisecond precision in LuaSocket, large blocks can cause I/O functions not to respect timeout values due to the time the library takes to transfer blocks to and from the OS and to and from the Lua interpreter. Also, function that accept host names and perform automatic name resolution might be blocked by the resolver for longer than the specified timeout value.
 *
 * @name master:settimeout
 * @param value [type:number] the amount of time to wait, in seconds. The `nil` timeout value allows operations to block indefinitely. Negative timeout values have the same effect.
 * @param [mode] [type:string] optional timeout mode to set:
 *
 * `"b"`
 * : block timeout. Specifies the upper limit on the amount of time LuaSocket can be blocked by the operating system while waiting for completion of any single I/O operation. This is the default mode;
 *
 * `"t"`
 * : total timeout. Specifies the upper limit on the amount of time LuaSocket can block a Lua script before returning from a call.
 *
 */

/*# set the timeout values for the socket
 *
 * Changes the timeout values for the object. By default, all I/O operations are blocking. That is, any call to the methods `send`, `receive`, and `accept` will block indefinitely, until the operation completes. The `settimeout` method defines a limit on the amount of time the I/O methods can block. When a timeout is set and the specified amount of time has elapsed, the affected methods give up and fail with an error code.
 *
 * There are two timeout modes and both can be used together for fine tuning.
 *
 * [icon:attention] Although timeout values have millisecond precision in LuaSocket, large blocks can cause I/O functions not to respect timeout values due to the time the library takes to transfer blocks to and from the OS and to and from the Lua interpreter. Also, function that accept host names and perform automatic name resolution might be blocked by the resolver for longer than the specified timeout value.
 *
 * @name client:settimeout
 * @param value [type:number] the amount of time to wait, in seconds. The `nil` timeout value allows operations to block indefinitely. Negative timeout values have the same effect.
 * @param [mode] [type:string] optional timeout mode to set:
 *
 * `"b"`
 * : block timeout. Specifies the upper limit on the amount of time LuaSocket can be blocked by the operating system while waiting for completion of any single I/O operation. This is the default mode;
 *
 * `"t"`
 * : total timeout. Specifies the upper limit on the amount of time LuaSocket can block a Lua script before returning from a call.
 *
 */

/*# set the timeout values for the socket
 *
 * Changes the timeout values for the object. By default, all I/O operations are blocking. That is, any call to the methods `send`, `receive`, and `accept` will block indefinitely, until the operation completes. The `settimeout` method defines a limit on the amount of time the I/O methods can block. When a timeout is set and the specified amount of time has elapsed, the affected methods give up and fail with an error code.
 *
 * There are two timeout modes and both can be used together for fine tuning.
 *
 * [icon:attention] Although timeout values have millisecond precision in LuaSocket, large blocks can cause I/O functions not to respect timeout values due to the time the library takes to transfer blocks to and from the OS and to and from the Lua interpreter. Also, function that accept host names and perform automatic name resolution might be blocked by the resolver for longer than the specified timeout value.
 *
 * @name server:settimeout
 * @param value [type:number] the amount of time to wait, in seconds. The `nil` timeout value allows operations to block indefinitely. Negative timeout values have the same effect.
 * @param [mode] [type:string] optional timeout mode to set:
 *
 * `"b"`
 * : block timeout. Specifies the upper limit on the amount of time LuaSocket can be blocked by the operating system while waiting for completion of any single I/O operation. This is the default mode;
 *
 * `"t"`
 * : total timeout. Specifies the upper limit on the amount of time LuaSocket can block a Lua script before returning from a call.
 *
 */

/*# shut down socket
 *
 * Shuts down part of a full-duplex connection.
 *
 * @name client:shutdown
 * @param mode [type:string] which way of the connection should be shut down:
 *
 * `"both"`
 * : disallow further sends and receives on the object. This is the default mode;
 *
 * `"send"`
 * : disallow further sends on the object;
 *
 * `"receive"`
 * : disallow further receives on the object.
 *
 * @return status [type:number] the value `1`.
 *
 */

/*# checks the read buffer status
 *
 * Check the read buffer status.
 *
 * [icon:attention] This is an internal method, any use is unlikely to be portable.
 *
 * @name master:dirty
 * @return status [type:boolean] `true` if there is any data in the read buffer, `false` otherwise.
 *
 */

/*# checks the read buffer status
 *
 * Check the read buffer status.
 *
 * [icon:attention] This is an internal method, any use is unlikely to be portable.
 *
 * @name client:dirty
 * @return status [type:boolean] `true` if there is any data in the read buffer, `false` otherwise.
 *
 */

/*# checks the read buffer status
 *
 * Check the read buffer status.
 *
 * [icon:attention] This is an internal method, any use is unlikely to be portable.
 *
 * @name server:dirty
 * @return status [type:boolean] `true` if there is any data in the read buffer, `false` otherwise.
 *
 */

/*# gets the socket descriptor
 *
 * Returns the underlying socket descriptor or handle associated to the object.
 *
 * [icon:attention] This is an internal method, any use is unlikely to be portable.
 *
 * @name master:getfd
 * @return handle [type:number] the descriptor or handle. In case the object has been closed, the return will be -1.
 *
 */

/*# gets the socket descriptor
 *
 * Returns the underlying socket descriptor or handle associated to the object.
 *
 * [icon:attention] This is an internal method, any use is unlikely to be portable.
 *
 * @name client:getfd
 * @return handle [type:number] the descriptor or handle. In case the object has been closed, the return will be -1.
 *
 */

/*# gets the socket descriptor
 *
 * Returns the underlying socket descriptor or handle associated to the object.
 *
 * [icon:attention] This is an internal method, any use is unlikely to be portable.
 *
 * @name server:getfd
 * @return handle [type:number] the descriptor or handle. In case the object has been closed, the return will be -1.
 *
 */

/*# sets the socket descriptor
 *
 * Sets the underling socket descriptor or handle associated to the object. The current one is simply replaced, not closed, and no other change to the object state is made
 *
 * @name master:setfd
 * @param handle [type:number] the descriptor or handle to set.
 *
 */

/*# sets the socket descriptor
 *
 * Sets the underling socket descriptor or handle associated to the object. The current one is simply replaced, not closed, and no other change to the object state is made
 *
 * @name client:setfd
 * @param handle [type:number] the descriptor or handle to set.
 *
 */

/*# sets the socket descriptor
 *
 * Sets the underling socket descriptor or handle associated to the object. The current one is simply replaced, not closed, and no other change to the object state is made
 *
 * @name server:setfd
 * @param handle [type:number] the descriptor or handle to set.
 *
 */

/****************************************/
/*                                      */
/*                  UDP                 */
/*                                      */
/****************************************/

/*# closes the UDP socket
 *
 * Closes a UDP object. The internal socket used by the object is closed and the local address to which the object was bound is made available to other applications. No further operations (except for further calls to the close method) are allowed on a closed socket.
 *
 * [icon:attention] It is important to close all used sockets once they are not needed, since, in many systems, each socket uses a file descriptor, which are limited system resources. Garbage-collected objects are automatically closed before destruction, though.
 *
 * @name connected:close
 *
 */

/*# closes the UDP socket
 *
 * Closes a UDP object. The internal socket used by the object is closed and the local address to which the object was bound is made available to other applications. No further operations (except for further calls to the close method) are allowed on a closed socket.
 *
 * [icon:attention] It is important to close all used sockets once they are not needed, since, in many systems, each socket uses a file descriptor, which are limited system resources. Garbage-collected objects are automatically closed before destruction, though.
 *
 * @name unconnected:close
 *
 */

/*# gets information about the UDP socket peer
 *
 * Retrieves information about the peer associated with a connected UDP object.
 *
 * [icon:attention] It makes no sense to call this method on unconnected objects.
 *
 * @name connected:getpeername
 * @return info [type:string] a string with the IP address of the peer, the port number that peer is using for the connection, and the family ("inet" or "inet6"). In case of error, the method returns `nil`.
 *
 */

/*# gets the local address information associated to the socket
 *
 * Returns the local address information associated to the object.
 *
 * [icon:attention] UDP sockets are not bound to any address until the `setsockname` or the `sendto` method is called for the first time (in which case it is bound to an ephemeral port and the wild-card address).
 *
 * @name connected:getsockname
 * @return info [type:string] a string with local IP address, a number with the local port, and the family ("inet" or "inet6"). In case of error, the method returns `nil`.
 *
 */

/*# gets the local address information associated to the socket
 *
 * Returns the local address information associated to the object.
 *
 * [icon:attention] UDP sockets are not bound to any address until the `setsockname` or the `sendto` method is called for the first time (in which case it is bound to an ephemeral port and the wild-card address).
 *
 * @name unconnected:getsockname
 * @return info [type:string] a string with local IP address, a number with the local port, and the family ("inet" or "inet6"). In case of error, the method returns `nil`.
 *
 */

/*# receives a datagram from the UDP socket
 *
 * Receives a datagram from the UDP object. If the UDP object is connected, only datagrams coming from the peer are accepted. Otherwise, the returned datagram can come from any host.
 *
 * @name connected:receive
 * @param [size] [type:number] optional maximum size of the datagram to be retrieved. If there are more than size bytes available in the datagram, the excess bytes are discarded. If there are less then size bytes available in the current datagram, the available bytes are returned. If size is omitted, the maximum datagram size is used (which is currently limited by the implementation to 8192 bytes).
 * @return datagram [type:string] the received datagram, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# receives a datagram from the UDP socket
 *
 * Receives a datagram from the UDP object. If the UDP object is connected, only datagrams coming from the peer are accepted. Otherwise, the returned datagram can come from any host.
 *
 * @name unconnected:receive
 * @param [size] [type:number] optional maximum size of the datagram to be retrieved. If there are more than size bytes available in the datagram, the excess bytes are discarded. If there are less then size bytes available in the current datagram, the available bytes are returned. If size is omitted, the maximum datagram size is used (which is currently limited by the implementation to 8192 bytes).
 * @return datagram [type:string] the received datagram, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# receives a datagram from the UDP socket
 *
 * Works exactly as the receive method, except it returns the IP address and port as extra return values (and is therefore slightly less efficient).
 *
 * @name unconnected:receivefrom
 * @param [size] [type:number] optional maximum size of the datagram to be retrieved.
 * @return datagram [type:string] the received datagram, or `nil` in case of error.
 * @return ip_or_error [type:string] the IP address, or the error message in case of error.
 * @return port [type:number] the port number, or `nil` in case of error.
 *
 */

/*# gets options for the UDP socket
 *
 * Gets an option value from the UDP object. See [ref:connected:setoption] for description of the option names and values.
 *
 * @name connected:getoption
 * @param option [type:string] the name of the option to get:
 *
 * - `"dontroute"`
 * - `"broadcast"`
 * - `"reuseaddr"`
 * - `"reuseport"`
 * - `"ip-multicast-loop"`
 * - `"ipv6-v6only"`
 * - `"ip-multicast-if"`
 * - `"ip-multicast-ttl"`
 * - `"ip-add-membership"`
 * - `"ip-drop-membership"`
 *
 * @return value [type:any] the option value, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# gets options for the UDP socket
 *
 * Gets an option value from the UDP object. See [ref:unconnected:setoption] for description of the option names and values.
 *
 * @name unconnected:getoption
 * @param option [type:string] the name of the option to get:
 *
 * - `"dontroute"`
 * - `"broadcast"`
 * - `"reuseaddr"`
 * - `"reuseport"`
 * - `"ip-multicast-loop"`
 * - `"ipv6-v6only"`
 * - `"ip-multicast-if"`
 * - `"ip-multicast-ttl"`
 * - `"ip-add-membership"`
 * - `"ip-drop-membership"`
 *
 * @return value [type:any] the option value, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# sends a datagram through the connected UDP socket
 *
 * Sends a datagram to the UDP peer of a connected object.
 *
 * [icon:attention] In UDP, the send method never blocks and the only way it can fail is if the underlying transport layer refuses to send a message to the specified address (i.e. no interface accepts the address).
 *
 * @name connected:send
 * @param datagram [type:string] a string with the datagram contents. The maximum datagram size for UDP is 64K minus IP layer overhead. However datagrams larger than the link layer packet size will be fragmented, which may deteriorate performance and/or reliability.
 * @return success [type:number] the value `1` on success, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# sends a datagram through the UDP socket to the specified IP address and port number
 *
 * Sends a datagram to the specified IP address and port number.
 *
 * [icon:attention] In UDP, the send method never blocks and the only way it can fail is if the underlying transport layer refuses to send a message to the specified address (i.e. no interface accepts the address).
 *
 * @name unconnected:sendto
 * @param datagram [type:string] a string with the datagram contents. The maximum datagram size for UDP is 64K minus IP layer overhead. However datagrams larger than the link layer packet size will be fragmented, which may deteriorate performance and/or reliability.
 * @param ip [type:string] the IP address of the recipient. Host names are not allowed for performance reasons.
 * @param port [type:number] the port number at the recipient.
 * @return success [type:number] the value `1` on success, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# remove the peer of the connected UDP socket
 *
 * Changes the peer of a UDP object. This method turns an unconnected UDP object into a connected UDP object or vice versa.
 *
 * For connected objects, outgoing datagrams will be sent to the specified peer, and datagrams received from other peers will be discarded by the OS. Connected UDP objects must use the `send` and `receive` methods instead of `sendto` and `receivefrom`.
 *
 * [icon:attention] Since the address of the peer does not have to be passed to and from the OS, the use of connected UDP objects is recommended when the same peer is used for several transmissions and can result in up to 30% performance gains.
 *
 * @name connected:setpeername
 * @param "*" [type:string] if address is "*"" and the object is connected, the peer association is removed and the object becomes an unconnected object again.
 * @return success [type:number] the value `1` on success, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# set the peer of the unconnected UDP socket
 *
 * Changes the peer of a UDP object. This method turns an unconnected UDP object into a connected UDP object or vice versa.
 *
 * For connected objects, outgoing datagrams will be sent to the specified peer, and datagrams received from other peers will be discarded by the OS. Connected UDP objects must use the `send` and `receive` methods instead of `sendto` and `receivefrom`.
 *
 * [icon:attention] Since the address of the peer does not have to be passed to and from the OS, the use of connected UDP objects is recommended when the same peer is used for several transmissions and can result in up to 30% performance gains.
 *
 * @name unconnected:setpeername
 * @param address [type:string] an IP address or a host name.
 * @param port [type:number] the port number.
 * @return success [type:number] the value `1` on success, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# binds the UDP socket to a local address
 *
 * Binds the UDP object to a local address.
 *
 * [icon:attention] This method can only be called before any datagram is sent through the UDP object, and only once. Otherwise, the system automatically binds the object to all local interfaces and chooses an ephemeral port as soon as the first datagram is sent. After the local address is set, either automatically by the system or explicitly by `setsockname`, it cannot be changed.
 *
 * @name unconnected:setsockname
 * @param address [type:string] an IP address or a host name. If address is "*" the system binds to all local interfaces using the constant `INADDR_ANY`.
 * @param port [type:number] the port number. If port is 0, the system chooses an ephemeral port.
 * @return success [type:number] the value `1` on success, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# sets options for the UDP socket
 *
 * Sets options for the UDP object. Options are only needed by low-level or time-critical applications. You should only modify an option if you are sure you need it.
 *
 * @name connected:setoption
 * @param option [type:string] the name of the option to set. The value is provided in the `value` parameter:
 *
 * `"dontroute"`
 * : Indicates that outgoing messages should bypass the standard routing facilities. Receives a boolean value;
 *
 * `"broadcast"`
 * : Requests permission to send broadcast datagrams on the socket. Receives a boolean value;
 *
 * `"reuseaddr"`
 * : Indicates that the rules used in validating addresses supplied in a `bind` call should allow reuse of local addresses. Receives a boolean value;
 *
 * `"reuseport"`
 * : Allows completely duplicate bindings by multiple processes if they all set `"reuseport"` before binding the port. Receives a boolean value;
 *
 * `"ip-multicast-loop"`
 * : Specifies whether or not a copy of an outgoing multicast datagram is delivered to the sending host as long as it is a member of the multicast group. Receives a boolean value;
 *
 * `"ipv6-v6only"`
 * : Specifies whether to restrict inet6 sockets to sending and receiving only IPv6 packets. Receive a boolean value;
 *
 * `"ip-multicast-if"`
 * : Sets the interface over which outgoing multicast datagrams are sent. Receives an IP address;
 *
 * `"ip-multicast-ttl"`
 * : Sets the Time To Live in the IP header for outgoing multicast datagrams. Receives a number;
 *
 * `"ip-add-membership"`: Joins the multicast group specified. Receives a table with fields:
 *
 * - [type:string] `multiaddr` (IP address)
 * - [type:string] `interface` (IP address)
 *
 * "'ip-drop-membership"`
 * : Leaves the multicast group specified. Receives a table with fields:
 *
 * - [type:string] `multiaddr` (IP address)
 * - [type:string] `interface` (IP address)
 *
 * @param [value] [type:any] the value to set for the specified option.
 * @return status [type:number] the value `1`, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */


/*# sets options for the UDP socket
 *
 * Sets options for the UDP object. Options are only needed by low-level or time-critical applications. You should only modify an option if you are sure you need it.
 *
 * @name unconnected:setoption
 * @param option [type:string] the name of the option to set. The value is provided in the `value` parameter:
 *
 * `"dontroute"`
 * : Indicates that outgoing messages should bypass the standard routing facilities. Receives a boolean value;
 *
 * `"broadcast"`
 * : Requests permission to send broadcast datagrams on the socket. Receives a boolean value;
 *
 * `"reuseaddr"`
 * : Indicates that the rules used in validating addresses supplied in a `bind` call should allow reuse of local addresses. Receives a boolean value;
 *
 * `"reuseport"`
 * : Allows completely duplicate bindings by multiple processes if they all set `"reuseport"` before binding the port. Receives a boolean value;
 *
 * `"ip-multicast-loop"`
 * : Specifies whether or not a copy of an outgoing multicast datagram is delivered to the sending host as long as it is a member of the multicast group. Receives a boolean value;
 *
 * `"ipv6-v6only"`
 * : Specifies whether to restrict inet6 sockets to sending and receiving only IPv6 packets. Receive a boolean value;
 *
 * `"ip-multicast-if"`
 * : Sets the interface over which outgoing multicast datagrams are sent. Receives an IP address;
 *
 * `"ip-multicast-ttl"`
 * : Sets the Time To Live in the IP header for outgoing multicast datagrams. Receives a number;
 *
 * `"ip-add-membership"`: Joins the multicast group specified. Receives a table with fields:
 *
 * - [type:string] `multiaddr` (IP address)
 * - [type:string] `interface` (IP address)
 *
 * "'ip-drop-membership"`
 * : Leaves the multicast group specified. Receives a table with fields:
 *
 * - [type:string] `multiaddr` (IP address)
 * - [type:string] `interface` (IP address)
 *
 * @param [value] [type:any] the value to set for the specified option.
 * @return status [type:number] the value `1`, or `nil` in case of error.
 * @return error [type:string] the error message, or `nil` if no error occurred.
 *
 */

/*# sets the timeout value for the UDP socket
 *
 * Changes the timeout values for the object. By default, the `receive` and `receivefrom`  operations are blocking. That is, any call to the methods will block indefinitely, until data arrives. The `settimeout` function defines a limit on the amount of time the functions can block. When a timeout is set and the specified amount of time has elapsed, the affected methods give up and fail with an error code.
 *
 * [icon:attention] In UDP, the `send` and `sendto` methods never block (the datagram is just passed to the OS and the call returns immediately). Therefore, the `settimeout` method has no effect on them.
 *
 * @name connected:settimeout
 * @param value [type:number] the amount of time to wait, in seconds. The `nil` timeout value allows operations to block indefinitely. Negative timeout values have the same effect.
 *
 */

/*# sets the timeout value for the UDP socket
 *
 * Changes the timeout values for the object. By default, the `receive` and `receivefrom`  operations are blocking. That is, any call to the methods will block indefinitely, until data arrives. The `settimeout` function defines a limit on the amount of time the functions can block. When a timeout is set and the specified amount of time has elapsed, the affected methods give up and fail with an error code.
 *
 * [icon:attention] In UDP, the `send` and `sendto` methods never block (the datagram is just passed to the OS and the call returns immediately). Therefore, the `settimeout` method has no effect on them.
 *
 * @name unconnected:settimeout
 * @param value [type:number] the amount of time to wait, in seconds. The `nil` timeout value allows operations to block indefinitely. Negative timeout values have the same effect.
 *
 */
