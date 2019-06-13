import shlex
import subprocess as sp
import sys
import os


def build_project(conf):
    args = ""

    cmd = "make docker " + args

    project_path = os.path.abspath(conf['dirs']['project'])
    try:
        sp.run(shlex.split(cmd), cwd=project_path, check=True)
    except sp.CalledProcessError as e:
        print("Failed building project.")
        sys.exit(1)
    else:
        print("Successfully built project!")
