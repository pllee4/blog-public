if sudo docker build --rm --force-rm . -t local/cxx20_samples:latest -f ./Dockerfile.cxx20_samples ; then
    sudo docker run -it local/cxx20_samples:latest
else
    echo "Build error!"
    exit 1
fi