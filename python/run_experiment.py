#!/usr/bin/env python3

import argparse
import sys


import modules


parser = argparse.ArgumentParser("Launches a Cyclon-RPS experiment.")
parser.add_argument(
    '--conf', dest="conf", default="conf/default.json",
    help="Path to experiment JSON config file (defaults to stor.default_conf)")

if __name__ == '__main__':
    args = parser.parse_args()

    conf = modules.load_config(args.conf)
    if conf is None:
        sys.exit(1)

    modules.build_project(conf)

    exp = modules.Experiment(conf)
    try:
        exp.bootstrap()
        exp.start()
    except KeyboardInterrupt:
        exp.terminate()
        sys.exit(1)
    else:
        exp.terminate()
        print("Experiment succeeded!")
        sys.exit(0)
