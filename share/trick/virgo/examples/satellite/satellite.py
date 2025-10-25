#!/usr/bin/env python3.11
import sys, os, argparse, yaml, inspect
# Add location of VIRGO code to sys.path so we can import classes
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))
sys.path.append(os.path.abspath(os.path.join(thisFileDir, '../../')))
# Import the only VIRGO class we need
from VirgoDataPlayback import VirgoDataPlayback

parser = argparse.ArgumentParser(description=
        'Visualize the satellite data in log_Satellite.csv using VIRGO.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
        )
parser.add_argument("--scene-config", help="YAML config file to load",
                    default=os.path.join(thisFileDir,'scene.yml'))
parser.add_argument("--data-dir", help="Directory to read the CSV data from",
                    default=thisFileDir)
parser.add_argument("--headless", action="store_true",
                    help="Render to video instead of providing an"
                    " interactive window")
parser.add_argument("--video-filename",
                    help="Filename for headless video when --headless given",
                    default=os.path.join(thisFileDir, 'satellite.mp4'))
args = parser.parse_args()

class SatelliteExample:
    def __init__(self):
        with open(args.scene_config) as file:
            scene = yaml.safe_load(file) 
        # VirgoDataPlayback is the VirgoScene we want because it's built
        # to consume TrickPy-compatible data, which log_Satellite.csv meets
        self.v = VirgoDataPlayback(run_dir=args.data_dir, scene=scene,
                                    headless=args.headless,
                                    video_filename=args.video_filename)
        self.v.initialize()
    def execute(self):
        # Start the Virgo 3D scene
        return(self.v.run())

if __name__ == '__main__':
    sys.exit(SatelliteExample().execute())