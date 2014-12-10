#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

int socket_util_listen(const char *addr, unsigned short port)
{
  int s;
  struct sockaddr_in name;
  int namelen;
  int reuse = 1;

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    err(1, "%s : socket", __func__);
  namelen = sizeof(struct sockaddr);
  name.sin_family = AF_INET;
  name.sin_port = htons(port);
  name.sin_addr.s_addr = inet_addr(addr);
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (int *)&reuse, sizeof(reuse));
  if (bind(s, (struct sockaddr *)&name, namelen) != 0)
    err(1, "%s : bind", __func__);
  if (listen(s, 5) != 0)
    err(1, "%s : listen", __func__);
  return s;
}

static void _fake_answer(int fd)
{
  FILE *file = fdopen(fd, "w");

  fprintf(file, "%s\n", "HTTP/1.1 200 OK");
  fprintf(file, "%s\n", "Content-Type: text/html;charset=UTF-8");
  fprintf(file, "%s\n", "");
  fprintf(file, "%s", "<html><head></head><body>coucou</body></html>");
  fflush(file);
  fclose(file);
}

int main(int argc, char **argv)
{
  struct sockaddr addr;
  socklen_t addrlen;

  int fd = socket_util_listen("0.0.0.0", HOOK_MIN_PORT);

  addrlen = sizeof(addr);
  int fdclient = accept(fd, &addr, &addrlen);
  printf("%s : got fdclient %d\n", __func__, fdclient);
  _fake_answer(fdclient);
  return 0;
}
