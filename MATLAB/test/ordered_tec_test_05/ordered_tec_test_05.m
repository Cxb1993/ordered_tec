%%
clear
close all
clc
%%
addpath('../../source')
import ORDERED_TEC.*
%%
x = 0:0.05:2*pi;
[x,y] = meshgrid(x);
u_h = @(t)(sin(x+t).*cos(y+t));
%%
path = 'test_05';
try
    rmdir(path,'s')
catch
end
mkdir(path)
%%
tec_file_g = TEC_FILE;
tec_file_g.FilePath = path;
tec_file_g.FileName = 'test_05_grid';
tec_file_g.FileType = 1;
tec_file_g.Variables = {'x','y'};
tec_file_g.Zones = TEC_ZONE;
tec_file_g.Zones.Data = {x,y};
tec_file_g = tec_file_g.write_plt();
tec_file_g.last_log.write_echo();
tec_file_g.last_log.write_json();
tec_file_g.last_log.write_xml();
%%
tec_file = TEC_FILE;
tec_file.FilePath = path;
tec_file.FileType = 2;
tec_file.Variables = {'u'};
tec_file.Zones = TEC_ZONE;
tec_file.Zones.StrandId = 0;
tec_file = tec_file.set_echo_mode('simple','none');
for kk = 0:20
    tec_file.Zones.SolutionTime = kk;
    tec_file.FileName = sprintf('test_05_%2.2i',kk);
    tec_file.Zones.Data = {u_h(kk/20*2*pi)};
    tec_file = tec_file.write_plt();
    loginf(kk+1) = tec_file.last_log;
end
%%
fid_echo  = fopen('test_05.txt','w');
fid_json  = fopen('test_05.json','w');
fid_xml  = fopen('test_05.xml','w');
fprintf(fid_json,'[\n');
fprintf(fid_xml,'<?xml version="1.0" encoding="UTF-8"?>\n');
fprintf(fid_xml,'<Files>\n');
for kk = 1:length(loginf)
    loginf(kk).write_echo(fid_echo);
    loginf(kk).write_json(1,fid_json);
    if kk~=length(loginf)
        fprintf(fid_json,'\t,\n');
    end
    loginf(kk).write_xml(1,fid_xml);
end
fprintf(fid_json,']\n');
fprintf(fid_xml,'</Files>\n');
fclose(fid_echo);
fclose(fid_json);
fclose(fid_xml);
