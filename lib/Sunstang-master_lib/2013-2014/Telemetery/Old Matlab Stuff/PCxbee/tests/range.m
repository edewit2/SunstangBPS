function [data, time] = range(s, numBytes)
%This function will send numBytes Random bytes to the xbee on the serial object "s",  the remote xbee
%should have the din and dout pin tied together.
%
% Data :
% Positive number is the RTT of the byte
% Negative numbers are error codes:
%  -1 means unknown error
%  -2 timeout, the remote xbee was out of range or off
%  -3 data was corrupted in transit.
% strength is the RSSI value in dBm
%   Positive number is error code, same as above


data = ones(numBytes,1)*-1;
%strength = ones(numBytes,1)*-1;
try
for i = 1:numBytes
    num =  uint8(rand(1)*255); %generate a random byte to send
    c = clock;
    fwrite(s, num, 'uint8');
    
    rnum = fread(s, 1, 'uint8');
    rtt = etime(clock, c) ;
    
    
    if num == rnum
        data(i) = rtt;
%        xbcommand(s, '+++')
%        rssi = xbcommand(s, 'ATDB');
%        strength(i) = hex2dec(rssi); %Conversion is from manual
        %disp(['You dun good: RTT: ' num2str(rtt)]);
    elseif strcmp(lastwarn, 'Unsuccessful read: The specified amount of data was not returned within the Timeout period.')
        lastwarn('none')
        data(i) = -2;
 %       strength = 2;
    else
        data(i) = -3;
 %       strength = 3;
        warn('Transmission Error');
    end
end
catch err
    disp err
end