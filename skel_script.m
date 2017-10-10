load('EXACTCase22_skel_graph.mat'); %load the mat file
w = size(skel,1);
l = size(skel,2);
h = size(skel,3);
[x,y,z] = ind2sub([w,l,h],find(skel ~= 0)); %just retains the different from zero
% store skeleton in skel.txt
fid = fopen('skel.txt','w');
fprintf(fid, '%d %d %d\n', [x y z]');
fclose(fid);