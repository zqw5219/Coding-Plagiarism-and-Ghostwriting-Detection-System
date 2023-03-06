#include <stdio.h>
#include "csapp.h"
#include "cache.h"
#include "string.h"

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

int parse_url(const char *url, char *host, char *port, char *path);
int read_header(char *buf, rio_t *riop, char *host);
void serve (int connfd);
size_t request_page(char *host, char *port, char *header,
                    char **resp_bufp, char *uri);
int  read_response_header(rio_t *riop, char* header_buf,
                            size_t *content_len, int *ok,
                            int *has_content_len);
size_t read_to_eof(rio_t *riop, char **bufp);
size_t read_bytes(rio_t *riop, char* bufp, size_t count);
char *merge_resp(char *p1, size_t len1, char *p2, size_t len2);

CacheList *cache;

/*
 * init cache
 *
 * Ignore SIGPIPE
 *
 * Start listening on given port
 *
 * loop:
 *  accept client connection
 *  get relevant info
 *  serve content with serve()
 *  close socket
 */
int main(int argc, char** argv)
{
  if (argc != 2) {
    printf("Wrong number of arguments: %d, requires 1\n", argc);
    return -1;
  }

  cache = malloc(sizeof(CacheList));
  cache_init(cache);

  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  char client_hostname[MAXLINE], client_port[MAXLINE];

  Signal(SIGPIPE, SIG_IGN);
  if ((listenfd = open_listenfd(argv[1])) < 0) {
    printf("Can't find a listen port\n");
    return -1;
  }

  while (1) {
    // accept
    clientlen = sizeof(struct sockaddr_storage);
    connfd = accept(listenfd, (SA *)&clientaddr, &clientlen);
    if (connfd < 0) {
      printf("accept error\n");
      continue;
    }
    // get info
    int ret = getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE,
              client_port, MAXLINE, 0);
    if (ret != 0) {
      printf("getnameinfo error: %s\n", gai_strerror(ret));
      continue;
    }
    printf("Connected to %s:%s\n", client_hostname, client_port);
    // serve content
    serve(connfd);
    close(connfd);
  }
  return 0;
}

/*
 * Serve content to client CLIENT_FD.
 *
 * read first line from client
 *
 * parse it to get host, port, path
 *
 * read rest of the headers, add default headers,
 * and request remote page from host with our header.
 *
 * send the response from remote server to our client
 */
void serve (int client_fd) {
  rio_t rio;
  rio_readinitb(&rio, client_fd);

  // read first line
  char line[MAXLINE];
  if ((rio_readlineb(&rio, &line, MAXLINE)) < 0) {
    printf("Failed to read header\n");
    return;
  }
  printf("Received request: %s", line);

  // parse first line
  char method[MAXLINE];
  char uri[MAXLINE];
  char version[MAXLINE];
  if (sscanf(line, "%s %s %s\r\n", method, uri, version) != 3) {
    printf("Failed to parse header: %s\n", line);
    return;
  }
  if (strcasecmp(method, "GET") != 0) {
    printf("not GET, skip\n");
    return;
  }

  // parse uri
  char host[MAXLINE];
  char port[MAXLINE];
  char path[MAXLINE];
  parse_url(uri, host, port, path);
  if (strcmp(host, "") == 0) {
    printf("Failed to parse uri: %s", uri);
    return;
  }

  // read headers and process them
  char header[MAXBUF];
  sprintf(header, "GET %s HTTP/1.0\r\n", path);
  if (read_header(header, &rio, host) < 0) {
    printf("read_header failed, header:\n");
    printf("%s", header);
    return;
  } else {
    printf("read_header success, header:\n");
    printf("%s", header);
  }

  // request for page
  char *resp_buf;
  int resp_len;
  if ((resp_len = request_page(host, port, header,
                               &resp_buf, uri)) < 0) {
    return;
  }

  printf("Got page from remote server (or cache):\n");
  // printf("%.*s", resp_len, resp_buf);

  // send to client
  if (rio_writen(client_fd, resp_buf, resp_len) < 0) {
    free(resp_buf);
    printf("Write to client failed\n");
    return;
  } else {
    free(resp_buf);
    printf("Sent to client\n");
  }

  printf("Served one page\n");
  return;
}

/*
 * Parse URL into HOST, PORT and PATH
 */
int parse_url(const char *url, char *host, char *port, char *path) {
  // 1 if https, 0 if https
  int http = 0;
  // beginning index of host

  int host_beg;
  // length of host
  int host_count = 0;
  // 1 if port exists, 0 if not

  int port_exist = 0;
  // beginning index of port
  int port_beg = 0;
  // length of port
  int port_count = 0;
  // 1 if path exists, 0 if not

  int path_exist = 0;
  // beginning index of path
  int path_beg = 0;
  // length of path
  int path_count = 0;

  // counter
  int i = 0;
  // state for finite machine
  int state = 0;

  // check http v. https
  if (strncasecmp(url, "http://", 7) == 0) {
    http = 1;
    i = 7;
  }
  if (strncasecmp(url, "https://", 8) == 0) {
    i = 8;
  }

  // find each part
  host_beg = i;
  while (url[i] != '\0') {
    switch (state) {
    // we are in host section
    case 0:
      if (url[i] == ':') {
        state = 1;
        port_exist = 1;
        port_beg = i+1;
      } else if (url[i] == '/') {
        state = 2;
        path_exist = 1;
        path_count = 1;
        path_beg = i;
      } else
        host_count++;
      break;
    // we are in port section
    case 1:
      if (url[i] == '/') {
        state = 2;
        path_exist = 1;
        path_count = 1;
        path_beg = i;
      } else
        port_count++;
      break;
    // we are in path section
    case 2:
      path_count++;
    }
    i++;
  }

  // copy each part
  strncpy(host, url+host_beg, host_count);
  host[host_count] = '\0';

  if (port_exist) {
    strncpy(port, url+port_beg, port_count);
    port[port_count] = '\0';
  } else {
    port[0] = '8';
    port[1] = '0';
    port[2] = '\0';
  }

  if (path_exist) {
    strncpy(path, url+path_beg, path_count);
    path[path_count] = '\0';
  } else {
    path[0] = '/';
    path[1] = '\0';
  }

  return http;
}

/*
 * Read the rest headers from RIOP and create a proper request header
 * to HOST and put into BUF.
 *
 * Get each line and parse, look for Connection, etc
 * If key is one of Connection, etc, don’t add to BUF
 * if not, add to BUF
 * Finally add our preset headers to BUF
 */
int read_header(char *buf, rio_t *riop, char *host) {
  int host_exists = 0;

  while (1) {
    char line[MAXLINE];
    char key[MAXLINE];
    char value[MAXLINE];
    size_t count;
    // parse each line
    if ((count = (rio_readlineb(riop, line, MAXLINE))) < 0)
      return -1;
    if (strcmp("\r\n", line) == 0)
      break;
    if (count == 0) {
      printf("Reached EOF without seeing \\r\\n\n");
      return -1;
    }
    if (sscanf(line, "%[^:]: %s\r\n", key, value) != 2) {
      printf("Failed to parse header: %s", line);
      return -1;
    } else
      printf("Parsed header: %s: %s\n", key, value);

    // handle header by their key
    if (strcasecmp(key, "Host") == 0) {
      host_exists = 1;
      if (sprintf(buf, "%s%s", buf, line) < 0)
        return -1;
    } else if ((strcasecmp(key, "Connection") &&
                strcasecmp(key, "Proxy-Connection") &&
                strcasecmp(key, "User-Agent") &&
                strcasecmp(key, "If-Modified-Since") &&
                strcasecmp(key, "If-None-Match")) == 0)
      // zero if any one is zero
      // don’t add them to our new header
      continue;
    else
      if (sprintf(buf, "%s%s", buf, line) < 0)
        return -1;
  }
  // add preset headers
  if (!host_exists)
    if (sprintf(buf, "%sHost: %s\r\n", buf, host) < 0)
      return -1;
  if (sprintf(buf, "%sConnection: close\r\n", buf) < 0)
      return -1;
  if (sprintf(buf, "%sProxy-Connection: close\r\n", buf) < 0)
      return -1;
  if (sprintf(buf, "%s%s\r\n", buf, user_agent_hdr) < 0)
      return -1;
  return 0;
}

/*
 * Request page from HOST:PORT with HEADER, put response into
 * a buffer and put a pointer into RESP_BUFP, URI is for caching.
 *
 * First check if URI is in cache, if so, copy the header and content
 * out and combine them into one buffer and return it.
 *
 * If not in cache, open a client socket and connect to remote server,
 * write HEADER, and get back response headers into RESP_HEADER.
 *
 * Now we read contents, if response header has Content-Length,
 * read exact that much, else read until EOF.
 *
 * Cache the header and content if all 3 criterions are satisfied.
 *
 * Finally combine them into one buffer and return it.
 */
size_t request_page(char *host, char *port, char *header,
                    char **resp_bufp, char *uri) {
  // first try to get from cache
  CachedItem *item = find(uri, cache);
  if (item != NULL) {
    printf("Found in cache\n");
    size_t header_len = strlen(item->headers);
    printf("header: %lu bytes, content: %lu bytes\n",
           header_len, item->size);
    // make copies of cache
    char *header = strdup(item->headers);
    char *content = malloc(item->size);
    if (header == NULL || content == NULL) {
      printf("Make copies from cache failed\n");
      return -1;
    }
    memcpy(content, item->item_p, item->size);
    // merge data and return it
    char *ret_buf = merge_resp(header, header_len,
                               content, item->size);
    free(header);
    free(content);
    if (ret_buf == NULL) {
      printf("merge_resp failed\n");
      return -1;
    }
    *resp_bufp = ret_buf;
    return item->size + header_len;
  }

  // if no cache, request from remote server
  int client_fd;

  // connect to server
  if ((client_fd = open_clientfd(host, port)) < 0) {
    printf("Open client port failed\n");
    return -1;
  } else
    printf("Client port opened: %s\n", port);


  // write
  if (rio_writen(client_fd, header, strlen(header)) < 0) {
    printf("Failed to write to remote server\n");
    close(client_fd);
    return -1;
  } else
    printf("Wrote to remote server\n");

  // read header
  rio_t rio;
  rio_readinitb(&rio, client_fd);
  char resp_header[MAXBUF];
  // length of http content
  size_t content_len;
  // length of header
  size_t resp_header_len;
  size_t count;
  // 1 if status code is 200
  int ok;
  // 1 if header includes content-length
  int has_content_len;
  if ((read_response_header(&rio, resp_header,
                            &content_len, &ok,
                            &has_content_len)) < 0) {
    close(client_fd);
    return -1;
  }
  resp_header_len = strlen(resp_header);

  // read content
  // buffer for http content
  char *resp_content;
  // actually bytes read when content-length is given
  size_t read_count = 0;
  // read until EOF
  if (!has_content_len) {
    printf("Reading until EOF\n");
    if ((count = read_to_eof(&rio, &resp_content)) < 0) {
      printf("Read content failed\n");
      close(client_fd);
      return -1;
    }
    content_len = count;
  }
  // read content_len
  else {
    printf("Reading %d bytes from remote server\n", (int) content_len);
    resp_content = malloc(content_len+1);
    if (resp_content == NULL) {
      printf("malloc failed\n");
      close(client_fd);
      return -1;
    }
    if ((read_count = read_bytes(&rio, resp_content, content_len)) < 0) {
      printf("read from remote server failed\n");
      free(resp_content);
    }
  }
  printf("resp_buf: %lu bytes, resp_header: %lu bytes\n",
         content_len, resp_header_len);

  // maybe cache the stuff
  printf("ok: %d\n", ok);
  printf("read_count: %lu\n", read_count);
  printf("has_content_len: %d\n", has_content_len);
  if (ok && has_content_len && read_count == content_len) {
    printf("Save to cache\n");
    // make copies of data for cache
    char *contentcpy = malloc(content_len);
    char *headercpy = strdup(resp_header);
    char *uricpy = strdup(uri);
    if (contentcpy == NULL || headercpy == NULL
        || uricpy == NULL) {
      printf("Make copy for cache failed\n");
      close(client_fd);
      return -1;
    }
    memcpy(contentcpy, resp_content, content_len);
    // save to cache
    cache_URL(uricpy, headercpy, contentcpy, content_len, cache);
  }

  // we have content in resp_buf and header in resp_header
  // stick them together
  char *ret_buf = merge_resp(resp_header, resp_header_len,
                             resp_content, content_len);
  if (ret_buf == NULL) {
    printf("Merge_resp failed\n");
    close(client_fd);
    return -1;
  } else {
    free(resp_content);
    *resp_bufp = ret_buf;
  }

  // close
  if (close(client_fd) < 0) {
    printf("close failed\n");
    return -1;
  }

  return content_len + resp_header_len;
}

/*
 * Read response header from RIOP, and put header into HEADER_BUF, and
 * update various informations: set HAS_CONTENT_LENGTH to 1 and
 * CONTENT_LENGTH to the length if found the key in the header; set OK to
 * 1 if the ststus code is 200.
 *
 * First read the frist line and parse it and update OK.
 * Add this line to HEADER_BUF.
 *
 * Then read each line and add to HEADER_BUF. If Content-Length
 * is found, set CONTENT_LEN and set HAS_CONTENT_LEN to 1.
 */
int read_response_header(rio_t *riop, char* header_buf, size_t *content_len,
                         int *ok, int *has_content_len) {
  // read first line
  char line[MAXLINE];

  if (rio_readlineb(riop, line, MAXLINE) < 0) {
    printf("Failed to read header from remote server\n");
    return -1;
  } else
    printf("Received response: %s", line);
  sprintf(header_buf, "%s", line);

  // parse first line
  int status_code;
  char version[MAXLINE];
  char status_msg[MAXLINE];
  if (sscanf(line, "%s %d %s\r\n", version, &status_code, status_msg) < 0) {
    printf("Parse first line failed\n");
    return -1;
  } else
    printf("Parse success: %s %d %s\n", version, status_code, status_msg);
  if (status_code == 200)
    *ok = 1;
  else
    *ok = 0;

  *content_len = 0;
  *has_content_len = 0;
  size_t count;
  // read rest line
  while (1) {
    // read a line
    if ((count = rio_readlineb(riop, line, MAXLINE)) < 0) {
      printf("Failed to read header from remote server\n");
      return -1;
    }
    // write to buffer
    sprintf(header_buf, "%s%s", header_buf, line);
    // parse
    if (strcmp("\r\n", line) == 0)
      break;
    if (count == 0) {
      printf("Reached EOF without seeing \\r\\n\n");
      return -1;
    }
    if (strncasecmp("Content-Length", line, 14) == 0) {
      // found
      int len;
      sscanf(line, "%*[^:]: %d\r\n", &len);
      *has_content_len = 1;
      *content_len = len;
    }
  }
  return 0;
}

/*
 * Read RIOP until EOF, and put content in to a buffer and point
 * BUFP to it.
 *
 * First allocate a buffer, and repeatly call rio_readnb until EOF, if
 * the content just read exceeds buffer's size, expand buffer to accommodate.
 */
size_t read_to_eof(rio_t *riop, char **bufp) {
  // tmp buffer
  char buf[MAXBUF];
  // returned buffer
  char *content_buf = malloc(MAXBUF);
  if (content_buf == NULL) {
    printf("malloc failed\n");
    return -1;
  }
  // how many bytes read
  size_t count;
  // position of cursor
  size_t pos = 0;
  // position of cursor if we write buf to content_buf
  size_t newpos;
  // current size of content_buf
  size_t bufsize = 0;
  while (1) {
    count = rio_readnb(riop, buf, MAXBUF);
    if (count == 0)
      break;
    else if (count < 0) {
      printf("Failed to read from remote server\n");
      return -1;
    }
    else {
      newpos = pos + count;
      if (newpos > bufsize) {
        // need to grow buffer
        bufsize += MAXBUF;
        char *newbuf = realloc(content_buf, bufsize);
        if (newbuf == NULL) {
          printf("Realloc failed\n");
          free(content_buf);
          return -1;
        } else
          content_buf = newbuf;
      }
      // copy buf to content_buf
      memcpy(content_buf+pos, buf, count);
      // update pos
      pos = newpos;
    }
  }
  *bufp = content_buf;
  return pos;
}

/*
 * Read COUNT bytes from RIOP to BUFP.
 *
 * Simply read into BUFP until bytes_left is 0 or
 * number of bytes read out = 0. And return number of read bytes.
 */
size_t read_bytes(rio_t *riop, char* bufp, size_t count) {
// bytes left to read from remote server
  size_t bytes_left = count;
  size_t ncount;
  while (1) {
    ncount = rio_readnb(riop, bufp, bytes_left);
    bytes_left -= ncount;
    if (ncount == 0)
      break;
    else if (ncount < 0) {
      printf("Read content failed\n");
      return -1;
    }
  }
  return count - bytes_left;
}

/*
 * Merge two chunks of memory P1 and P2 to gether into a new buffer and
 * return the pointer to it. LEN1, LEN2 are the length of P1 and P2.
 * Doesn’t P1 and P2 are not freed or anything.
 *
 * Create a new buffer and memcpy p1 and p2 into it.
 */
char *merge_resp(char *p1, size_t len1, char *p2, size_t len2) {
  char *ret_buf = malloc(len1 + len2);
  if (ret_buf == NULL) {
    printf("malloc failed\n");
    return NULL;
  }
  memcpy(ret_buf, p1, len1);
  memcpy(ret_buf+len1, p2, len2);
  return ret_buf;
}
