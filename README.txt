# you should export the host "network namespace" to the Docker container into the /var/run/netns-host file.

if [ ! -e /var/run/netns-host ]; then
  touch /var/run/netns-host
  mount --bind /proc/1/ns/net /var/run/netns-host
fi

docker create --privileged=true -v /var/run/netns-host:/var/run/netns-host ...

# and then you should add to your /etc/apache2/envvars file:
export LD_PRELOAD=/root/docker_preload_listen_host/libpreload-docker-listen-host.so
