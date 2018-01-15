function [ r, metrics, raw_data ] = parse_jobfile( fileName )
%PARSE_JOBFILE Summary of this function goes here
%   Detailed explanation goes here

%% Parse Data
metrics = [NaN,NaN,NaN,NaN];

fileID = fopen(fileName, 'r');
if(fileID<0)
    disp(['error opening ',fileName]);
    r = 1;
    return
end

D = fscanf(fileID, "%d %d\n");

formatSpec = "[RANK %d]: Comm Time = %lf KNN chunk Time = %lf\n";
A = fscanf(fileID, formatSpec);

formatSpec = "knn-time = %lf";
KnnTime = fscanf(fileID, formatSpec);

fclose(fileID);

%% Re-Format data

N = length(A);
B = zeros(N/3,3);
b = 0;
for i=1:3:N
    b = b + 1;
    B(b,1) = A(i);
    B(b,2) = A(i+1);
    B(b,3) = A(i+2);
end

ExtraCommTime = mean(B(:,2)-B(:,3));

% metrics = struct('mpi', D(1), 'omp', D(2), ...
%     'extracomm', ExtraCommTime, 'knntime', k=KnnTime);

metrics = [D(1), D(2), KnnTime, ExtraCommTime];
r = 0;

raw_data = B;
end

