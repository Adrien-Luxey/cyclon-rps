import datetime
import random
import json


def utcnow_string():
    return datetime.datetime.utcnow().isoformat()+'Z'


class SafeList:
    def __init__(self):
        self.list = []

    def append(self, x):
        self.list.append(x)

    def get_subset(self, n):
        n = min(n, len(self.list))
        return random.sample(self.list, n)


def load_config(conf_path):
    try:
        f = open(conf_path, 'r')
        conf = json.load(f)
    except Exception as e:
        print("Failed loading config file from {}: {}".format(
            conf_path, e))
        return None
    else:
        print("Succesfully loaded config file from {}.".format(
            conf_path))
        return conf
