%
% Author: Savvas Sampaziotis
%   Purpose of this script is to read the output binary of the mpi_knn.c
%   program and compare with the matlab calculaton.
%
clear;
load('knn_result.mat');

fileID = fopen('../knn_indeces', 'r');
N = 10;
K = 3;

for i=1:N
    for k=1:K
        
        A(i,k) = fread( fileID, 1 ,'int32',4);
        B(i,k) = fread( fileID, 1 ,'double');
        
    end
end
fclose(fileID);

errNN = sum(sum(A~=IDX));
distErr = mean(mean((B-DIST.^2).^2));
disp(['Num of wrong neighbors = ',num2str(erNN)]);
disp(['KNN dist MSE = ',num2str(distErr)]);

