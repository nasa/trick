import subprocess

def run(program):
    print("Running:", program)
    process = subprocess.run(program.split(" "), capture_output=True)
    return process