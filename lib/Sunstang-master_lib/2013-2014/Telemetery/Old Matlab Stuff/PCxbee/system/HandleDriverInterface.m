function [leftBlinker, rightBlinker, horn, brake, hazards, accel, temp] = HandleDriverInterface( raw )
%HANDLEDRIVERINTERFACE Summary of this function goes here
%   Detailed explanation goes here

leftBlinker = bitget(raw(1), 1);
rightBlinker= bitget(raw(1), 2);
horn =  bitget(raw(1), 3);
brake = bitget(raw(1), 4);
hazards=  bitget(raw(1), 5);

accel = raw(2);

temp =  uint16(raw(3)) * 256 + uint16(raw(4)); %convert the 2 Bytes into a Uint16


end

