#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <sched.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
  struct sockaddr_in *local4 = (struct sockaddr_in *)addr;
  struct sockaddr_in6 *local6 = (struct sockaddr_in6 *)addr;
  int flag_listen_host = 0;
  int fd_save_netns_container;

  unsetenv("LD_PRELOAD");

  if (local4->sin_family == AF_INET && ntohs(local4->sin_port) >= HOOK_MIN_PORT && ntohs(local4->sin_port) <= HOOK_MAX_PORT)
    flag_listen_host = 1;
  if (local6->sin6_family == AF_INET6 && ntohs(local6->sin6_port) >= HOOK_MIN_PORT && ntohs(local6->sin6_port) <= HOOK_MAX_PORT)
    flag_listen_host = 1;

  if (flag_listen_host)
    {
      int fd_netns_host;
      int fd_new_socket;

      if ((fd_save_netns_container = open("/proc/self/ns/net", O_RDONLY)) < 0)
	err(1, "%s : open /proc/self/ns/net failed", __func__);
      if ((fd_netns_host = open("/var/run/netns-host", O_RDONLY)) < 0)
	err(1, "%s : open /var/run/netns-host failed", __func__);
      if (setns(fd_netns_host, CLONE_NEWNET) != 0)
	err(1, "%s : setns fd_netns_host failed", __func__);
      close(fd_netns_host);

      if ((fd_new_socket = socket(addr->sa_family, SOCK_STREAM, 0)) < 0)
	err(1, "%s : socket failed", __func__);
      if (dup2(fd_new_socket, sockfd) != sockfd)
	err(1, "%s : dup2 failed", __func__);
      close(fd_new_socket);
    }
  int ret = syscall(SYS_bind, sockfd, addr, addrlen);
  if (flag_listen_host)
    {
      if (setns(fd_save_netns_container, CLONE_NEWNET) != 0)
	err(1, "%s : setns fd_save_netns_container failed", __func__);
      close(fd_save_netns_container);
    }

  return ret;
}
