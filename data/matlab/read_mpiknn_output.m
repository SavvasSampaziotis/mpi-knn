%
% Author: Savvas Sampaziotis
%   Purpose of this script is to read the output binary of the mpi_knn.c
%   program and compare with the matlab calculaton.
%
clear;
load('knn_result.mat');

%knn_example

fileID = fopen('../../mpi_knn_output', 'r');
[N,K] = size(IDX);
A = zeros(N,K);
B = A;

disp('Reading mpi_knn output file. Please wait...');
for i=1:N
    for k=1:(K)
        
        A(i,k) = fread( fileID, 1 ,'int32',4);
        B(i,k) = fread( fileID, 1 ,'double');
        
    end
end
fclose(fileID);

disp('Comparing matlab output with Savvas-MPI output...');

% A = A(:,2:end);
% B = B(:,2:end);

errNN = sum(sum(A~=IDX));
distErr = mean(mean((B-DIST.^2).^2));
disp(['Num of wrong neighbors = ',num2str(errNN)]);
disp(['KNN dist MSE = ',num2str(distErr)]);

