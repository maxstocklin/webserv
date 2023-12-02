# WEBSERV

## Custom C++ Web Server from Scratch

![Web Server Showcase](./www/webserv_showcase.png)

## Overview

This project presents a custom-built web server written in C++. It's a lightweight and efficient server capable of handling HTTP requests and executing CGI scripts, specifically designed for PHP support. The server handles GET, POST, and DELETE methods and operates in a non-blocking manner using the select() system call. It supports multiple clients and servers concurrently, making it suitable for a range of web applications.

## Features

HTTP Methods: Supports GET, POST, and DELETE methods.
Concurrent Connections: Uses select() for non-blocking, simultaneous handling of multiple clients and servers.
CGI Support: Includes a handler for PHP, allowing the execution of PHP scripts.
Default Files: Comes with a set of default HTML, CSS, and PHP files to demonstrate the server's capabilities.

## Getting Started

### Prerequisites
Ensure you have a C++ compiler and make tool installed on your system. The server is built and tested on [specify OS and compiler version, if applicable].

### Installation

1. Clone the repository:

```bash
git clone git@github.com:maxstocklin/webserv.git
```

3. Compile the server:

```bash
make
```
4. Running the Server:

To start the server, use the following command:

```bash
./webserv [path_to_config_file]
```

4. Access the Web Server:

After starting the server with the default config file, you can access the default web pages provided. Point your web browser to http://localhost:[port] where [port] is the port number specified in your config file.

## Usage Examples

For example, to run the server with the default configuration:

```bash
./webserv confs/default.conf
```

You can then access: http://localhost:80 or http://localhost:88 on your web browser

### Configuration
The server configuration can be modified using the config files located in confs/. The default configuration file is default.conf. You can create your custom config file following the same format.

## Contributors & Acknowledgments

- [@stuartrapop](https://github.com/stuartrapop)
- [@maxstocklin](https://github.com/maxstocklin)

Webserv is a 42 project.


