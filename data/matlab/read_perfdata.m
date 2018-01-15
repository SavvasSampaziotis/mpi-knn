
%% Traverse through all the files
k = 1;
filename = {};
for i=7:9
    filename{k} = strcat(...
        '../dataout/sampazio_mpiknn.o428420',num2str(i));
    k = k + 1;
end

for i=10:14
    filename{k} = strcat(...
        '../dataout/sampazio_mpiknn.o42842',num2str(i));
    k = k + 1;
end

k=1;
A = zeros(1,4);
for i=1:length(filename)
    [r, metrics] = parse_jobfile(filename{i});
    if r==0
        A(k,:) = metrics;
        k = k + 1;
    end
end

%% Thread Effect
figure(1); clf;
subplot(2,1,1);
time2 = A(A(:,1)==2,3)/max(time2)*100;
time4 = A(A(:,1)==4, 3)/max(time4)*100;
time4 = [time4(1:end);0];
bar([time2,time4]);
xlabel('Thread Num'); ylabel('Speed-up %');
title('KNN-Time Speed-up Comparison');
legend('2 Nodes', '4 Nodes');
xticks([1 2 3 4])
xticklabels({'1' '2' '4' '8'})

subplot(2,1,2);
time2 = A(A(:,1)==2,3);
time4 = A(A(:,1)==4, 3);
time4 = [time4(1:end);0];
bar([time2,time4]);
xlabel('Thread Num'); ylabel('KNN-Time [sec]');
title('KNN-Time Actual Calc TIme');
legend('2 Nodes', '4 Nodes');
xticks([1 2 3 4])
xticklabels({'1' '2' '4' '8'})

figure(2); clf;
time2 = A(A(:,1)==2,4);
time4 = A(A(:,1)==4, 4);
time4 = [time4(1:end);0];
bar([time2,time4]);
xlabel('Thread Num'); ylabel('Wait-Time [sec]');
title('Average Wait Time');
legend('2 Nodes', '4 Nodes');
xticks([1 2 3 4])
xticklabels({'1' '2' '4' '8'})
