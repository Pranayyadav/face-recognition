% A sample script, which shows the usage of functions, included in
% PCA-based face recognition system (Eigenface method)
%
% See also: CREATEDATABASE, EIGENFACECORE, RECOGNITION
%
% Original version by Amir Hossein Omidvarnia, October 2007
%                     Email: aomidvar@ece.ut.ac.ir
%
clear
clc
close all

format long e

TrainDatabasePath = '../../train_images/';
TestDatabasePath = '../../test_images/';

TrainFiles = dir(strcat(TrainDatabasePath, '/*.ppm'));
TestFiles = dir(strcat(TestDatabasePath, '/*.ppm'));

% create training database
T = CreateDatabase(TrainDatabasePath);

[m, A, Eigenfaces] = EigenfaceCore(T);
[numPixels, numImages] = size(T);
[~, numFaces] = size(Eigenfaces);

ProjectedImages = Eigenfaces' * A;

% test each image in the test set
num_correct = 0;

for i = 1 : size(TestFiles, 1)
    % perform recognition algorithm
    strtest = strcat(TestDatabasePath, '/', TestFiles(i).name);
    j = Recognition(strtest, m, Eigenfaces, ProjectedImages);

    % print results
    fprintf('test image: \"%s\"\n', TestFiles(i).name);
    fprintf('       PCA: \"%s\"\n', TrainFiles(j).name);
    fprintf('\n');

    % determine whether the algorithm was correct
    % assumes that filename is formatted as '{class}_{index}.ppm'
    tokens_train = strsplit(TrainFiles(j).name, '_');
    tokens_test = strsplit(TestFiles(i).name, '_');

    if strcmp(tokens_train{1}, tokens_test{1})
        num_correct = num_correct + 1;
    end
end

success_rate = num_correct / size(TestFiles, 1) * 100;

fprintf('%d / %d matched, %.2f%%\n', num_correct, size(TestFiles, 1), success_rate);
