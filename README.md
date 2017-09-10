<h1 align="center">Emmi-Greeter</h1>

<p align="center">forked from <a href="https://github.com/surlykke/qt-lightdm-greeter">qt-lightdm-greeter</a></p>

<p align="center">
<a href="https://github.com/emmilinux/emmi-greeter/blob/master/LICENSE"><img src="https://img.shields.io/badge/License-GPL--3.0-yellow.svg" alt="license"/></a></p>


## Instalação

git clone https://github.com/emmilinux/emmi-greeter.git

```
cd emmi-greeter
mkdir build
cd build
cmake ..
make 
sudo make install
```

vá em /etc/lightdm/lightdm.conf e adiciona o emmi-greeter na linha greeter-session.<br>
greeter-session=emmi-greeter<br>

## configuração
`/etc/lightdm/emmi-greeter.conf`

<br>

![CSCore Logo](https://raw.githubusercontent.com/emmilinux/emmi-greeter/master/screeshot/greeter.png)
