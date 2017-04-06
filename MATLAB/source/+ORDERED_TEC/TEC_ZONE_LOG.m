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
                obj.write_echo(fid);
                fclose(fid);
            elseif nargin==2
                for ss = obj.Echo_Text
                    fprintf(fid,'%s\n',ss{1});
                end
            end
        end
        
        function write_json(obj,depth,fid)
            if nargin==1
                depth = 0;
                obj.write_json(depth);
            elseif nargin==2
                fid = fopen([obj.ZoneName,'.json'],'w');
                if fid==-1
                    ME = MException('TEC_ZONE_LOG:FileError', 'can not open file %s.json',obj.ZoneName);
                    throw(ME);
                end
                obj.write_json(depth,fid);
                fclose(fid);
            elseif nargin==3
                for ss = obj.Json_Text
                    fprintf(fid,repmat('\t',1,depth));
                    fprintf(fid,'%s\n',ss{1});
                end
            end
        end
        
        function write_xml(obj,depth,fid)
            if nargin==1
                depth = 0;
                obj.write_xml(depth);
            elseif nargin==2
                fid = fopen([obj.ZoneName,'.xml'],'w');
                if fid==-1
                    ME = MException('TEC_ZONE_LOG:FileError', 'can not open file %s.xml',obj.ZoneName);
                    throw(ME);
                end
                fprintf(fid,'<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n');
                obj.write_xml(depth,fid);
                fclose(fid);
            elseif nargin==3
                for ss = obj.Xml_Text
                    fprintf(fid,repmat('\t',1,depth));
                    fprintf(fid,'%s\n',ss{1});
                end
            end
        end
        
    end
   
    methods (Hidden = true)
        function obj = gen_json(obj)
            obj.Json_Text = [];
            obj.Json_Text{end+1} = '{';
            buf = sprintf('\t"ZoneName" : "%s" ,', obj.ZoneName); obj.Json_Text{end+1} = buf;
            buf = sprintf('\t"StrandId" : %i ,', obj.StrandId); obj.Json_Text{end+1} = buf;
            if obj.StrandId ~= -1
                buf = sprintf('\t"SolutionTime" : %e ,', obj.SolutionTime); obj.Json_Text{end+1} = buf;
            end
            buf = sprintf('\t"Real_Dim" : %i ,', obj.Real_Dim); obj.Json_Text{end+1} = buf;
            buf = sprintf('\t"Org_Max" : [ %i, %i, %i ] ,', obj.Max(1), obj.Max(2), obj.Max(3)); obj.Json_Text{end+1} = buf;
            buf = sprintf('\t"Skip" : [ %i, %i, %i ] ,', obj.Skip(1), obj.Skip(2), obj.Skip(3)); obj.Json_Text{end+1} = buf;
            buf = sprintf('\t"Begin" : [ %i, %i, %i ] ,', obj.Begin(1), obj.Begin(2), obj.Begin(3)); obj.Json_Text{end+1} = buf;
            buf = sprintf('\t"End" : [ %i, %i, %i ] ,', obj.EEnd(1), obj.EEnd(2), obj.EEnd(3)); obj.Json_Text{end+1} = buf;
            buf = sprintf('\t"Real_Max" : [ %i, %i, %i ] ,', obj.Real_Max(1), obj.Real_Max(2), obj.Real_Max(3)); obj.Json_Text{end+1} = buf;
            
            if ~isempty(obj.Auxiliary)
                buf  = sprintf('\t"Auxiliary" : {'); obj.Json_Text{end+1} = buf;
                for kk = 1:length(obj.Auxiliary)
                    buf = sprintf('\t\t"%s" : "%s"',obj.Auxiliary{kk}{1},obj.Auxiliary{kk}{2});
                    obj.Json_Text{end+1} = buf;
                    if kk~= length(obj.Auxiliary)
                        obj.Json_Text{end} = [obj.Json_Text{end},','];
                    end
                end
                buf  = sprintf('\t} ,'); obj.Json_Text{end+1} = buf;
            end
            
            buf = sprintf('\t"Data_type_comment" : "1=Float, 2=Double, 3=LongInt, 4=ShortInt, 5=Byte, 6=Bit" ,'); obj.Json_Text{end+1} = buf;
            buf  = sprintf('\t"Data" : ['); obj.Json_Text{end+1} = buf;
            for kk = 1:numel(obj.Data)
                buf = sprintf('\t\t{ "type":%i, "size_i":%i, "file_pt":%i }',obj.Data(kk).type,obj.Data(kk).size_i,obj.Data(kk).file_pt);
                obj.Json_Text{end+1} = buf;
                if kk~=numel(obj.Data)
                    obj.Json_Text{end} = [obj.Json_Text{end},','];
                end
            end
            buf  = sprintf('\t]'); obj.Json_Text{end+1} = buf;
            
            buf  = sprintf('}'); obj.Json_Text{end+1} = buf;
        end
        
        function obj = gen_xml(obj)
            obj.Xml_Text = [];
            buf = sprintf('<Zone ZoneName="%s">', obj.ZoneName); obj.Xml_Text{end+1} = buf;
            buf = sprintf('\t<ZoneName>%s</ZoneName>', obj.ZoneName); obj.Xml_Text{end+1} = buf;
            buf = sprintf('\t<StrandId>%i</StrandId>', obj.StrandId); obj.Xml_Text{end+1} = buf;
            if obj.StrandId ~= -1
                buf = sprintf('\t<SolutionTime>%le</SolutionTime>', obj.SolutionTime); obj.Xml_Text{end+1} = buf;
            end
            buf = sprintf('\t<Real_Dim>%i</Real_Dim>', obj.Real_Dim); obj.Xml_Text{end+1} = buf;
            buf = sprintf('\t<Org_Max> <I>%i</I> <I>%i</I> <I>%i</I> </Org_Max>', obj.Max(1), obj.Max(2), obj.Max(3)); obj.Xml_Text{end+1} = buf;
            buf = sprintf('\t<Skip> <I>%i</I> <I>%i</I> <I>%i</I> </Skip>', obj.Skip(1), obj.Skip(2), obj.Skip(3)); obj.Xml_Text{end+1} = buf;
            buf = sprintf('\t<Begin> <I>%i</I> <I>%i</I> <I>%i</I> </Begin>', obj.Begin(1), obj.Begin(2), obj.Begin(3)); obj.Xml_Text{end+1} = buf;
            buf = sprintf('\t<End> <I>%i</I> <I>%i</I> <I>%i</I> </End>', obj.EEnd(1), obj.EEnd(2), obj.EEnd(3)); obj.Xml_Text{end+1} = buf;
            buf = sprintf('\t<Real_Max> <I>%i</I> <I>%i</I> <I>%i</I> </Real_Max>', obj.Real_Max(1), obj.Real_Max(2), obj.Real_Max(3)); obj.Xml_Text{end+1} = buf;
            
            if ~isempty(obj.Auxiliary)
                buf  = sprintf('\t<Auxiliary>'); obj.Xml_Text{end+1} = buf;
                for kk = 1:length(obj.Auxiliary)
                    buf = sprintf('\t\t<Auxiliary Name="%s">%s</Auxiliary>',obj.Auxiliary{kk}{1},obj.Auxiliary{kk}{2});
                    obj.Xml_Text{end+1} = buf;
                end
                buf  = sprintf('\t</Auxiliary>'); obj.Xml_Text{end+1} = buf;
            end
            
            buf = sprintf('\t<!--1=Float, 2=Double, 3=LongInt, 4=ShortInt, 5=Byte, 6=Bit-->'); obj.Xml_Text{end+1} = buf;
            buf  = sprintf('\t<Datas>'); obj.Xml_Text{end+1} = buf;
            for kk = 1:numel(obj.Data)
                buf = sprintf('\t\t<Data type="%i" size_i="%i" file_pt="%i"/>',obj.Data(kk).type,obj.Data(kk).size_i,obj.Data(kk).file_pt);
                obj.Xml_Text{end+1} = buf;
            end
            buf  = sprintf('\t</Datas>'); obj.Xml_Text{end+1} = buf;
            
            buf  = sprintf('</Zone>'); obj.Xml_Text{end+1} = buf;
        end
        
    end
    
end

