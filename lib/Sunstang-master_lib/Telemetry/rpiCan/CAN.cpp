#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <iostream>
#include <stdint.h> 
#include <linux/can.h>
#include <linux/can/bcm.h>
#include <string.h>
#include <cstring>

#include <time.h> 
#include <fstream>

#include <sys/mount.h>
#include <fstab.h>
#include <mntent.h>
#include <errno.h>

#include <xbee.h>
#include "storage.h"
#include "frameHolder.h"
#include "SunstangHelpers/CANHelper.h"
#include "SunstangHelpers/GPSHelper.h"

storage radioQueue;
storage fileQueue;

void myCallback(struct xbee *xbee, struct xbee_con *con, struct xbee_pkt **pkt, void **data) {
	std::cout << "recieved callback!" << std::endl;
}

void *sendDataThread(void *threadid)
{
   long tid;
   tid = (long)threadid;

	std::string xbeeModel = "xbee2";  //This shouldn't change
	int xbeeBaud = 57600;
	//Setup xbee!
	struct xbee *xbee;
	struct xbee_con *con;
	struct xbee_conAddress address;
	struct xbee_conSettings settings;
	xbee_err ret;



	if ((ret = xbee_setup(&xbee, "xbee2", "/dev/ttyUSB0", 57600)) != XBEE_ENONE) {
		std::cout <<"Can't connect to xbee " << xbee_errorToStr(ret) << std::endl;
		//return ret;
	}


	std::cout << "Xbee is setup" << std::endl;	

	//Setup remote address
	memset(&address, 0, sizeof(address)); //set the memory to 0
	address.addr64_enabled =1;

	address.addr64[0] = 0x00;
	address.addr64[1] = 0x13;
	address.addr64[2] = 0xA2;
	address.addr64[3] = 0x00;
	address.addr64[4] = 0x40;
	address.addr64[5] = 0xA6;
	address.addr64[6] = 0x83;
	address.addr64[7] = 0xEC;

	if ((ret = xbee_conNew(xbee, &con, "Data", &address)) != XBEE_ENONE) {
		std::cout <<"unable to connect to xbee " <<  xbee_errorToStr(ret) << std::endl;
		//return ret;
	}

	if (xbee_conSettings(con, NULL, &settings) != XBEE_ENONE){
		std::cout <<"unable to grab xbee settings: " <<  xbee_errorToStr(ret) << std::endl;
	}	
	
	settings.disableAck = 0;
	settings.noWaitForAck = 0;	

	if (xbee_conSettings(con, &settings, NULL) != XBEE_ENONE){
		std::cout <<"unable to set xbee settings: " <<  xbee_errorToStr(ret) << std::endl;
	}	

	std::cout<< " Xbee connection is setup." << std::endl; 	
	if (xbee_conCallbackSet(con, myCallback, NULL) != XBEE_ENONE){
		std::cout << "can't add callback" << std::endl;
	}

	while(1){
		frameHolder frame = radioQueue.getNext();	
		std::string csv = frame.toString();	
		while(xbee_connTx(con, NULL, (const unsigned char*)csv.c_str(), csv.length()) != XBEE_ENONE){}
		std::cout <<"Transmit success!" << std::endl;
	}
}

/**
	This thread is responsible for saving the data to file
*/
void *saveDataThread(void *threadid)
{
	setfsent();
	fstab* t = getfsfile("/media/drive0");
	FILE * mtab = NULL;
	struct mntent * part = NULL;

	std::ofstream dataLog;
	dataLog.exceptions ( std::ifstream::failbit);

	bool mounted = false;

	//check if the usb is already mounted
	if ( ( mtab = setmntent ( _PATH_MOUNTED, "r") ) != NULL) {
		while ( ( part = getmntent ( mtab) ) != NULL) {
			if ( ( part->mnt_fsname != NULL ) && ( std::strcmp ( part->mnt_fsname, t->fs_spec ) ) == 0 ){
				mounted = true;
				break;
			}
		}
	}

	endmntent(mtab);

	//wait here until a usb is inserted.
	while(!mounted)
	{
		if(!mounted){
			if( mount(t->fs_spec, t->fs_file, t->fs_vfstype , MS_MGC_VAL, t->fs_mntops ) != 0)
			{
				int e = errno;
				if(e == EPERM)
				{
					std::cerr << "You need to be root to mount the USB" << std::endl;
					return (void*) -1;  // the thread can't do anything since USB isn't mounted and the process isn't root. just give up
				}else if( e == ENOENT){
					std::cerr << "Plug the USB drive in..." << std::endl;
				}else{
					std::cerr << "Weird error from mounting usb: " << strerror(e) <<std::endl;
				}
			}else{
				std::cout<< "Mounted USB"  <<std::endl;
				mounted =true;
			}
		}
	}

	while(true)
	{
		if(!dataLog.is_open()){
			try{
				dataLog.open("/media/drive0/data.csv",  std::ofstream::app);
			}catch(std::ifstream::failure e){
				std::cerr << "Unable to open file:  " << strerror(errno) << std :: endl;
				sleep(1);
				continue;
			}
		}

		frameHolder f = fileQueue.getNext();	
		dataLog << f.toString();
		dataLog << std::endl;
	}

	dataLog.close();
}
int main()
{
	std::string ifname = "can0";
	std::string xbeeIfname = "/dev/ttyUSB0";
	int s;
	struct sockaddr_can addr;
	struct msghdr msg;
	struct ifreq ifr;	
	
	pthread_t xbeeThread;
	pthread_t fileThread;


	pthread_create(&xbeeThread, NULL, sendDataThread  ,NULL );	
	pthread_create(&fileThread, NULL, saveDataThread  ,NULL );	


	//Request a Can bus socket
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		std::cout << s << std::endl;
		return 1;
	}
	std::cout << "CAN is Connected" << std::endl;

	

	//Setup where to bind the CAn socket to.
	addr.can_family = AF_CAN;
	strcpy(ifr.ifr_name, "can0");
	ioctl(s, SIOCGIFINDEX, &ifr);
	addr.can_ifindex = ifr.ifr_ifindex;	

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		//std::cout << val << std::endl;
		return 1;
	}
	
	std::cout << "Bound!" << std::endl;


	//Start Reading Data!
	struct can_frame frame;
	
	//TODO:: ensure drive is mounted


	time_t timer;
	bool timeSet = false;
	
	for(int i  =0; i!=-1; i++){
		read(s, &frame, sizeof(frame));
		time(&timer);

		if( frame.can_id == GPS_ALTITUDE_TIME_CAN_ID  && !timeSet)
		{
			timer = getTime(frame.data);
			timeSet = true;
			if(stime(&timer) == 0)
			{
				std::cout << "Set the time" << std::endl;
			}
		}

		frameHolder f(frame, timer);

		radioQueue.push(f);
		fileQueue.push(f);
		//std::cout << "CAN Frame id: " << frame.can_id  <<std::endl;
	}
	return 0;

}

