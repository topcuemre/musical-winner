#include <iostream>
#include <libusb-1.0/libusb.h>
using namespace std;

libusb_device **rc; //Rocksmith Cable
int find_interface(libusb_device *dev);

int main() {
    libusb_device **devs; //pointer to pointer to devices
    libusb_context *sess = NULL; //libusb session
    int sess_control; //for return values
    ssize_t ctr; //number of devices
    ssize_t i;
    sess_control = libusb_init(&sess); //initialize session

    if(sess_control < 0) {
        cout<<" Session initialization failed"<<sess_control<<endl;
        return 1;
    }

    libusb_set_debug(sess, 3); //verbosity lvl=3
    ctr = libusb_get_device_list(sess, &devs); //list of devices
    if(ctr < 0) {
        cout<<"There are not any device at the present"<<endl;
    }
    for(i = 0; i < ctr; i++) {
        if(find_interface(devs[i])){
            rc = &devs[i];
            break;
        }
    }
    if(rc == NULL) {
        cout<<"Rocksmith cable is not plugged"<<endl;
    }
    libusb_free_device_list(devs, 1); //clear device list
    libusb_exit(sess); //exit session
    return 0;
}

int find_interface(libusb_device *dev) { //function seeks the audio interface device
    libusb_device_descriptor desc;
    int dsc = libusb_get_device_descriptor(dev, &desc);
    if (dsc < 0) {
        cout<<"No device description acquired"<<endl;
    }
    if (desc.idVendor ==  4794 && desc.idProduct == 255) {
        cout<<"Rocksmith cable plugged!"<<endl;
        return 1;
    }
    return 0;
}