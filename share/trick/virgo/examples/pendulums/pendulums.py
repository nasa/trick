#!/usr/bin/env python3
import sys, os, argparse, yaml, inspect
# Add location of VIRGO code to sys.path so we can import classes
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))
sys.path.append(os.path.abspath(os.path.join(thisFileDir, '../../')))
# Import the only VIRGO class we need
from VirgoDataPlayback import VirgoDataPlayback

parser = argparse.ArgumentParser(
    description='Generate pendulum data then visualize it with VIRGO',
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("--scene", help="Scene YAML config file to load",
                    default=os.path.join(thisFileDir,'scene.yml'))
parser.add_argument("--data-dir", help="Directory to write the CSV data to",
                    default=thisFileDir)
parser.add_argument("--headless", action="store_true",
                    help="Render to video instead of providing an"
                    " interactive window")
parser.add_argument("--splash", help="Display VIRGO splash screen",
                    default=True)
parser.add_argument("--video-filename",
                    help="Filename for headless video when --headless given",
                    default=os.path.join(thisFileDir, 'pendulums.mp4'))
args = parser.parse_args()

class PendulumExample:
    def __init__(self):
        self.generate_pendulum_csv(logfile='log_pendulum.csv')
        self.generate_pendulum_csv(logfile='log_pendulum_small.csv', L=0.5, rate=15)
        with open(args.scene) as file:
            scene = yaml.safe_load(file) 
        # VirgoDataPlayback is the VirgoScene we want because it's built
        # to consume TrickPy-compatible data, which log_pendulum.csv meets
        self.v = VirgoDataPlayback(run_dir=args.data_dir, scene=scene,
                                    headless=args.headless,
                                    video_filename=args.video_filename, splash=args.splash)
        # Initialize the scene but do not render yet
        self.v.initialize()

    @staticmethod
    def generate_pendulum_csv(logfile='log_pendulum.csv',
                              L = 1.0, num_data_points=1001, rate=10):
        """
        Simple analytic generation of data for motion of a 3D pendulum.
        Data is dumped to logfile, a comma-separated-value file,
        ready for VIRGO consumption via VirgoDataPlayback() class.

        Args:
          L (float): Length of pendulum string
          num_data_points (int): Total number of data points to generate
          rate (int): time-history rate. ex: 10 means each line increments
            time by 1/10=0.1 sec.
        """
        import math
        g = 9.81
        theta0, omega, Omega = 0.5236, math.sqrt(g/L), 0.05
        with open(logfile, 'w') as f:
            f.write('time (s), pos[0] (m), pos[1] (m), pos[2] (m)\n')
            for t in [i/rate for i in range(num_data_points)]:
                theta = theta0 * math.cos(omega * t)
                phi = Omega * t
                x = L * math.sin(theta) * math.cos(phi)
                y = L * math.sin(theta) * math.sin(phi)
                z = -L * math.cos(theta)
                f.write(f'{t:.5f},{x:.15f},{y:.15f},{z:.15f}\n')

    def run(self):
        # Enter the renderer/interactor loop which creates the VIRGO window
        return(self.v.run())

if __name__ == '__main__':
    sys.exit(PendulumExample().run())
