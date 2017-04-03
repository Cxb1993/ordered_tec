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
                obj.Echo_Mode = 'brief';
            elseif nargin==1
                if isa(varargin{1},'numeric') && isequal(mod(varargin{1},1),zeros(size(varargin{1})))
                    obj = repmat(ORDERED_TEC.TEC_ZONE,varargin{1});
                else
                    ME = MException('TEC_ZONE:TypeWrong', 'constructor type wrong');
                    throw(ME);
                end
            else
                ME = MException('TEC_ZONE:NArgInWrong', 'too many input arguments');
                throw(ME);
            end
        end
        
        function obj = set.Echo_Mode(obj, zone_mode)
            if islogical(zone_mode) && ( isequal(size(zone_mode),[1,9]) || isequal(size(zone_mode),[9,1]) )
                obj.Echo_Mode = zone_mode;
            elseif ischar(zone_mode)
                if strcmp(zone_mode,'brief')
                    obj.Echo_Mode = logical([0,0,0,0,0,1,0,0,1]);
                elseif strcmp(zone_mode,'full')
                    obj.Echo_Mode = true(1,9);
                elseif strcmp(zone_mode,'simple')
                    obj.Echo_Mode = logical([0,0,0,0,0,0,0,0,1]);
                elseif strcmp(zone_mode,'none')
                    obj.Echo_Mode = false(1,9);
                elseif strcmp(zone_mode,'leave')
                else
                    ME = MException('TEC_ZONE:InputWrong', 'echo_mode code string wrong');
                    throw(ME);
                end
            else
                ME = MException('TEC_ZONE:TypeWrong', 'echo_mode type wrong');
                throw(ME);
            end
        end
    end
    
    methods (Access = protected)
        
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
    ME = MException('ORDERTEC:ErrorVariable','one of Skip or Begin or EEnd is error');
    throw(ME);
end
end

function ty = gettype(data)
% get the type of the data and its number

t=class(data);
switch t
    case 'single' %Float
        ty = 1;
    case 'double' %Double
        ty = 2;
    case 'int32'  %LongInt
        ty = 3;
    case 'int16'  %ShortInt
        ty = 4;
    case 'int8'   %Byte
        ty = 5;
    otherwise
        ME = MException('OT:TypeError',t);
        throw(ME);
end
end
