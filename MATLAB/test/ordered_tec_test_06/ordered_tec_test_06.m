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
u = sin(x).*cos(y);
%%
tec_file = TEC_FILE;
tec_file.FileName = 'test_06';
try
    tec_file = tec_file.write_plt();
catch ME
    disp(ME.message)
end
tec_file.Variables = {'x','y','u'};
try
    tec_file = tec_file.write_plt();
catch ME
    disp(ME.message)
end
tec_file.Zones = TEC_ZONE;
tec_file.Zones.ZoneName = 'test_06_zone';
try
    tec_file = tec_file.write_plt();
catch ME
    disp(ME.message)
end
tec_file.Zones.Data = {x,y};
try
    tec_file = tec_file.write_plt();
catch ME
    disp(ME.message)
end
tec_file.Zones.Data = {x,[],[]};
try
    tec_file = tec_file.write_plt();
catch ME
    disp(ME.message)
end
tec_file.Zones.Data = {x,y,rand(4,4,4,4)};
try
    tec_file = tec_file.write_plt();
catch ME
    disp(ME.message)
end
v = u;
v(1) = nan;
tec_file.Zones.Data = {x,y,v};
try
    tec_file = tec_file.write_plt();
catch ME
    disp(ME.message)
end
tec_file.Zones.Data = {x,y,u(1)};
try
    tec_file = tec_file.write_plt();
catch ME
    disp(ME.message)
end
%%
tec_file.Zones.Data = {x,y,u};
tec_file.Zones.Skip = [-1,-1,2];
tec_file.Zones.Begin = [1,1,1.5];
tec_file.Zones.EEnd = [-1,0,0];
try
    tec_file = tec_file.write_plt();
catch ME
    disp(ME.message)
end
tec_file.Zones.Skip = [1,2,1];
try
    tec_file = tec_file.write_plt();
catch ME
    disp(ME.message)
end
tec_file.Zones.Begin = [10,3,1];
try
    tec_file = tec_file.write_plt();
catch ME
    disp(ME.message)
end
tec_file.Zones.EEnd = [10,5,2];
try
    tec_file = tec_file.write_plt();
catch ME
    disp(ME.message)
end
tec_file.Zones.EEnd = [10,5,0];
%%
tec_file = tec_file.write_plt();
%%
try
    [Real_Max,Real_Dim,noskip,noexc] = tec_file.Zones.gather_real_size(4);
catch ME
    disp(ME.message)
end
[Real_Max,Real_Dim,noskip,noexc] = tec_file.Zones.gather_real_size();
%%
clear ME