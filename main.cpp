// wxWidgets "Hello World" Program
#include "header.h"



int main(int argc, char** argv) {
    const std::string filename = "C:/Users/michael.h_chemwatch/source/repos/commute/test_coords.csv";
    //std::vector<DataPoint> data = readCSV(filename);
    std::vector<coordinate_input> input_data;
    input_data = read_csv(filename);
    for (int i = 0; i < input_data.size(); i++)
    {
        std::cout << input_data[i].longitude << std::endl;
    }
    wxApp::SetInstance(new MyApp());
    return wxEntry(argc, argv);
}
