#!/bin/bash



setup_cglm() {
    sudo apt-get install autotools-dev
    sudo apt-get install automake
    sudo apt-get install libtool m4

    git clone https://github.com/recp/cglm.git cglm
    pushd cglm
    sh autogen.sh
    ./configure
    make
    sudo make install
    popd
}

#------------------------------
# This is for Ubuntu LTS 20.
#------------------------------
install_deps() {
    sudo apt-get install libasound2-dev
    sudo apt-get install libglew-dev
    sudo apt-get install libsdl2-dev
    sudo apt-get install libsdl2-ttf-dev
    sudo apt-get install libsdl2-image-2.0-0
    sudo apt install libsdl2-image-dev
    sudo apt install libglm-dev

    setup_cglm
}

install_gdbgui() {
    sudo apt install python3-pip
    python3 -m pip install --user pipx
    python3 -m userpath append ~/.local/bin
    sudo apt install pipx
    pipx upgrade gdbgui

    sudo apt install python3.8-venv
    pipx install --force gdbgui
    pipx ensurepath
}

jclient() {
    gdbgui --no-browser -g 'gdb -ex "file app"'
}
