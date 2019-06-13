import time
from ipaddress import IPv4Network

from .node import Node
from .utils import utcnow_string, SafeList
from .device import Device


class Experiment:
    def __init__(self, conf):
        self.conf = conf
        self._terminated = False

        # Devices running the project code
        self.devices = []
        # Cluster hosts
        self.nodes = [Node(conf)]
        # Global IPs provider
        self.ips_generator = None

    def bootstrap(self):
        print("\n{} Beginning bootstrap...".format(utcnow_string()))

        self._stop_or_rm_containers(rm=True)
        self._bootstrap_devices()

        print("{} Done bootstrapping.\n".format(utcnow_string()))

    def start(self):
        print("{} Sleeping {}s...".format(
            utcnow_string(), self.conf["experiment"]["sleep_time"]))
        time.sleep(self.conf["experiment"]["sleep_time"])

    def terminate(self):
        print("\nTerminating experiment...")

        if self._terminated:
            print("Already terminated.")
            return

        self._stop_or_rm_containers(
            rm=not self.conf['parameters']['only_stop'])

    def _bootstrap_devices(self):
        self.devices = [
            Device(self.conf, self.nodes[0], d_id, False)
            for d_id in range(self.conf['experiment']['n_devices'])]

        ips_generator = IPv4Network(self.conf['net']['addr']).hosts()
        [next(ips_generator) for _ in range(2)]
        running_ips = SafeList()

        for d in self.devices:
            d.start(ips_generator, running_ips)

    # --- Utils ---
    def _stop_or_rm_containers(self, rm=False):
        self.nodes[0].stop_or_rm(
            label=self.conf['project_name'], rm=rm)
