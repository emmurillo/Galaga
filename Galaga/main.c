/*
 ██████╗  █████╗ ██╗      █████╗  ██████╗  █████╗ 
██╔════╝ ██╔══██╗██║     ██╔══██╗██╔════╝ ██╔══██╗
██║  ███╗███████║██║     ███████║██║  ███╗███████║
██║   ██║██╔══██║██║     ██╔══██║██║   ██║██╔══██║
╚██████╔╝██║  ██║███████╗██║  ██║╚██████╔╝██║  ██║
 ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝
               
Lenguajes de programación
Prof: M.Sc. Eddy Ramírez Jiménez                                   
Emmanuel Murillo Sánchez
Anamari Morales
19/09/2014

██╗     ██╗██████╗ ██████╗ ███████╗██████╗ ██╗ █████╗ ███████╗
██║     ██║██╔══██╗██╔══██╗██╔════╝██╔══██╗██║██╔══██╗██╔════╝
██║     ██║██████╔╝██████╔╝█████╗  ██████╔╝██║███████║███████╗
██║     ██║██╔══██╗██╔══██╗██╔══╝  ██╔══██╗██║██╔══██║╚════██║
███████╗██║██████╔╝██║  ██║███████╗██║  ██║██║██║  ██║███████║
╚══════╝╚═╝╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚══════╝
                                                              

sudo apt-get update
sudo apt-get install build-essential git cmake xorg-dev libgl1-mesa-dev libglu-dev

sudo apt-get install libpng-dev libcurl4-nss-dev libfreetype6-dev libjpeg-dev libvorbis-dev libopenal-dev libphysfs-dev libgtk2.0-dev libasound-dev libpulse-dev libflac-dev libdumb1-dev

sudo apt-get install git

sudo git clone git://git.code.sf.net/p/alleg/allegro

cd allegro

sudo git checkout 5.0

mkdir build

cd build

sudo ccmake -DCMAKE_INSTALL_PREFIX=/usr ..

Now, press ‘c’ twice then ‘g’.

sudo make

sudo make install
_________________________

Go to “Settings/Compiler and debugger…”
Under “Search directories/Compiler”, press “Add” and add “/usr/include/allegro5″

Under “Search directories/Linker”, press “Add” and add “/usr/lib”
_________________________

/usr/lib/liballegro_dialog.so /usr/lib/liballegro_color.so /usr/lib/liballegro_audio.so /usr/lib/liballegro_image.so /usr/lib/liballegro_physfs.so /usr/lib/liballegro.so /usr/lib/liballegro_font.so /usr/lib/liballegro_acodec.so /usr/lib/liballegro_main.so /usr/lib/liballegro_memfile.so /usr/lib/liballegro_primitives.so /usr/lib/liballegro_ttf.so
pthread*/


int main(){

    iniciarJuego();
}
