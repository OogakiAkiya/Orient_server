FROM ubuntu:22.04

RUN apt-get update && apt-get -y upgrade && \
    apt-get -y install git && \
    apt-get -y install unzip && \
    apt-get -y install wget && \
    apt-get install -y build-essential && \
    apt-get -y install libssl-dev


# Install compilers
RUN apt-get -y install gcc g++

ENV CC=/usr/bin/gcc \
    CXX=/usr/bin/g++


#install cmake
RUN cd $HOME && \
    wget https://github.com/Kitware/CMake/releases/download/v3.16.3/cmake-3.16.3.tar.gz && \
    tar zxvf cmake-3.16.3.tar.gz && \
    cd cmake-3.16.3 && \
    ./bootstrap && \
    make -j12 && make install -j8

RUN echo 'export PATH=$HOME/cmake-3.16.3/bin/:$PATH' >> ~/.bashrc && \
    . ~/.bashrc

# install boost
RUN cd $HOME && \
    wget https://github.com/boostorg/boost/releases/download/boost-1.81.0/boost-1.81.0.tar.gz && \
    tar zxvf boost-1.81.0.tar.gz && \
    cd boost-1.81.0 && \
    ./bootstrap.sh && \
    ./b2 install -j4 --prefix=/usr/local link=static

# Orient_server download
RUN cd $HOME && \
    git clone https://github.com/OogakiAkiya/Orient_server.git && \
    cd Orient_server && \
    cmake -S . -B build && \
    cmake --build build 

CMD $HOME/Orient_server/build/Orient_server.out
    
