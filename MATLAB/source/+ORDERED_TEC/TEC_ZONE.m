classdef TEC_ZONE < ORDERED_TEC.TEC_ZONE_BASE
    %UNTITLED7 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        Data;
        Echo_Mode; %zone_head, zone_end, variable, max_real, max_org, skip, begin & end, stdid & soltime, size
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
                obj.Echo_Mode = 'brief';
            elseif nargin==1
                if isempty(varargin{1})
                    ME = MException('TEC_ZONE:TypeWrong', 'input of TEC_ZONE constructor is empty');
                    throw(ME);
                end
                if isa(varargin{1},'numeric')
                    if isequal(mod(varargin{1},1),zeros(size(varargin{1})))
                        obj = repmat(ORDERED_TEC.TEC_ZONE,varargin{1});
                    else
                        ME = MException('TEC_ZONE:TypeWrong', 'input of TEC_ZONE constructor must be a positive integer');
                        throw(ME);
                    end
                else
                    ME = MException('TEC_ZONE:TypeWrong', 'TEC_ZONE constructor type wrong (%s)',class(varargin{1}));
                    throw(ME);
                end
            else
                ME = MException('TEC_ZONE:NArgInWrong', 'TEC_ZONE constructor too many input arguments');
                throw(ME);
            end
        end
        
        function obj = set.Echo_Mode(obj, zone_mode)
            if islogical(zone_mode)
                if isequal(size(zone_mode),[1,9])
                    obj.Echo_Mode = zone_mode;
                elseif isequal(size(zone_mode),[9,1])
                    obj.Echo_Mode = zone_mode';
                else
                    ME = MException('TEC_ZONE:InputWrong', 'echo_mode code size wrong');
                    throw(ME);
                end
            elseif ischar(zone_mode)
                if strcmp(zone_mode,'brief')
                    obj.Echo_Mode = logical([1,0,0,1,0,0,0,0,0]);
                elseif strcmp(zone_mode,'full')
                    obj.Echo_Mode = true(1,9);
                elseif strcmp(zone_mode,'simple')
                    obj.Echo_Mode = logical([1,0,0,0,0,0,0,0,0]);
                elseif strcmp(zone_mode,'none')
                    obj.Echo_Mode = false(1,9);
                elseif strcmp(zone_mode,'leave')
                else
                    ME = MException('TEC_ZONE:InputWrong', 'echo_mode code string wrong ("%s")',zone_mode);
                    throw(ME);
                end
            else
                ME = MException('TEC_ZONE:TypeWrong', 'echo_mode type wrong (%s)',class(zone_mode));
                throw(ME);
            end
        end
        
        function [Real_Max,Real_Dim,noskip,noexc] = gather_real_size(obj,n)
            if nargin==1
                [Real_Max,Real_Dim,noskip,noexc] = obj.gather_real_size(1);
            elseif nargin==2
                if numel(obj.Data)<n
                    ME = MException('TEC_ZONE:RuntimeError', ...
                        'numel(obj.Data):%i < n:%i',numel(obj.Data),n);
                    throw(ME);
                end
                if any(obj.Skip<=0 | mod(obj.Skip,1)~=0)
                    ME = MException('TEC_ZONE:RuntimeError', ...
                        'the Skip of zone is not possitive integer:[%s]',num2str(obj.Skip));
                    throw(ME);
                end
                if any(obj.Begin<=0 | mod(obj.Begin,1)~=0)
                    ME = MException('TEC_ZONE:RuntimeError', ...
                        'the Begin of zone is not possitive integer:[%s]',num2str(obj.Begin));
                    throw(ME);
                end
                if any(obj.EEnd<0 | mod(obj.EEnd,1)~=0)
                    ME = MException('TEC_ZONE:RuntimeError', ...
                        'the EEnd of zone is not possitive integer:[%s]',num2str(obj.EEnd));
                    throw(ME);
                end
                Real_Max = real_ijk(size(obj.Data{n}),obj.Skip,obj.Begin,obj.EEnd);
                if any(Real_Max<=0)
                    ME = MException('TEC_ZONE:RuntimeError', ...
                        'sum of Begin and EEnd is not smaller than Max:[%s]+[%s]>=[%s]', ...
                        num2str(obj.Begin-1),num2str(obj.EEnd),num2str(real_ijk(size(obj.Data{n}),[1,1,1],[1,1,1],[0,0,0])));
                    throw(ME);
                end
                Real_Dim = find(Real_Max~=1,1,'last');
                noskip = isequal(obj.Skip,[1,1,1]);
                noexc = isequal(obj.Begin,[1,1,1]) && isequal(obj.EEnd,[0,0,0]);
            else
                ME = MException('TEC_ZONE:NArgInWrong', 'too many input arguments');
                throw(ME);
            end
        end
        
    end
    
    methods (Hidden = true)
        function [obj,zone_log] = wrtie_plt_pre(obj,file,zone_log)
            if isempty(obj.Data)
                ME = MException('TEC_ZONE:RuntimeError', ...
                    'FILE[%s]--ZONE[%s]: TEC_ZONE.Data is empty', file.FileName, obj.ZoneName);
                throw(ME);
            end
            if ~isequal(size(obj.Data),size(file.Variables))
                ME = MException('TEC_ZONE:RuntimeError', ...
                    'FILE[%s]--ZONE[%s]: TEC_ZONE.Data is not correspond to TEC_FILE.Variables', file.FileName, obj.ZoneName);
                throw(ME);
            end
            data_size = size(obj.Data{1});
            for kk = 1:numel(obj.Data)
                da = obj.Data{kk};
                if isempty(da)
                    ME = MException('TEC_ZONE:RuntimeError', ...
                        'FILE[%s]--ZONE[%s]: data[%s] is empty', file.FileName, obj.ZoneName,file.Variables{kk});
                    throw(ME);
                end
                if ndims(da)>3
                    ME = MException('TEC_ZONE:RuntimeError', ...
                        'FILE[%s]--ZONE[%s]: the dimension of data[%s] is bigger than 3', file.FileName, obj.ZoneName,file.Variables{kk});
                    throw(ME);
                end
                unvalid = isinf(da) | isnan(da);
                unvalid = any(unvalid(:));
                if unvalid
                    ME = MException('TEC_ZONE:RuntimeError', ...
                        'FILE[%s]--ZONE[%s]: data[%s] is not valid (has nan or inf)', file.FileName, obj.ZoneName, file.Variables{kk});
                    throw(ME);
                end
                if ~isequal(size(da),data_size)
                    ME = MException('TEC_ZONE:RuntimeError', ...
                        'FILE[%s]--ZONE[%s]: data size is not equal', file.FileName, obj.ZoneName);
                    throw(ME);
                end
                try
                    [zone_log.Data(kk).type,zone_log.Data(kk).size_i] = gettype(da);
                catch ME
                    ME = MException('TEC_ZONE:RuntimeError', ...
                        'FILE[%s]--ZONE[%s]: Data[%s] %s', file.FileName, obj.ZoneName, file.Variables{kk}, ME.message);
                    throw(ME);
                end
            end
            zone_log.Data = reshape(zone_log.Data,size(obj.Data));
            
            zone_log.Max = real_ijk(size(obj.Data{1}),[1,1,1],[1,1,1],[0,0,0]);
            zone_log.Dim = find(zone_log.Max~=1,1,'last');
            try
                [zone_log.Real_Max,zone_log.Real_Dim,zone_log.noskip,zone_log.noexc] = obj.gather_real_size();
            catch ME_
                ME = MException('TEC_ZONE:RuntimeError', ...
                        'FILE[%s]--ZONE[%s]: %s', file.FileName, obj.ZoneName, ME_.message);
                throw(ME);
            end
        end
        
        function write_plt_head(obj,fid)
            % iv   Zones
            fwrite(fid,299.0,'float32');% Zone marker. Value = 299.0
            fwrite(fid,ORDERED_TEC.s2i(obj.ZoneName),'int32');% Zone name
            fwrite(fid,-1,'int32');% ParentZone
            fwrite(fid,obj.StrandId,'int32');% StrandID
            fwrite(fid,obj.SolutionTime,'float64');% Solution time
            fwrite(fid,-1,'int32');% Not used. Set to -1
            fwrite(fid,0,'int32');% ZoneType 0=ORDERED
            fwrite(fid,0,'int32');% Specify Var Location. 0 = Don't specify, all data is located at the nodes
            fwrite(fid,0,'int32');% Are raw local 1-to-1 face neighbors supplied? (0=FALSE 1=TRUE) ORDERED and FELINESEG zones must specify 0
            fwrite(fid,0,'int32');% Number of miscellaneous user-defined face neighbor connections (value >= 0)
            fwrite(fid,real_ijk(size(obj.Data{1}),obj.Skip,obj.Begin,obj.EEnd),'int32');% IMax,JMax,KMax
            if ~isempty(obj.Auxiliary)
                for au = obj.Auxiliary
                    fwrite(fid,1,'int32');% Auxiliary name/value pair to follow
                    fwrite(fid,ORDERED_TEC.s2i(au{1}{1}),'int32');% name string
                    fwrite(fid,0,'int32');% Auxiliary Value Format (Currently only allow 0=AuxDataType_String)
                    fwrite(fid,ORDERED_TEC.s2i(au{1}{2}),'int32');% Value string
                end
            end
            fwrite(fid,0,'int32');% No more Auxiliary name/value pairs
        end
        
        function zone_log = write_plt_data(obj,fid,file,zone_log)
            pos_b = ftell(fid);
            fwrite(fid,299.0,'float32');% Zone marker. Value = 299.0
            for val_n = 1:numel(obj.Data)
                try
                    fwrite(fid,zone_log.Data(val_n).type,'int32');% Variable data format
                catch t
                    ME = MException('OT:TypeError','zone [%s] var[%s]: class %s is not support',zone.ZoneName,tec_file.Variables{val_n},t.message);
                    throw(ME);
                end
            end
            fwrite(fid,0,'int32');% Has passive variables: 0 = no
            fwrite(fid,0,'int32');% Has variable sharing 0 = no
            fwrite(fid,-1,'int32');% Zero based zone number to share connectivity list with (-1 = no sharing)
            buf = cellfun(@(x)makebuf(x,obj.Skip,obj.Begin,obj.EEnd), obj.Data,'UniformOutput',false);
            for kk = 1:numel(buf)
                val = buf{kk};
                min_buf=min(val(:));
                max_buf=max(val(:));
                fwrite(fid,min_buf,'float64');% Min value
                fwrite(fid,max_buf,'float64');% Max value
                zone_log.Data(kk).min = min_buf;
                zone_log.Data(kk).max = max_buf;
            end
            
            if obj.Echo_Mode(4)
                echobuf = sprintf('     Dim = %i   Real_Max = [',zone_log.Real_Dim);
                for dd = 1:zone_log.Real_Dim
                    echobuf = [echobuf,' ',num2str(zone_log.Real_Max(dd))];
                end
                echobuf = [echobuf,' ]'];
                e_l = length(zone_log.Echo_Text)+1;
                zone_log.Echo_Text{e_l} = echobuf;
                fprintf('%s\n',echobuf);
            end
            if obj.Echo_Mode(5)
                echobuf = sprintf('     Org_Dim = %i   Org_Max = [',zone_log.Dim);
                for dd = 1:zone_log.Dim
                    echobuf = [echobuf,' ',num2str(zone_log.Max(dd))];
                end
                echobuf = [echobuf,' ]'];
                e_l = length(zone_log.Echo_Text)+1;
                zone_log.Echo_Text{e_l} = echobuf;
                fprintf('%s\n',echobuf);
            end
            if obj.Echo_Mode(6)
                echobuf = '     Skip = [';
                for dd = 1:3
                    echobuf = [echobuf,' ',num2str(zone_log.Skip(dd))];
                end
                echobuf = [echobuf,' ]'];
                e_l = length(zone_log.Echo_Text)+1;
                zone_log.Echo_Text{e_l} = echobuf;
                fprintf('%s\n',echobuf);
            end
            if obj.Echo_Mode(7)
                echobuf = '     Begin = [';
                for dd = 1:3
                    echobuf = [echobuf,' ',num2str(zone_log.Begin(dd))];
                end
                echobuf = [echobuf,' ]'];
                echobuf = [echobuf,'   EEnd = ['];
                for dd = 1:3
                    echobuf = [echobuf,' ',num2str(zone_log.EEnd(dd))];
                end
                echobuf = [echobuf,' ]'];
                e_l = length(zone_log.Echo_Text)+1;
                zone_log.Echo_Text{e_l} = echobuf;
                fprintf('%s\n',echobuf);
            end
            if obj.Echo_Mode(8) && obj.StrandId~=-1
                echobuf = sprintf('     StrandId = %i SolutionTime = %e',obj.StrandId,obj.SolutionTime);
                e_l = length(zone_log.Echo_Text)+1;
                zone_log.Echo_Text{e_l} = echobuf;
                fprintf('%s\n',echobuf);
            end
            
            if obj.Echo_Mode(3)
                echobuf = '     write variables:';
                fprintf('%s',echobuf);
            end
            for val_n = 1:numel(buf)
                val = buf{val_n};
                if obj.Echo_Mode(3)
                    echobuf = [echobuf,' <',file.Variables{val_n},'>'];
                    fprintf(' <%s>',file.Variables{val_n});
                end
                zone_log.Data(val_n).file_pt = ftell(fid);
                fwrite(fid,val,class(val));% Zone Data. Each variable is in data format as specified above
            end
            if obj.Echo_Mode(3)
                e_l = length(zone_log.Echo_Text)+1;
                zone_log.Echo_Text{e_l} = echobuf;
                fprintf('\n');
            end
            
            pos_e = ftell(fid);
            zone_log.Size = (pos_e-pos_b)/1024/1024;
            if obj.Echo_Mode(9)
                echobuf = sprintf('     zone size: %.1f MB',zone_log.Size);
                e_l = length(zone_log.Echo_Text)+1;
                zone_log.Echo_Text{e_l} = echobuf;
                fprintf('%s\n',echobuf);
            end
        end
        
    end
    
end

function rijk = real_ijk(ijk,skip,begin,eend)
% calculate real IJK from an array with its Skip, Begin and EEnd

if length(ijk)==2
    ijk = [ijk,1];
end
begin = begin-[1,1,1];
rijk=(ijk-begin-eend)./skip;
rijk=floor(rijk);
rijk(mod(ijk-begin-eend,skip)~=0)=rijk(mod(ijk-begin-eend,skip)~=0)+1;
end

function buf = makebuf(data,skip,begin,eend)
% make buffer data from an array with its Skip, Begin and EEnd

if isequal(skip,[1,1,1]) && isequal(begin,[1,1,1]) && isequal(eend,[0,0,0])
    buf = data;
else
    buf=data(begin(1):skip(1):end-eend(1), ...
        begin(2):skip(2):end-eend(2), ...
        begin(3):skip(3):end-eend(3));
end
if isempty(buf)
    ME = MException('TEC_ZONE:ErrorVariable','one of Skip or Begin or EEnd is error');
    throw(ME);
end
end

function [ty,si] = gettype(data)
% get the data type (number) and its size

t=class(data);
switch t
    case 'single' %Float
        ty = 1;
        si = 4;
    case 'double' %Double
        ty = 2;
        si = 8;
    case 'int32'  %LongInt
        ty = 3;
        si = 4;
    case 'int16'  %ShortInt
        ty = 4;
        si = 2;
    case 'int8'   %Byte
        ty = 5;
        si = 1;
    otherwise
        ME = MException('TEC_ZONE:TypeError','class(%s) do not supported',t);
        throw(ME);
end
end
