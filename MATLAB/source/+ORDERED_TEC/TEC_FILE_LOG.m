classdef TEC_FILE_LOG < ORDERED_TEC.TEC_FILE_BASE
    %UNTITLED8 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        Time_Begin;
		Time_End;
		UsingTime;
		Size;
		Error;
		Echo_Text;
		Json_Text;
		Xml_Text;
        Zones;
    end
    
    methods
        function obj = TEC_FILE_LOG(varargin)
            if nargin==0
            elseif nargin==1
                if(isa(varargin{1},'ORDERED_TEC.TEC_FILE'))
                    if(isscalar(varargin{1}))
                        tec_file = varargin{1};
                        obj.FilePath = tec_file.FilePath;
                        obj.FileName = tec_file.FileName;
                        obj.Title = tec_file.Title;
                        obj.Variables = tec_file.Variables;
                        obj.FileType = tec_file.FileType;
                        obj.Auxiliary = tec_file.Auxiliary;
                    else
                        tec_file_m = varargin{1};
                        [m,n] = size(tec_file_m);
                        obj(m,n) = ORDERED_TEC.TEC_FILE_LOG;
                        for mm = 1:m
                            for nn = 1:n
                                obj(mm,nn) = ORDERED_TEC.TEC_FILE_LOG(tec_file_m(mm,nn));
                            end
                        end
                    end
                elseif(isa(varargin{1},'numeric') && isscalar(varargin{1}) && mod(varargin{1},1)==0)
                    obj(varargin{1}) = ORDERED_TEC.TEC_FILE_LOG;
                else
                    ME = MException('TEC_FILE_LOG:TypeWrong', 'constructor type wrong');
                    throw(ME);
                end
            else
                ME = MException('TEC_FILE_LOG:NArgInWrong', 'too many input arguments');
                throw(ME);
            end
        end
    end
    
    methods (Access = protected)

    end
    
end

