// wxWidgets "Hello World" Program
#include "header.h"

struct pixel_data {
    double value;
    double colour;

};



int main(int argc, char** argv) {
    const std::string filename_input = "C:/Users/michael.h_chemwatch/source/repos/commute/test_coords.csv";
    int image_width = 1448;
    int image_height = 1340;
    //update
    //std::vector<DataPoint> data = readCSV(filename);
    std::vector<coordinate_input> input_data;
    input_data = read_csv(filename_input);
    for (int i = 0; i < input_data.size(); i++)
    {
        std::cout << input_data[i].longitude << std::endl;
    }
    wxApp::SetInstance(new MyApp());
    return wxEntry(argc, argv);
}
