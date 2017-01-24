function ordered_tec(tec_file,tec_zone,echo)
% ORDERED_TEC(tec_file,tec_zone,echo)
% write ordered data as a plt file for tecplot
%
% tec_file[in]: struct to describe tecplot file
% the member of tec_file:
% + [FileName]    string    (untitled_file)
% + [Title]    string    (untitled)
% + Variables    cell(1*N {variablenames})
% + [FileType]    integer(0-full 1-grid 2-solution)    (0)
% + [Auxiliary]    cell(n*2 {name,value})    (empty)
%
% tec_zone[in]: struct array to describ zone
% the member of tec_zone:
% + [ZoneName]   string    (untitled_zone)
% + [StrandId]    integer    (-1)
% + [SolutionTime]    double number    (0)
% + Data    cell(1*N {Data})
% + [Skip]    array(1*3)    ([1,1,1])
% + [Begin]    array(1*3)    ([1,1,1])
% + [EEnd]    array(1*3)    ([0,0,0])
% + [Auxiliary]    cell(n*2 {name,value})    (empty)
%
% echo[in]: integer to describ report leval, 1-7

% lmy 2016.2.22 0.9beta
%% set default value and check value
% for tec_file
if ~isfield(tec_file,'FileName')
	tec_file.FileName = 'untitled_file';
end
if ~isfield(tec_file,'Title')
	tec_file.Title = 'untitled';
end
if ~isfield(tec_file,'Variables')
	ME = MException('ORDERTEC:MissingInput', ...
        'file [%s]: tec_file.Variables is unset',tec_file.FileName);
	throw(ME);
end
if ~isfield(tec_file,'FileType')
	tec_file.FileType = 0;
end
% for tec_zone
for zone_n = 1:length(tec_zone)
	if ~isfield(tec_zone(zone_n),'ZoneName') || isempty(tec_zone(zone_n).ZoneName)
		tec_zone(zone_n).ZoneName = 'untitled_zone';
	end
	if ~isfield(tec_zone(zone_n),'StrandId') || isempty(tec_zone(zone_n).StrandId)
		tec_zone(zone_n).StrandId = -1;
	end
	if ~isfield(tec_zone(zone_n),'SolutionTime') || isempty(tec_zone(zone_n).SolutionTime)
		tec_zone(zone_n).SolutionTime = 0;
	end
	if ~isfield(tec_zone(zone_n),'Data') || isempty(tec_zone(zone_n).Data)
		ME = MException('ORDERTEC:MissingInput', ...
            'zone [%s]: zone.Data is empty',tec_zone(zone_n).ZoneName);
		throw(ME);
	end
	if length(tec_zone(zone_n).Data)~=length(tec_file.Variables)
		ME = MException('ORDERTEC:ErrorVariable', ...
            'zone [%s]: zone.Data is not correspond to tec_file.Variables',tec_zone(zone_n).ZoneName);
		throw(ME);
	end
	data_size = size(tec_zone(zone_n).Data{1});
	for da = tec_zone(zone_n).Data
		if ~isequal(size(da{1}),data_size)
			ME = MException('ORDERTEC:ErrorData', ...
                'zone [%s]: data size is not equal',tec_zone(zone_n).ZoneName);
			throw(ME);
		end
		unvalid = isinf(da{1}) | isnan(da{1});
		unvalid = any(unvalid(:));
		if unvalid
			ME = MException('ORDERTEC:ErrorData', ...
                'zone [%s]: data is not valid (has nan or inf)',tec_zone(zone_n).ZoneName);
			throw(ME);
		end
	end
	if ~isfield(tec_zone(zone_n),'Skip') || isempty(tec_zone(zone_n).Skip)
		tec_zone(zone_n).Skip = [1,1,1];
	end
	if ~isfield(tec_zone(zone_n),'Begin') || isempty(tec_zone(zone_n).Begin)
		tec_zone(zone_n).Begin = [1,1,1];
	end
	if ~isfield(tec_zone(zone_n),'EEnd') || isempty(tec_zone(zone_n).EEnd)
		tec_zone(zone_n).EEnd = [0,0,0];
	end
end
%% write data
if echo>1
	fprintf('#### creat file %s.plt ####\n',tec_file.FileName);
end
fid = fopen([tec_file.FileName,'.plt'],'wb');
try
	% I    HEADER SECTION
	% i    Magic number, Version number
	fwrite(fid,'#!TDV112','char*1');% 8 Bytes, exact characters "#!TDV112". Version number follows the "V" and consumes the next 3 characters (for example: "V75", "V101")
	% ii   Integer value of 1
	fwrite(fid,1,'int32');% This is used to determine the byte order of the reader, relative to the writer
	% iii  Title and variable names
	fwrite(fid,tec_file.FileType,'int32');% FileType 0 = FULL, 1 = GRID, 2 = SOLUTION
	fwrite(fid,s2i(tec_file.Title),'int32');% The TITLE
	fwrite(fid,length(tec_file.Variables),'int32');% Number of variables (NumVar) in the datafile
	fwrite(fid,s2i(tec_file.Variables),'int32');% Variable names
	% iv   Zones
	for zone = tec_zone
		fwrite(fid,299.0,'float32');% Zone marker. Value = 299.0
		fwrite(fid,s2i(zone.ZoneName),'int32');% Zone name
		fwrite(fid,-1,'int32');% ParentZone
		fwrite(fid,zone.StrandId,'int32');% StrandID
		fwrite(fid,zone.SolutionTime,'float64');% Solution time
		fwrite(fid,-1,'int32');% Not used. Set to -1
		fwrite(fid,0,'int32');% ZoneType 0=ORDERED
		fwrite(fid,0,'int32');% Specify Var Location. 0 = Don't specify, all data is located at the nodes
		fwrite(fid,0,'int32');% Are raw local 1-to-1 face neighbors supplied? (0=FALSE 1=TRUE) ORDERED and FELINESEG zones must specify 0
		fwrite(fid,0,'int32');% Number of miscellaneous user-defined face neighbor connections (value >= 0)
		fwrite(fid,real_ijk(size(zone.Data{1}),zone.Skip,zone.Begin,zone.EEnd),'int32');% IMax,JMax,KMax
		if isfield(zone,'Auxiliary')
			for au = zone.Auxiliary
				fwrite(fid,1,'int32');% Auxiliary name/value pair to follow
				fwrite(fid,s2i(au{1}{1}),'int32');% name string
				fwrite(fid,0,'int32');% Auxiliary Value Format (Currently only allow 0=AuxDataType_String)
				fwrite(fid,s2i(au{1}{2}),'int32');% Value string
			end
		end
		fwrite(fid,0,'int32');% No more Auxiliary name/value pairs
	end
	% ix Dataset Auxiliary data
	if isfield(tec_file,'Auxiliary')
		for au = tec_file.Auxiliary
			fwrite(fid,799.0,'float32');% DataSetAux Marker
			fwrite(fid,s2i(au{1}{1}),'int32');% Text for Auxiliary "Name"
			fwrite(fid,0,'int32');% Auxiliary Value Format (Currently only allow 0=AuxDataType_String)
			fwrite(fid,s2i(au{1}{2}),'int32');% Text for Auxiliary "Value"
		end
	end
	% EOHMARKER, value=357.0
	fwrite(fid,357.0,'float32');
	if echo>1
		fprintf('--   write head section   --\n');
	end
	% II   DATA SECTION
	% i    For both ordered and fe zones
	s_f = 0;
	zone_n = 0;
	for zone = tec_zone
		fwrite(fid,299.0,'float32');% Zone marker. Value = 299.0
		zone_n = zone_n + 1;
		if echo>2
			fprintf('--   write zone %i: %s   --\n',zone_n,zone.ZoneName);
		end
		val_n =0;
		for val = zone.Data
			val_n = val_n + 1;
			try
				fwrite(fid,gettype(val{1}),'int32');% Variable data format
			catch t
				ME = MException('OT:TypeError','zone [%s] var[%s]: class %s is not support',zone.ZoneName,tec_file.Variables{val_n},t.message);
				throw(ME);
			end
		end
		fwrite(fid,0,'int32');% Has passive variables: 0 = no
		fwrite(fid,0,'int32');% Has variable sharing 0 = no
		fwrite(fid,-1,'int32');% Zero based zone number to share connectivity list with (-1 = no sharing)
		try
			buf = cellfun(@(x)makebuf(x,zone.Skip,zone.Begin,zone.EEnd), ...
                zone.Data,'UniformOutput',false);
		catch
			ME = MException('ORDERTEC:ErrorVariable', ...
                'zone [%s]: one of Skip or Begin or EEnd is error',zone.ZoneName);
			throw(ME);
		end
		for val = buf
			min_buf=min(val{1}(:));
			max_buf=max(val{1}(:));
			fwrite(fid,min_buf,'float64');% Min value
			fwrite(fid,max_buf,'float64');% Max value
		end
		if echo>2
			fprintf('..   write variables: ');
		end
		val_n =0;
		s_z = 0;
		for val = buf
			val_n = val_n +1;
			v = val{1};
			if echo>2
				fprintf('%s ',tec_file.Variables{val_n});
			end
			fwrite(fid,v,class(v));% Zone Data. Each variable is in data format as specified above
			vname=@(x)(inputname(1));
			ts = whos(vname(v));
			ts = ts.bytes/1024/1024;
			s_z = s_z + ts;
		end
		s_f = s_f + s_z;
		if echo>2
			fprintf('  ..\n');
		end
		data_dim = ndims(zone.Data{1});
		data_size = size(zone.Data{1});
		if echo>4
			fprintf('     zone size: %.2fMB\n',s_z);
		end
		if echo>3
			fprintf('     IMax=%i',data_size(1));
			if data_dim>1
				fprintf(' JMax=%i',data_size(2));
				if data_dim>2
					fprintf(' KMax=%i',data_size(3));
				end
			end
			fprintf('\n');
		end
		if echo>5
			fprintf('     ISkip=%i',zone.Skip(1));
			if data_dim>1
				fprintf(' JSkip=%i',zone.Skip(2));
				if data_dim>2
					fprintf(' KSkip=%i',zone.Skip(3));
				end
			end
			fprintf('\n');
		end
		if echo>6
			fprintf('     IBegin=%i IEnd=%i\n',zone.Begin(1),zone.EEnd(1));
			if data_dim>1
				fprintf('     JBegin=%i JEnd=%i\n',zone.Begin(2),zone.EEnd(2));
				if data_dim>2
					fprintf('     KBegin=%i KEnd=%i\n',zone.Begin(3),zone.EEnd(3));
				end
			end
		end
		if echo>3 && zone.StrandId~=-1
			fprintf('     StrandId=%i SolutionTime=%f\n',zone.StrandId,zone.SolutionTime);
		end
		if echo>1
			fprintf('--   write zone %i: %s   --\n',zone_n,zone.ZoneName);
		end
	end
	if echo>4
		fprintf('     file size: %.2fMB\n',s_f);
	end
	if echo>0
		fprintf('#### creat file %s.plt ####\n',tec_file.FileName);
	end
catch ME
	fclose(fid);
	delete([tec_file.FileName,'.plt']);
	rethrow(ME);
end
fclose(fid);
end

function i32 = s2i(s)
% convert cell(1*N) of string to a row integer array which stands for the
% string's ascii, and end with zero

if isa(s,'cell')
	i32 = cellfun(@(x)([int32(x),0]),s,'UniformOutput',false);
	i32 = reshape(i32,1,[]);
	i32 = cell2mat(i32);
else
	i32 = [int32(s),0];
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