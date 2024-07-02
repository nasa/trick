# Summary

Docker for running koviz without having to worry about dependencies and building, all automatic.

# Building the Docker

Running with `docker-run-koviz.sh` will automatically build the docker image for you if necessary.
However if you want to build the image yourself you can do so with.

```sh
docker compose build
```

# Run
```sh
docker-run-koviz.sh -h                  # for usage
docker-run-koviz.sh /path/to/RUN_dir    # View trick run
docker-run-koviz.sh /path/to/MONTE_dir  # View trick MONTE dir (set of runs)
```
Running this way does not require building manually.  If the binary is not available
it will be built automatically.

In order to run the GUI through the docker, you will need to allow X11 connection from the
docker to the host.  This can be done by running `xhost + {MY_IP}` or `xhost +`.
