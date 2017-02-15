%%
clear
close all
clc
%%
try
    xmlfile = xmlread('test_01.xml');
catch
    printf('error');
end
%%
zone = xmlfile.getElementsByTagName('Zones').item(0).getElementsByTagName('Zone').item(0);
N = zone.getElementsByTagName('Real_Max').item(0);
N = [str2double(N.getAttribute('I')),str2double(N.getAttribute('J')),str2double(N.getAttribute('K'))];
Dim = str2double(zone.getElementsByTagName('Real_Dim').item(0).getTextContent);
datas = zone.getElementsByTagName('Datas').item(0).getElementsByTagName('Data');
for varnum = 1:datas.getLength
    file_pt(varnum) = str2double(datas.item(varnum-1).getAttribute('file_pt'));
end
clear xmlfile zone datas 
%%
fid = fopen('simple_test.plt','rb');
try
    for k = 1:varnum
        fseek(fid,file_pt(k),'bof');
        x{k} = fread(fid,N(1:Dim),'double');
    end
catch
    fclose(fid);
end
fclose(fid);
clear ans fid
%%
figure
contourf(x{1},x{2},x{3})
xlabel('x')
ylabel('y')
axis equal
