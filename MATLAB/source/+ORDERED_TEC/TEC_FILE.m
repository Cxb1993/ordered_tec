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
            if nargin==0
                obj.FilePath = '.';
                obj.FileName = 'untitled_file';
                obj.Title = 'untitled';
                obj.FileType = 0;
            elseif nargin==1
                if(isa(varargin{1},'numeric') && isscalar(varargin{1}) && mod(varargin{1},1)==0)
                        obj(varargin{1}) = ORDERED_TEC.TEC_FILE;
                else
                    ME = MException('TEC_FILE:TypeWrong', 'constructor type wrong');
                    throw(ME);
                end
            else
                ME = MException('TEC_FILE:NArgInWrong', 'too many input arguments');
                throw(ME);
            end
        end
    end
    
    methods (Access = protected)

    end
    
end

