
%% Parse Data
filename = '../dataout/text';
fileID = fopen(filename, 'r');

D = fscanf(fileID, "%d %d\n");

formatSpec = "[RANK %d]: Comm Time = %lf KNN chunk Time = %lf\n"
A = fscanf(fileID, formatSpec);

formatSpec = "knn-time = %lf"
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

temp = [B(:,1), B(:,2)-B(:,3)];
procNum = max(B(:,1));
ExtraCommTime = zeros(procNum, N/3);
for i=0:procNum
%     ExtraCommTime(i,:) = 0;
    temp( i==temp(:,1), :)
end
ExtraCommTime