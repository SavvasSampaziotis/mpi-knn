
[r, metricsB, raw_dataB] = parse_jobfile(...
    '../dataout/sampazio_mpiknn_blockingCLEAN.o4284332');
% raw_dataB = raw_dataB(1:end-1,:); 

[r, metricsNB, raw_dataNB] = parse_jobfile(...
    '../dataout/sampazio_mpiknn.o4284211');

% raw_dataB = raw_dataNB;

for rank=0:3
    commB(rank+1) = mean(raw_dataB( raw_dataB(:,1)==rank, 2));
    knnB(rank+1) = mean(raw_dataB( raw_dataB(:,1)==rank, 3));
    
    commNB(rank+1) = mean(raw_dataNB( raw_dataB(:,1)==rank, 2));
    knnNB(rank+1) = mean(raw_dataNB( raw_dataNB(:,1)==rank, 3));
end

figure(1); clf; hold on;
bar([commB',commNB', commB'+knnB',knnB']); 
xlabel('rank'); ylabel('sec');
legend('Comm-Time [Blocking]','Comm-Time [Non-Blocking]',...
    'Comm+Knn-Time [Blocking]', 'Knn-Time', 'location', 'best');
title('Blocking VS Non-Blocking: n=4|ppn=1');
xticks([1 2 3 4])
xticklabels({'0' '1' '2' '3'})

% bar(commNB); xlabel('rank'); ylabel('sec');

