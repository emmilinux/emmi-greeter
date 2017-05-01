# emmi-greeter

forked from <a href="https://github.com/Dev-Linux/qt-lightdm-greeter">qt-lightdm-greeter</a>

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
