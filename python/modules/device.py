class Device:
    def __init__(self, conf, node, d_id, *args, **kwargs):
        self.conf = conf
        self.node = node
        self.id = d_id
        self.started = False
        self.name = "{}_d{:04d}".format(conf["project_name"], d_id)
        self.ip = ""
        self.image_name = self.conf["images"]["rps"]
        self.volumes = {}

    def start(self, ips_generator, running_ips=None):
        self.ip = str(next(ips_generator))
        bsips = running_ips.get_subset(
            self.conf['parameters']['rps_bootstrap_ips'])

        rps_args = self.conf['parameters']['rps_args'].format(
            name=self.name,
            bs_ips=','.join(bsips),
            ip=self.ip,
            **self.conf['parameters']['rps'])

        action = "Flabbergasted"
        try:
            self.cont = start_device_container(
                self.name, rps_args, self.conf['net']['name'],
                self.image_name, self.volumes, self.ip,
                self.conf['project_name'], self.node.docker)
        except Exception as e:
            action = "Error starting"
            self.started = False
            print(e)
        else:
            action = "Started"
            self.started = True
            running_ips.append(self.ip)
        finally:
            print("{action} {device_name} {ip}.".format(
                action=action, device_name=self.name, ip=self.ip))


# --- Utils ---


# Sets the image name as label of the container
def start_device_container(name, args, net_name,
                           image_name, volumes, ip,
                           label, docker_clients):
     # Create Docker instance config
    net_conf = docker_clients[1].create_networking_config({
        net_name: docker_clients[1].create_endpoint_config(
            ipv4_address=ip)
    })
    vol_conf = {}
    volumes_cont_list = []
    if volumes is not None:
        for vol in volumes.values():
            vol_conf[vol['host']] = {
                'bind': vol['cont'],
                'mode': vol['mode'],
                'no_copy': True
            }
            volumes_cont_list.append(vol['cont'])
    host_conf = docker_clients[1].create_host_config(
        binds=vol_conf, network_mode=net_name)

    cont_info = docker_clients[1].create_container(
        image_name, args, volumes=volumes_cont_list, name=name,
        labels=[label], networking_config=net_conf,
        host_config=host_conf, detach=True)
    docker_clients[1].start(cont_info)

    cont_id = cont_info['Id'][:12]
    return docker_clients[0].containers.get(cont_id)
