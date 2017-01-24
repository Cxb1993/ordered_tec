% test of function ordered_tec

% lmy 2016.2.22 0.9beta
%% clean
clear
close all
clc
%% tec_file
tec_file.FileName = 'test_01';
tec_file.Title = 'test_01';
tec_file.Variables = {'x','y','z','w'};
tec_file.FileType = 0;
tec_file.Auxiliary{1} = {'Auxiliary1','Auxiliary_test_1_ds'};
tec_file.Auxiliary{2} = {'Auxiliary2',num2str(3.14)};
%% tec_zone_1
[x,y,z] = meshgrid(0:0.1:3,-1:0.1:3,-2:0.1:3);
w = x+y.*z;
tec_zone(1).ZoneName = 'zone1';
tec_zone(1).StrandId = -1;
tec_zone(1).SolutionTime = 0;
tec_zone(1).Data = {x,y,z,w};
% tec_zone(1).Skip = [2,3,2];
% tec_zone(1).Begin = [4,5,5];
% tec_zone(1).EEnd = [2,3,3];
tec_zone(1).Auxiliary{1} = {'Auxiliary1','Auxiliary_test_1'};
tec_zone(1).Auxiliary{2} = {'Auxiliary2',num2str(3.14)};
%% tec_zone_2
[x,y,z] = meshgrid(3:0.1:5,-2:0.1:3,-2:0.1:3);
w = x+y.*z;
tec_zone(2).ZoneName = 'zone2';
tec_zone(2).StrandId = -1;
tec_zone(2).SolutionTime = 0;
tec_zone(2).Data = {x,y,z,w};
% tec_zone(2).Skip = [2,3,2];
% tec_zone(2).Begin = [4,5,5];
% tec_zone(2).EEnd = [2,3,3];
tec_zone(2).Auxiliary{1} = {'Auxiliary1','Auxiliary_test_1'};
tec_zone(2).Auxiliary{2} = {'Auxiliary2',num2str(3.14)};
%% ordered_tec
ordered_tec(tec_file,tec_zone,7);