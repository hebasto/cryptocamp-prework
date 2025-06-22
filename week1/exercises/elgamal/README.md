# ElGamal PKC for ℤp*

### Testing Guide

Test vectors: https://gist.github.com/devinrsmith/19256389288b7e9ff5685a658f9b22d1

```sh
c++ -std=c++20 elgamal.cpp -o elgamal -DMODULUS=71 -DGENERATOR=33
./elgamal a=62 k=31 m=15
```

```sh
c++ -std=c++20 elgamal.cpp -o elgamal -DMODULUS=23 -DGENERATOR=11
./elgamal a=6 k=3 m=10
```

```sh
c++ -std=c++20 elgamal.cpp -o elgamal -DMODULUS=809 -DGENERATOR=3
./elgamal a=68 k=89 m=100
```

```sh
c++ -std=c++20 elgamal.cpp -o elgamal -DMODULUS=17 -DGENERATOR=6
./elgamal a=5 k=10 m=13
```
