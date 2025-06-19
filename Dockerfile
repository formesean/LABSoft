FROM i386/debian:bullseye

RUN apt-get update && apt-get install -y \
  build-essential \
  libx11-dev \
  libxext-dev \
  libxft-dev \
  libxinerama-dev \
  wget \
  cmake \
  git \
  vim \
  x11-apps \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /opt
RUN wget https://www.fltk.org/pub/fltk/1.3.5/fltk-1.3.5-source.tar.gz && \
    tar -xvzf fltk-1.3.5-source.tar.gz && \
    cd fltk-1.3.5 && \
    ./configure && \
    make -j$(nproc) && \
    make install

WORKDIR /labsoft
CMD ["/bin/bash"]
