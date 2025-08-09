# Steps:
## Replace:
### arduino_secrets.h:
Insert the asked secrets
### main.cpp
If needed change the part above `int main(){`
## Run:
Just upload via platformIO, it automatically compiles and links everything that is needed.
If you have the platformIO cli installed, you can simply run `pio run --target upload`.