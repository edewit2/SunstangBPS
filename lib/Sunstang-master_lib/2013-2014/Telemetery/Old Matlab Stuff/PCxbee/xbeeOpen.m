function [ s ] = xbeeOpen( serialName )
%XBEEOPEN serial is the comport the xbee is on
%   Detailed explanation goes here

s = serial(serialName);
s.Terminator ='CR'; %Xbee expects this at the end of each line
fopen(s);

end

