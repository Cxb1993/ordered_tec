classdef TEC_ZONE_LOG < ORDERED_TEC.TEC_ZONE_BASE
    %UNTITLED9 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        Max;
        Dim;
        Real_Max;
        Real_Dim;
        noskip;
        noexc;
        
        Size;
        Echo_Text;
        Json_Text;
        Xml_Text;
        
        Data
    end
    
    methods
        function obj = TEC_ZONE_LOG(varargin)
            if nargin==0
            elseif nargin==1
                if isa(varargin{1},'ORDERED_TEC.TEC_ZONE')
                    if isscalar(varargin{1})
                        tec_zone = varargin{1};
                        obj.ZoneName = tec_zone.ZoneName;
                        obj.StrandId = tec_zone.StrandId;
                        obj.SolutionTime = tec_zone.SolutionTime;
                        obj.Skip = tec_zone.Skip;
                        obj.Begin = tec_zone.Begin;
                        obj.EEnd = tec_zone.EEnd;
                        obj.Auxiliary = tec_zone.Auxiliary;
                    else
                        tec_zone_m = varargin{1};
                        obj = repmat(ORDERED_TEC.TEC_ZONE_LOG,size(tec_zone_m));
                        for kk = 1:numel(tec_zone_m)
                            obj(kk) = ORDERED_TEC.TEC_ZONE_LOG(tec_zone_m(kk));
                        end
                    end
                elseif(isa(varargin{1},'numeric') && isscalar(varargin{1}) && mod(varargin{1},1)==0)
                    obj(varargin{1}) = ORDERED_TEC.TEC_ZONE_LOG;
                else
                    ME = MException('TEC_ZONE_LOG:TypeWrong', 'constructor type wrong');
                    throw(ME);
                end
            else
                ME = MException('TEC_ZONE_LOG:NArgInWrong', 'too many input arguments');
                throw(ME);
            end
        end
        
        function write_echo(obj,fid)
            if nargin==1
                fid = fopen([obj.ZoneName,'.txt'],'w');
                if fid==-1
                    ME = MException('TEC_ZONE_LOG:FileError', 'can not open file %s.txt',obj.ZoneName);
                    throw(ME);
                end
            end
            for ss = obj.Echo_Text
                fprintf(fid,'%s\n',ss{1});
            end
        end
        
    end
   
    methods (Access = protected)

    end
    
end

