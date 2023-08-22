FROM utensils/opengl:latest
LABEL Description="Linux build/debug environment with Mesa OpenGL"
LABEL Maintainer="dhanakoti.murali@gmail.com"

ENV HOME /root

SHELL ["/bin/sh", "-c"]

# Base dev environment
RUN apk add build-base \ 
    clang \
    cmake \
    gdb \
    wget

CMD ["sh"] 
