#!/usr/bin/env python3.11
import sys, os, argparse, yaml, inspect

# Location of Virgo code for 3D rendering, and dependency trickpy for loading sim data
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))
sys.path.append(os.path.abspath(os.path.join(thisFileDir, '../../share/trick/virgo/')))
from VirgoDataPlayback import VirgoDataPlayback

parser = argparse.ArgumentParser(description=
        'Visualize the SIM_satellite data after the simulation has finished. '
        'Make sure Modified_data/Satellite.dr is configured to log data before running '
        'this simulation, then execute this script after logged data exists in the --run '
        'directory. This script uses VIRGO which depends on pip packages PyYAML and '
        'vtk, see share/trick/virgo/README.md for details.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
        )
parser.add_argument("--run", help="RUN directory to load.", default='RUN_test')
parser.add_argument("--scene-config", help="YAML config file to load", default='.virgo_config.yml')
args = parser.parse_args()

class VisualizeWithVirgo:
    def __init__(self):
        with open(args.scene_config) as file:
            scene = yaml.safe_load(file) 
        self.v = VirgoDataPlayback(run_dir=args.run, scene=scene, verbosity=5)
        self.v.initialize()
    def execute(self):
        # Start the Virgo 3D scene
        return(self.v.run())

if __name__ == '__main__':
    sys.exit(VisualizeWithVirgo().execute())