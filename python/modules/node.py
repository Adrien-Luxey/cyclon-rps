import docker


class Node:
    """Physical host

    e.g. localhost
    """

    def __init__(self, conf, ip="localhost"):
        self.conf = conf
        self.ip = ip
        if ip == "localhost":
            self.docker = (docker.from_env(), docker.APIClient())

    def stop_or_rm(self, name=None, label=None, ancestor=None, rm=False):
        filter_str = ""
        containers = []

        if name is not None:
            filter_str = "(name={})".format(name)
            containers = self.docker[0].containers.list(
                all=True, filters={'name': name})
        elif label is not None:
            filter_str = "(label={})".format(label)
            containers = self.docker[0].containers.list(
                all=True, filters={'label': label})
        elif ancestor is not None:
            filter_str = "(ancestor={})".format(ancestor)
            containers = self.docker[0].containers.list(
                all=True, filters={'ancestor': ancestor})
        else:
            raise ValueError("Provide a container name or ancestor image")

        if len(containers) == 0:
            print("[{:^15}] No containers found with {}.".format(
                self.ip, filter_str))
            return

        for container in containers:
            if rm:
                action = "Removed"
                container.remove(force=True)
            else:
                action = "Stopped"
                container.stop()

        print("[{:^15}] {} {} container(s) {}.".format(
            self.ip, action, len(containers), filter_str))
