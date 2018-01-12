% This script writes the .mat files for the knn test into text files, for
% the MPI program toy read.
% 
% Author: Savvas Sampaziotis
% 

datasets = {"mnist_train", "mnist_train_svd"};

for f = 1:2
    
    fileName = datasets{f};
    
    load( strcat('./raw_data/',fileName ,'.mat'));
    
    [N, D] = size(train_X);
    % For testing purposes only
%     N=15;
%     D = 3;
    n=1:N;  d = 1:D;

    joinData = [train_X(n,d), train_labels(n)];
    header = [N,D];
    
    outputFile = strcat('./formatted_data/',fileName, '.txt');
    
    dlmwrite(outputFile, header, ...
        'delimiter','\t', ...
        'newline','pc');
    dlmwrite(outputFile, joinData , ...
        'delimiter','\t', ...
        'newline','pc', ...
        '-append', ...
        'precision', 16);
end

% FIle Format:
% N D  # Header       # D+1
% x11 x12 x13 ... x1D label1  # Main Data:  [datapoint label]
% x21 x22 x23 ... x2D label2
% x31 x32 x33 ... x3D label3
%   ...
% xN1 xN2 xN3 ... xND labelN