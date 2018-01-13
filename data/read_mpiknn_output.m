
clear;
load('knn_result.mat');

fileID = fopen('../knn_indeces', 'r');

for i=1:15
    for k=1:4
        
        A(i,k) = fread( fileID, 1 ,'int32',4);
        B(i,k) = fread( fileID, 1 ,'double');
        
    end
end
fclose(fileID);

A(:,2:end)-IDX
B(:,2:end)-DIST

