classdef TEC_FILE < ORDERED_TEC.TEC_FILE_BASE
    %UNTITLED6 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        Zones;
        Echo_Mode;
        last_log;
    end
    
    methods
        function obj = TEC_FILE(varargin)
            if nargin == 0
                obj.FilePath = '.';
                obj.FileName = 'untitled_file';
                obj.Title = 'untitled';
                obj.FileType = 0;
                obj.Echo_Mode = 'brief';
            elseif nargin == 1
                if isa(varargin{1},'numeric') && isequal(mod(varargin{1},1),zeros(size(varargin{1})))
                    obj = repmat(ORDERED_TEC.TEC_FILE,varargin{1});
                else
                    ME = MException('TEC_FILE:TypeWrong', 'constructor type wrong');
                    throw(ME);
                end
            else
                ME = MException('TEC_FILE:NArgInWrong', 'too many input arguments');
                throw(ME);
            end
        end
        
        function obj = set.Echo_Mode(obj, file_mode)
            if islogical(file_mode) && ( isequal(size(file_mode),[1,7]) || isequal(size(file_mode),[7,1]) )
                obj.Echo_Mode = file_mode;
            elseif ischar(file_mode)
                if strcmp(file_mode,'brief')
                    obj.Echo_Mode = logical([0,1,0,0,1,1,1]);
                elseif strcmp(file_mode,'full')
                    obj.Echo_Mode = true(1,7);
                elseif strcmp(file_mode,'simple')
                    obj.Echo_Mode = logical([0,1,0,0,0,0,1]);
                elseif strcmp(file_mode,'none')
                    obj.Echo_Mode = false(1,7);
                elseif strcmp(file_mode,'leave')
                else
                    ME = MException('TEC_FILE:InputWrong', 'echo_mode code string wrong');
                    throw(ME);
                end
            else
                ME = MException('TEC_FILE:TypeWrong', 'echo_mode type wrong');
                throw(ME);
            end
        end
        
        function obj = set_echo_mode(obj, file_mode, zone_mode)
            if nargin == 2
                obj.Echo_Mode = file_mode;
            elseif nargin == 3
                obj.set_echo_mode(file_mode);
                for kk=1:numel(obj.Zones)
                    obj.Zones(kk).Echo_Mode = zone_mode;
                end
            else
                ME = MException('TEC_FILE:NArgInWrong', 'too many or too few input arguments');
                throw(ME);
            end
        end
    end
    
    methods (Access = protected)
    end
    
end

