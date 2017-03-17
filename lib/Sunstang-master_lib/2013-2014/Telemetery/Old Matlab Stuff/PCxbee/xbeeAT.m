function [ out ] = xbeeAT(s, cmd )
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here

fwrite(s, '+++');
while(s.BytesAvailable == 0)
    pause(0.1);
end
ack = char(fread(s, s.BytesAvailable))';

if ~strcmp(strtrim(ack), 'OK')
    warning('ack');
end
fprintf(s, cmd);


while(s.BytesAvailable == 0)
    pause(0.1);
end
out = strtrim(char(fread(s, s.BytesAvailable))');
  

fprintf(s, 'ATCN')
while(s.BytesAvailable == 0)
    pause(0.1);
end
fread(s, s.BytesAvailable);

end

