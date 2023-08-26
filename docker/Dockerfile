FROM alpine:latest
LABEL Description="Linux build/debug environment with Mesa OpenGL"
LABEL Maintainer="dhanakoti.murali@gmail.com"

ENV HOME /root

SHELL ["/bin/sh", "-c"]

# Setup apk
RUN apk update

# Base dev environment
RUN apk add \ 
    build-base \ 
    clang \
    cmake \
    gdb \
    wget \ 
    glew-dev \
    glfw-dev \   
    glm-dev \
    valgrind \
    git \
    vim    

# Create home directory to place vim settings and ssh keys
RUN mkdir /home/root    

CMD ["sh"] 
