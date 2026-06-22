

This project was implemented for the 42 curriculum by <miltavar> and <helsnous>

# WebServ

Small HTTP server written in C++ as part of the 42 school curriculum.

## Description

WebServ is a minimal HTTP server that can:

- Serve static files from the `www/` directory.
- Handle multiple `server` blocks and `location` directives.
- Execute CGI scripts (e.g. `.py`, `.php`).
- Receive file uploads via the `upload_store` directive.
- Use custom error pages and enable `autoindex`.

## Main features

- Multi-vhost / multi-port support
- Configuration file parsing (`server` / `location` blocks)
- CGI support via `cgi_ext`
- Configurable allowed HTTP methods per `location`
- Clean signal handling (SIGINT / SIGTERM)

## Instructions

- A modern C++ compiler (e.g. `g++`)
- `make`
- Interpreters for CGI scripts if needed (`python3`, `php-cgi`, ...)

## Build

From the project root, run:

```bash
make
```

## Run

Start the server with a configuration file:

```bash
./webserv configure.conf
```

## Testing

To verify the session cookie flow end to end, run:

```bash
sh ./test_session.sh
```

Expected results:

- `POST /login` returns `Set-Cookie: session_id=...`
- `GET /profile` returns `200 OK` with the user name
- `GET /logout` clears the cookie with `Max-Age=0`
- `GET /profile` after logout returns `401 Unauthorized`

## Configuration

The `configure.conf` file contains `server` and `location` blocks. Example directives:

- `listen`, `server_name`
- `root`, `index`, `autoindex`
- `allow_methods` (GET, POST, DELETE...)
- `upload_store` to set the upload target directory
- `cgi_ext .py /usr/bin/python3` to map an extension to a CGI executable

## Project structure

- `srcs/` — C++ source code (subfolders `Client`, `Config`, `Parsing`, `Server`, `Socket`)
- `includes/` — headers and interfaces
- `www/` — web content served (index, assets, upload_file/)
- `cgi-bin/` — example CGI scripts
- `configure.conf` — example configuration used for testing

## Where to start reading the code

- Entry point: `srcs/main.cpp`
- Configuration parser: `srcs/Parsing/Parsing.cpp` and `includes/Config/Config.hpp`
- Network loop & servers: `srcs/Server/Server.cpp`
- Client/request handling: `srcs/Client/Client.cpp`
- Low-level socket code: `srcs/Socket/Socket.cpp`

## Resources

- **HTTP specs:** RFC 7230, RFC 7231 (HTTP/1.1) — protocol structure, headers, semantics.
- **CGI spec:** RFC 3875 (Common Gateway Interface).
- **Network programming:** Beej’s Guide to Network Programming; *UNIX Network Programming* (W. R. Stevens).
- **Linux man pages:** `socket(2)`, `bind(2)`, `listen(2)`, `accept(2)`, `epoll(7)`, `select(2)`, `poll(2)`.
- **C++ reference:** cppreference.com and the C++ Core Guidelines.
- **Web docs:** MDN Web Docs (HTTP methods, status codes, headers).
- **Example servers & architecture:** study nginx and lighttpd source and design patterns on GitHub.
- **Async / networking libraries:** Boost.Asio documentation; libmicrohttpd for lightweight patterns.
- **Debugging & profiling:** Valgrind, AddressSanitizer/UBSan (use compiler flags), GDB, strace, lsof, tcpdump/wireshark.
- **Code quality tools:** clang-format, clang-tidy, ccache.
- **JWT Debugger.

