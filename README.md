# Prerequisites
 - PF_RING (http://www.ntop.org/products/packet-capture/pf_ring/) (already included).

# Install PF_RING

AJWS uses the PF_RING library to handle TCP/IP packets at wire level. The latest sources are already available under `./lib/pf_ring`. The original name is all in uppercase but I put it in lowercase in purpose to avoid the hassle involved.

### PF_RING kernel module

PF_RING has two components: a kernel module and a userland shared library. The first step is to compile and load the kernel module. Its source code is on `./lib/pf_ring/kernel`.
```
cd lib/pf_ring/kernel
./configure
make
```
The kernel module is named `pf_ring.ko`. We load it with `insmod`.
```
$ sudo insmod ./pf_ring.ko
```
If the above steps were successful, `lsmod` should report the module is loaded:
```
# lsmod | head -1 && lsmod | grep pf_ring

Module                  Size  Used by
pf_ring               709003  0
```

### PF_RING userland libraries

Now we have to compile the PF_RING userland shared libraries. These are found in `./lib/pf_ring/userland/lib`. Here there are two points to take into account. First, we're not going to use libpcap, and PF_RING links against it by default, so we have to tell it not to do so, or we'll introduce unnecessary burden. I don't know about you, but I want my binaries to be as smallest as possible. What's more, you won't be able to compile PF_RING if you don't have libpcap installed. Thus, we pass `--disable-bpf` to `./configure` to prevent it from linking against libpcap. The second point is that PF_RING needs libnuma. More specifically, libnuma's development sources. In a Debian-based system, those are in the package `libnuma-dev`, so make sure you have it installed.
```
cd lib/pf_ring/userland/lib
sudo aptitude install libnuma-dev
./configure --disable-bpf --prefix=$(pwd)/../../
make
make install
```
The above commands will install the libraries in `./lib/pf_ring/lib` and the header files in `./lib/pf_ring/include`. If you want them to be available to the whole system, just feel free to remove the `--prefix` switch. We should have the following two files:
```
$ ls lib/pf_ring/lib
libpfring.a  libpfring.so
$ ls lib/pf_ring/include
pfring.h  pfring_mod_sysdig.h  pfring_zc.h
```

# Compile AJWS

If you survived so far, you're now ready for the final step: compile AJWS. You need the recently compiled libraries, and the header files of both the libraries and the kernel component of PF_RING. These are in `./lib/pf_ring/kernel`.

So, let's go. If you skipped the `--prefix` switch from the previous `./configure` call, you may omit the last two `-I` flags and the `-L` flag, because the PF_RING libraries and headers are already in the expected locations.
```
gcc -o ajws -I. -I./lib/pf_ring/kernel -I./lib/pf_ring/include -L./lib/pf_ring/lib main.c log.c -lpfring -lnuma
```
And don't forget that you must run AJWS as root.
```
sudo ./ajws
```
