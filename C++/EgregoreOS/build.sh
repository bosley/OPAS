# Ensure that the docker container is built
sudo docker build buildenv -t egregore-build-env

# Launch the docker container (runs make and then exits)
sudo docker run --rm -it -v "/home/bosley/projects/EgregoreOS":/root/env egregore-build-env
