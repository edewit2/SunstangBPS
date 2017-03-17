#pragma once
#include <linux/can.h>
#include <time.h>
#include <iomanip>
#include <sstream>
#include <string>

class frameHolder
{
	private:
		can_frame frame;
		time_t time;
	public:
		frameHolder(can_frame cf, time_t t):frame(cf), time(t){ }
		std::string toString(){
			std::stringstream ss("");

			ss << time << ',';  // insert time;
			ss << std::uppercase << std::hex << frame.can_id << ',';
 
			for(int i = 0; i < (int)frame.can_dlc; i++)
			{
				  ss << std::hex << std::setfill('0') << std::setw(2) << (int)frame.data[i]; 
			} 

			return ss.str();
		}
};