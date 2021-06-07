


    sudo docker build buildenv -t egregore-build-env
    sudo docker run --rm -it -v "/home/bosley/projects/egregore":/root/env egregore-build-env
    make build-x86_64
    qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso
