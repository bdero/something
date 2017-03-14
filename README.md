# something-md
Simple SEGA Genesis homebrew example.

## Build instructions

1. Download [andwn's](https://github.com/andwn) latest [gendev build](https://github.com/andwn/gendev/releases),
  extract it, and move the `toolchains` directory into `/opt`.

  ```sh
  # Visit this URL for the latest build: https://github.com/andwn/gendev/releases
  wget https://github.com/andwn/gendev/releases/download/2017.01.20/gendev-17.01.tar.bz2
  tar xvf gendev-17.01.tar.bz2
  sudo mv opt/toolchains /opt
  ```

1. Clone this repository and `make` this demo.

  ```sh
  git clone https://github.com/bdero/something-md.git
  ```

1. (Optional, but recommended) Compile and use [BlastEm](https://www.retrodev.com/blastem/) to run the rom.

  ```sh
  hg clone https://www.retrodev.com/repos/blastem
  cd blastem
  make
  ./blastem /path/to/something.bin
  ```
