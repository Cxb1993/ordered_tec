classdef TEC_ZONE < ORDERED_TEC.TEC_ZONE_BASE
    %UNTITLED7 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        Data;
        Echo_Mode;
    end
    
    methods
        function obj = TEC_ZONE(varargin)
            if nargin==0
                obj.ZoneName = 'untitled_zone';
                obj.StrandId = -1;
                obj.SolutionTime = 0;
                obj.Skip = [1,1,1];
                obj.Begin = [1,1,1];
                obj.EEnd = [0,0,0];
            elseif nargin==1
                if(isa(varargin{1},'numeric') && isscalar(varargin{1}) && mod(varargin{1},1)==0)
                        obj(varargin{1}) = ORDERED_TEC.TEC_ZONE;
                else
                    ME = MException('TEC_ZONE:TypeWrong', 'constructor type wrong');
                    throw(ME);
                end
            else
                ME = MException('TEC_ZONE:NArgInWrong', 'too many input arguments');
                throw(ME);
            end
        end
    end
    
    methods (Access = protected)
        
    end
    
end

