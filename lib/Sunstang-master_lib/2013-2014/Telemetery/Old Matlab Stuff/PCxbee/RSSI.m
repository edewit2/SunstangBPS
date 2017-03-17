function [val, time] = RSSI(s)
%RSSI Sends a random byte of info to the a loop back circuit and records
%and append the value to filename;
%   Detailed explanation goes here
    
     fwrite(s, uint8(rand(1)*255), 'uint8');
     fread(s, 1, 'uint8');
     
     pause(2); %because there must be a race condition somewhere.
     
     val = xbeeAT(s, 'ATDB');
     val  = -1*hex2dec(val);
     time = datenum(clock);

end

