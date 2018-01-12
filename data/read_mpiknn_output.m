
clear;

fileID = fopen('../knn_indeces', 'r');

for i=1:15
    for k=1:2
        
        A(i,k) = fread( fileID, 1 ,'int32',4);
        B(i,k) = fread( fileID, 1 ,'double');
        
    end
end

A
B
fclose(fileID);
