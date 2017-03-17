classdef xbee
    %UNTITLED Summary of this class goes here
    %   Detailed explanation goes here
    
    properties(Access = private)
        s
    end
    
    methods(Static)
        function out = getInstance()
           persistent xbeeObj;
           if isempty(xbeeObj)
               disp('constructing')
               xbeeObj = xbee();
           end

           out = xbeeObj;
          
        end
    
    end
    
    methods(Access = private)
        function obj = xbee()
            obj.s = serial('com4');
            obj.s.Terminator = 'E'; %Xbee expects this at the end of each line
        end        
    end
    
    methods
        function delete(obj)
        %%This is the descontructor, you should never call it
        %
           fclose(obj.s); 
        end 
        
        function write(obj, val)
        %% This will write 'val' to the xbee
        
            fwrite(obj.s, val); 
        end
        
        function setReadCallback(obj, func)
            obj.s.BytesAvailableFcn = func;
            fopen(obj.s);
        end
        
        
    end
end
