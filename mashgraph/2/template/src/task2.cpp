#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include <iostream>
#include <cmath>

#include "classifier.h"
#include "EasyBMP.h"
#include "linear.h"
#include "argvparser.h"
#include "matrix.h"

using CommandLineProcessing::ArgvParser;

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::pair;
using std::make_pair;
using std::cout;
using std::cerr;
using std::endl;


typedef vector<pair<BMP*, int> > TDataSet;
typedef vector<pair<string, int> > TFileList;
typedef vector<pair<vector<float>, int> > TFeatures;

// Load list of files and its labels from 'data_file' and
// stores it in 'file_list'
void LoadFileList(const string& data_file, TFileList* file_list) {
    ifstream stream(data_file.c_str());

    string filename;
    int label;

    int char_idx = data_file.size() - 1;
    for (; char_idx >= 0; --char_idx)
        if (data_file[char_idx] == '/' || data_file[char_idx] == '\\')
            break;
    string data_path = data_file.substr(0,char_idx+1);

    while(!stream.eof() && !stream.fail()) {
        stream >> filename >> label;
        if (filename.size())
            file_list->push_back(make_pair(data_path + filename, label));
    }

    stream.close();
}

// Load images by list of files 'file_list' and store them in 'data_set'
void LoadImages(const TFileList& file_list, TDataSet* data_set) {
    for (size_t img_idx = 0; img_idx < file_list.size(); ++img_idx) {
            // Create image
        BMP* image = new BMP();
            // Read image from file
        image->ReadFromFile(file_list[img_idx].first.c_str());
            // Add image and it's label to dataset
        data_set->push_back(make_pair(image, file_list[img_idx].second));
    }
}

// Save result of prediction to file
void SavePredictions(const TFileList& file_list,
                     const TLabels& labels,
                     const string& prediction_file) {
        // Check that list of files and list of labels has equal size
    assert(file_list.size() == labels.size());
        // Open 'prediction_file' for writing
    ofstream stream(prediction_file.c_str());

        // Write file names and labels to stream
    for (size_t image_idx = 0; image_idx < file_list.size(); ++image_idx)
        stream << file_list[image_idx].first << " " << labels[image_idx] << endl;
    stream.close();
}

Matrix<double> CustomFilter(Matrix<double> image, Matrix<double> kernel) {
    Matrix<double> result = image.deep_copy();

    for (int i = 0; i < int(image.n_rows); i++) {
        for (int j = 0; j < int(image.n_cols); j++) {
            double res = 0;

            for (int k = - int(kernel.n_rows / 2); k <= int(kernel.n_rows / 2); k++) {
                for (int l = - int(kernel.n_cols / 2); l <= int(kernel.n_cols / 2); l++) {
                    if (k + i < 0 || l + j < 0 || k + i >= int(image.n_rows) || l + j >= int(image.n_cols)) { continue; }
                    double val = image(i + k, j + l);
                    val *= kernel(kernel.n_rows / 2 + k, kernel.n_cols / 2 + l);
                    res += val;
                }
            }

            result(i, j) = res;
        }
    }

    return result;
}


Matrix<double> mirror(Matrix<double> image, int radius) {
    Matrix<double> result(image.n_rows + 2 * radius, image.n_cols + 2 * radius);

    for (int i = 0; i < int(image.n_rows); i++) {
        for (int j = 0; j < int(image.n_cols); j++) {
            result(i + radius, j + radius) = image(i, j);
        }
    }
    for (int i = 0; i < radius; i++) {
        for (int j = radius; j < int(image.n_cols) + radius; j ++) {
            result(radius - i - 1, j) = result(radius + i, j);
        }
    }
    for (int i = 0; i < radius; i++) {
        for (int j = radius; j < int(image.n_cols) + radius; j ++) {
            result(radius + image.n_rows + i, j) = result(radius + image.n_rows - i - 1, j);
        }
    }
    for (int i = 0; i < radius; i++) {
        for (int j = 0; j < int(image.n_rows) + radius * 2; j ++) {
            result(j, radius - i - 1) = result(j, radius + i);
        }
    }
    for (int i = 0; i < radius; i++) {
        for (int j = 0; j < int(image.n_rows) + 2 * radius; j ++) {
            result(j, radius + image.n_cols + i) = result(j, radius + image.n_cols - i - 1);
        }
    }

    return result;
}

// Exatract features from dataset.
// You should implement this function by yourself =)
void ExtractFeatures(const TDataSet& data_set, TFeatures* features) {
    for (size_t image_idx = 0; image_idx < data_set.size(); ++image_idx) {
        BMP * image = data_set[image_idx].first;
        int label = data_set[image_idx].second;
        Matrix<double> greyscale(image->TellHeight(), image->TellWidth());
        for (long long i = 0; i < image->TellHeight(); i++) {
            for (long long j = 0; j < image->TellWidth(); j++) {
                greyscale(i, j) = image->GetPixel(j, i).Red * 0.299 + image->GetPixel(j, i).Green * 0.587 + image->GetPixel(j, i).Blue * 0.114;
            }
        }
        int dist = ceil(std::max(greyscale.n_rows, greyscale.n_cols) / 8.0);
        greyscale = mirror(greyscale, dist);

        //HOG
        Matrix<double> kernel_w(1, 3);
        kernel_w(0, 0) = -1; kernel_w(0, 1) = 0; kernel_w(0, 2) = 1;
        Matrix<double> sobel_w = CustomFilter(greyscale, kernel_w);
        Matrix<double> kernel_h(3, 1);
        kernel_h(0, 0) = 1; kernel_h(1, 0) = 0; kernel_h(2, 0) = -1;
        Matrix<double> sobel_h = CustomFilter(greyscale, kernel_h);

        Matrix<double> modules(sobel_w.n_rows, sobel_w.n_cols);
        Matrix<double> angles(sobel_w.n_rows, sobel_w.n_cols);

        for (long long i = 0; i < sobel_w.n_rows; i++) {
            for (long long j = 0; j < sobel_w.n_cols; j++) {
                modules(i, j) = sqrt(pow(sobel_h(i, j) , 2) + pow(sobel_w(i, j), 2));
                angles(i, j) = sobel_w(i, j) < 0.000001 && sobel_h(i, j) < 0.000001 ? 0 : atan(sobel_h(i, j) / sobel_w(i, j));
            }
        }
        // 8 items on each side, 16 hist segments
        double items1 = 8;
        int hist_segments = 16;

        vector<float> total_hist{};
        int size_h = ceil((sobel_w.n_rows - 2 * dist) / items1);
        int size_w = ceil((sobel_w.n_cols - 2 * dist) / items1);
        for (long long i = dist; i < sobel_w.n_rows - dist; i += size_h) {
            for (long long j = dist; j < sobel_w.n_cols - dist; j += size_w) {
                vector<double> hist{};
                hist.resize(hist_segments);
                for (long long k = i; k < i + size_h; k++) {
                    for (long long l = j; l < j + size_w; l++) {
                        if (k >= sobel_w.n_rows || l >= sobel_w.n_cols) { continue; }
                        double spot = angles(k, l);
                        spot = spot / M_PI * hist_segments / 2;
                        spot = round(spot + hist_segments / 2);
                        spot = spot >= hist_segments / 2 ? hist_segments / 2 - 1 : spot;
                        spot = spot <= -hist_segments / 2 ? -hist_segments / 2 + 1 : spot;
                        hist[spot] += modules(k, l);
                    }
                }
                double normalizer = 0;
                for (long long k = 0; k < hist_segments; k++) {
                    normalizer += pow(hist[k], 2);
                }
                normalizer = sqrt(normalizer);

                if (normalizer > 0) {
                    for (long long k = 0; k < hist_segments; k++) {
                        hist[k] /= normalizer;
                    }
                }

                for (auto hh : hist) {
                    total_hist.push_back(hh);
                }
            }
        }

        //colors
        double items2 = 8;
        size_h = ceil(image->TellHeight() / items2);
        size_w = ceil(image->TellWidth() / items2);
        for (long long i = 0; i < image->TellHeight(); i += size_h) {
            for (long long j = 0; j < image->TellWidth(); j += size_w) {
                double red = 0, green = 0, blue = 0;
                for (long long k = i; k < i + size_h; k++) {
                    for (long long l = j; l < j + size_w; l++) {
                        red += image->GetPixel(j, i).Red;
                        green += image->GetPixel(j, i).Green;
                        blue += image->GetPixel(j, i).Blue;
                    }
                }
                red /= size_w * size_h;
                green /= size_w * size_h;
                blue /= size_w * size_h;

                total_hist.push_back(red / 255);
                total_hist.push_back(green / 255);
                total_hist.push_back(blue / 255);
            }
        }

        //LBP
        double items3 = 8;
        size_h = ceil((greyscale.n_rows - 2 * dist) / items3);
        size_w = ceil((greyscale.n_cols - 2 * dist) / items3);
        Matrix<long long> numbers(greyscale.n_rows, greyscale.n_cols);
        for (long long i = dist; i < greyscale.n_rows - dist; i++) {
            for (long long j = dist; j < greyscale.n_cols - dist; j++) {
                int number = 0;
                for (long long k = i - 1; k < i + 2; k++) {
                    for (long long l = j - 1; l < j + 2; l++) {
                        if (i != k || j != l) {
                            number = number << 1 | (greyscale(i, j) <= greyscale(k, l));
                        }
                    }
                }
                numbers(i, j) = number;
            }
        }
        for (long long i = dist; i < numbers.n_rows - dist; i += size_h) {
            for (long long j = dist; j < numbers.n_cols - dist; j += size_w) {
                vector<double> hist{};
                hist.resize(256);
                for (long long k = i; k < i + size_h; k++) {
                    for (long long l = j; l < j + size_w; l++) {
                        if (k >= numbers.n_rows - dist || l >= numbers.n_cols - dist ) { continue; }
                        hist[numbers(k, l)]++;
                    }
                }
                double normalizer = 0;
                for (long long k = 0; k < 256; k++) {
                    normalizer += pow(hist[k], 2);
                }
                normalizer = sqrt(normalizer);

                if (normalizer > 0) {
                    for (long long k = 0; k < 256; k++) {
                        hist[k] /= normalizer;
                    }
                }
                for (auto hh : hist) {
                    total_hist.push_back(hh);
                }
            }
        }
        total_hist.resize(items1 * items1 * hist_segments +
                          items2 * items2 * 3 +
                          items3 * items3 * 256);
        features->push_back(make_pair(total_hist, label));
    }
}

// Clear dataset structure
void ClearDataset(TDataSet* data_set) {
        // Delete all images from dataset
    for (size_t image_idx = 0; image_idx < data_set->size(); ++image_idx)
        delete (*data_set)[image_idx].first;
        // Clear dataset
    data_set->clear();
}

// Train SVM classifier using data from 'data_file' and save trained model
// to 'model_file'
void TrainClassifier(const string& data_file, const string& model_file) {
        // List of image file names and its labels
    TFileList file_list;
        // Structure of images and its labels
    TDataSet data_set;
        // Structure of features of images and its labels
    TFeatures features;
        // Model which would be trained
    TModel model;
        // Parameters of classifier
    TClassifierParams params;

        // Load list of image file names and its labels
    LoadFileList(data_file, &file_list);
        // Load images
    LoadImages(file_list, &data_set);
        // Extract features from images
    ExtractFeatures(data_set, &features);

    params.C = 0.3;
    TClassifier classifier(params);
        // Train classifier
    classifier.Train(features, &model);
        // Save model to file
    model.Save(model_file);
        // Clear dataset structure
    ClearDataset(&data_set);
}

// Predict data from 'data_file' using model from 'model_file' and
// save predictions to 'prediction_file'
void PredictData(const string& data_file,
                 const string& model_file,
                 const string& prediction_file) {
        // List of image file names and its labels
    TFileList file_list;
        // Structure of images and its labels
    TDataSet data_set;
        // Structure of features of images and its labels
    TFeatures features;
        // List of image labels
    TLabels labels;

        // Load list of image file names and its labels
    LoadFileList(data_file, &file_list);
        // Load images
    LoadImages(file_list, &data_set);
        // Extract features from images
    ExtractFeatures(data_set, &features);

        // Classifier
    TClassifier classifier = TClassifier(TClassifierParams());
        // Trained model
    TModel model;
        // Load model from file
    model.Load(model_file);
        // Predict images by its features using 'model' and store predictions
        // to 'labels'
    classifier.Predict(features, model, &labels);

        // Save predictions
    SavePredictions(file_list, labels, prediction_file);
        // Clear dataset structure
    ClearDataset(&data_set);
}

int main(int argc, char** argv) {
    // Command line options parser
    ArgvParser cmd;
        // Description of program
    cmd.setIntroductoryDescription("Machine graphics course, task 2. CMC MSU, 2014.");
        // Add help option
    cmd.setHelpOption("h", "help", "Print this help message");
        // Add other options
    cmd.defineOption("data_set", "File with dataset",
        ArgvParser::OptionRequiresValue | ArgvParser::OptionRequired);
    cmd.defineOption("model", "Path to file to save or load model",
        ArgvParser::OptionRequiresValue | ArgvParser::OptionRequired);
    cmd.defineOption("predicted_labels", "Path to file to save prediction results",
        ArgvParser::OptionRequiresValue);
    cmd.defineOption("train", "Train classifier");
    cmd.defineOption("predict", "Predict dataset");

        // Add options aliases
    cmd.defineOptionAlternative("data_set", "d");
    cmd.defineOptionAlternative("model", "m");
    cmd.defineOptionAlternative("predicted_labels", "l");
    cmd.defineOptionAlternative("train", "t");
    cmd.defineOptionAlternative("predict", "p");

        // Parse options
    int result = cmd.parse(argc, argv);

        // Check for errors or help option
    if (result) {
        cout << cmd.parseErrorDescription(result) << endl;
        return result;
    }

        // Get values
    string data_file = cmd.optionValue("data_set");
    string model_file = cmd.optionValue("model");
    bool train = cmd.foundOption("train");
    bool predict = cmd.foundOption("predict");

        // If we need to train classifier
    if (train)
        TrainClassifier(data_file, model_file);
        // If we need to predict data
    if (predict) {
            // You must declare file to save images
        if (!cmd.foundOption("predicted_labels")) {
            cerr << "Error! Option --predicted_labels not found!" << endl;
            return 1;
        }
            // File to save predictions
        string prediction_file = cmd.optionValue("predicted_labels");
            // Predict data
        PredictData(data_file, model_file, prediction_file);
    }
}
