% This script writes the .mat files for the knn test into text files, for
% the MPI program toy read.
% 
% Author: Savvas Sampaziotis
% 

datasets = {"mnist_train", "mnist_train_svd"};

for f = 1:2
    
    fileName = datasets{f};
    
    load( strcat('../raw_data/',fileName ,'.mat'));
    
    [N, D] = size(train_X);
    % For testing purposes only
%     N=15;
%     D = 4;
    n=1:N;  d = 1:D;
 
    header = [N,D];
    mainBody = train_X(n,d)';
    mainBody = mainBody(:);
    
    outputFile = strcat('../bin_data/',fileName, '.bin');
    fileID = fopen(outputFile, 'w+');
    
    % Write Head first
    fwrite(fileID, header, 'int32');
    fwrite(fileID, mainBody, 'double');
    % Write Data [x(1,1),x(1,2)...x(1,D), x(2,1) ... x(2,D) ... ];
    
    fclose(fileID);
end

% File Format:
% [N D ]
% header length = 4+4 = 8 Bytes
% Main body length = N*D*8 Bytes
