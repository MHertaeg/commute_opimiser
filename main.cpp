// wxWidgets "Hello World" Program
#include "header.h"




    int main(int argc, char** argv) {
        if (1 == 0)
        {
            MapBounds mapBounds = {
                { -37.673467, 144.900807 }, // topLeft
                { -37.675475, 145.244541 }, // topRight
                { -37.933589, 144.913295 }, // bottomLeft
                { -37.931713, 145.237128 }  // bottomRight
               };
            const std::string filename_input = "C:/Users/michael.h_chemwatch/source/repos/commute/test_coords.csv";
            std::vector<coordinate_input> m_inputData;
            m_inputData = read_csv(filename_input);
            GPSCoordinate topLeft = mapBounds.topLeft;
            GPSCoordinate topRight = mapBounds.topRight;
            GPSCoordinate bottomLeft = mapBounds.bottomLeft;
            GPSCoordinate bottomRight = mapBounds.bottomRight;
            PixelCoordinate MouseClick = { 0, 0 };

            GPSCoordinate gps = pixelToGPS(MouseClick, 1000, 500, topLeft, topRight, bottomLeft, bottomRight);
            //std::cout << gps.latitude << " , " << gps.longitude << std::endl;
            

            //double value = interpolate(m_inputData, gps.latitude, gps.longitude);
            double value = linear_interpolate_closest_points(m_inputData, -38.34, 145.42);

            std::cout << value;
        }
        if (1 == 1)
        {
            wxApp::SetInstance(new MyApp());
            return wxEntry(argc, argv);
        }
    };
